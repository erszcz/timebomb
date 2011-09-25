#include <QtCore>
#include <QAction>

#include "project.h"

//
#include <QtDebug>
//

Project::Project(const QFileInfo& fInfo, time_t _total)
  : QObject(),
    toggleCounterAction(0),
    fInfo(fInfo),
    secondsId(0),
    autoSaveId(0),
    _sinceLastBreak(0),
    _thisSession(0),
    _total(_total)
{
  toggleCounterAction = new QAction(tr("Start/stop"), this);
  toggleCounterAction->setCheckable(true);
  toggleCounterAction->setChecked(false);

  connect(toggleCounterAction, SIGNAL(toggled(bool)),
          this, SLOT(toggleCounter(bool)));
}

Project::Project(const Project& other)
  : QObject(),
    toggleCounterAction(0),
    fInfo(other.fInfo),
    secondsId(0),
    autoSaveId(0),
    _sinceLastBreak(other._sinceLastBreak),
    _thisSession(other._thisSession),
    _total(other._total)
{
  toggleCounterAction = new QAction(tr("Start/Stop"), this);
  toggleCounterAction->setCheckable(true);
  toggleCounterAction->setChecked(other.toggleCounterAction->isChecked());

  connect(toggleCounterAction, SIGNAL(toggled(bool)),
          this, SLOT(toggleCounter(bool)));
}

Project* Project::fromFile(const QFileInfo& info)
{
  return Project::fromFile(info.filePath());
}

Project* Project::fromFile(const QString& path)
{
  QFile file(path);
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream in(&file);
  QString time;
  in >> time;
  file.close();

  time_t _total = time.section(':', 0, 0).toULong() * SECS_DAY;
  QTime qt = QTime::fromString(time.section(':', 1), "hh:mm:ss");
  _total += qt.hour() * SECS_HOUR;
  _total += qt.minute() * SECS_MIN;
  _total += qt.second();

//  qDebug() << QString::number(_total);

  return new Project(QFileInfo(path), _total);
}

void Project::saveToFile() const
{
//  qDebug() << QString("Project::save: method stub");

  QFile file(fInfo.filePath());
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&file);

  out << total() << "\n";

  file.close();
}

void Project::deleteFile() const
{
//  qDebug() << QString("Project::delete_: method stub");

  QFile file(fInfo.filePath());
  if (file.exists())
  {
//    qDebug() << "file " + fInfo.filePath() + " exists -- deleting";
    file.remove();
  }
}

QString Project::name() const
{
  return fInfo.baseName();
}

void Project::toggleCounter(bool checked)
{
  switch (checked)
  {
    case true:
      if (secondsId != 0)
        throw CounterRunning;
      _sinceLastBreak = 0;
      secondsId = startTimer(1000); // schedule timerEvent every 1000 miliseconds
      autoSaveId = startTimer(60000); // schedule save every minute
      break;
    case false:
      killTimer(secondsId);
      secondsId = 0;
      killTimer(autoSaveId);
      autoSaveId = 0;
      break;
  }
}

bool Project::running() const
{
  return secondsId != 0;
}

QString Project::sinceLastBreak() const
{
  QString lbreak;
  QTextStream out(&lbreak, QIODevice::WriteOnly | QIODevice::Text);

  out << QTime().addSecs(_sinceLastBreak % SECS_DAY).toString("hh:mm:ss");

  return lbreak;
}

QString Project::thisSession() const
{
  QString session;
  QTextStream out(&session, QIODevice::WriteOnly | QIODevice::Text);

  out << QTime().addSecs(_thisSession % SECS_DAY).toString("hh:mm:ss");

  return session;
}

QString Project::total() const
{
  QString total;
  QTextStream out(&total, QIODevice::WriteOnly | QIODevice::Text);

  out << QString::number(_total / SECS_DAY) + ":";
  out << QTime().addSecs(_total % SECS_DAY).toString("hh:mm:ss");

  return total;
}

void Project::timerEvent(QTimerEvent* event)
{
  int id = event->timerId();
  if (id == secondsId)
  {
    ++_sinceLastBreak;
    ++_thisSession;
    ++_total;
    emit counterTicked();
//    qDebug() << "project: " + name();
//    qDebug() << "_sinceLastBreak: " + QString::number(_sinceLastBreak);
//    qDebug() << "_thisSession: " + QString::number(_thisSession);
//    qDebug() << "_total: " + QString::number(_total);
//    qDebug();
  }
  else if (id == autoSaveId)
    saveToFile();
//  else
//    qDebug() << "Project::timerEvent: unknown timerEvent received";
}
