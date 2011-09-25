#ifndef PROJECT_H
#define PROJECT_H

#include <QtCore>
#include <QAction>
#include <ctime>

enum ProjectError { CounterRunning };

class Project: public QObject
{
  Q_OBJECT

  public:
    Project(const QFileInfo& fInfo, time_t total=0);
    Project(const Project&);

    /* Construct a project from data in file pointed by 'path'. */
    static Project* fromFile(const QString& path);
    static Project* fromFile(const QFileInfo&);

    /* Save project to file 'dir/name'. */
    void saveToFile() const;

    /* Delete file corresponding to this project. */
    void deleteFile() const;

    /* Return project name. */
    QString name() const;

    /* Start and stop the builtin counter.
       Project info is updated only when the counter is running.
    */
    QAction* toggleCounterAction;
    bool running() const;

    QString sinceLastBreak() const;
    QString thisSession() const;
    QString total() const;
  
  public slots:
    void toggleCounter(bool);

  signals:
    void counterTicked();

  private:
    QFileInfo fInfo;

    /* Ensures that stored times are up to date. */
    void timerEvent(QTimerEvent* event);

    int secondsId;
    int autoSaveId;
    time_t _sinceLastBreak;  // number of seconds
    time_t _thisSession;
    time_t _total;

    QTime _dt;

    // seconds per X
    static const time_t SECS_DAY   = 86400;
    static const time_t SECS_HOUR  = 3600;
    static const time_t SECS_MIN   = 60;
};

#endif // PROJECT_H
