#include "details.h"
#include "trayicon.h"

//
//#include <QtDebug>
//

TrayIcon::TrayIcon()
  : QSystemTrayIcon(),
    menu(NULL),
    offIcon(":/gfx/off.png"),
    onIcon(":/gfx/on.png"),
    currentProject(NULL),
    runningSince(QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate))
{
  setIcon(offIcon);
  setVisible(true);

  setupActions();
  setupConnections();
  setupProjects();
  // commented out because it's handled by a signal
  //setupMenu();

  setCurrentProject(NULL);
}

TrayIcon::~TrayIcon()
{
  QList<Project*>::iterator i;
  for (i = projects.begin(); i != projects.end(); ++i)
  {
    (*i)->saveToFile();
    delete *i;
  }
}

void TrayIcon::setupActions()
{
  newProjectAction = new QAction(tr("New project"), this);
  connect(newProjectAction, SIGNAL(triggered()),
          this, SLOT(newProject()));

  deleteCurrentProjectAction = new QAction(tr("Delete this project"), this);
  connect(deleteCurrentProjectAction, SIGNAL(triggered()),
          this, SLOT(deleteCurrentProject()));

  currentProjectDetailsAction = new QAction(tr("Project details"), this);
  connect(currentProjectDetailsAction, SIGNAL(triggered()),
          this, SLOT(currentProjectDetails()));

  exitAction = new QAction(tr("Exit"), this);
  connect(exitAction, SIGNAL(triggered()), this, SLOT(exit()));
}

void TrayIcon::setupConnections()
{
  // systray icon clicked
  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  // project list changed
  connect(this, SIGNAL(projectListChanged()),
          this, SLOT(setupMenu()));
}

void TrayIcon::setupProjects()
{
  QDir dir(QDir::currentPath() + "/projects");
  QFileInfoList files(dir.entryInfoList(QDir::Files | 
                                        QDir::NoDotAndDotDot | 
                                        QDir::Readable,
                                        QDir::Name |
                                        QDir::IgnoreCase));
  for (QFileInfoList::iterator i = files.begin(); i != files.end(); ++i)
    projects.append(Project::fromFile(*i));
  emit projectListChanged();
}

void TrayIcon::setupMenu()
{
  if (menu)
    delete menu;
  menu = new QMenu;

  // dynamic actions -- these correspond to existing projects
  SelectAction* action;
  for (int i = 0; i < projects.count(); ++i)
  {
    action = new SelectAction(projects[i]->name(), i, this);
    connect(action, SIGNAL(selected(int)),
            this, SLOT(selectProject(int)));
    menu->addAction(action);
  }

  // static actions
  menu->addSeparator();
  menu->addAction(newProjectAction);
  menu->addAction(deleteCurrentProjectAction);
  menu->addAction(currentProjectDetailsAction);
  menu->addSeparator();
  menu->addAction(exitAction);
}

void TrayIcon::updateCounterView(bool running)
{
  switch (running)
  {
    case true:
//      qDebug() << "TrayIcon::update: 'running' is true; setting icon 'onIcon'";
      setIcon(onIcon);
      break;
    case false:
//      qDebug() << "TrayIcon::update: 'running' is false; setting icon 'offIcon'";
      setIcon(offIcon);
      break;
  }
}

void TrayIcon::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason)
  {
    case QSystemTrayIcon::Trigger: // single left click
      if (currentProject and currentProject->running())
      {
//        qDebug() << QString("stopping counter");
        currentProject->toggleCounterAction->setChecked(false);
      }
      else if (currentProject)
      {
//        qDebug() << QString("starting counter");
        currentProject->toggleCounterAction->setChecked(true);
      }
      break;
    case QSystemTrayIcon::Context: // right click -- context menu
//      qDebug() << QString("show context menu");
      menu->exec(QCursor::pos());
      break;
  }
}

void TrayIcon::setCurrentProject(Project* new_, bool delete_)
{
  if (currentProject)
  {
    currentProject->toggleCounterAction->setChecked(false);
    if (! delete_)
      currentProject->saveToFile();
    currentProject->disconnect();
    currentProject->toggleCounterAction->disconnect();

    if (details)
      details->toggler->setEnabled(false);
  }

  if (delete_)
    delete currentProject;
  currentProject = new_;
  if (currentProject)
  {
    // enable menu actions
    deleteCurrentProjectAction->setEnabled(true);
    currentProjectDetailsAction->setEnabled(true);

    // setup connection(s)
    connect(currentProject, SIGNAL(counterTicked()),
            this, SLOT(updateToolTip()));
    connect(currentProject->toggleCounterAction, SIGNAL(toggled(bool)),
            this, SLOT(updateCounterView(bool)));

    updateToolTip();
  }
  else
  {
    deleteCurrentProjectAction->setEnabled(false);
    currentProjectDetailsAction->setEnabled(false);
    setToolTip(tr("No project set!"));
  }
}

void TrayIcon::selectProject(int idx)
{
//  qDebug() << "TrayIcon::selectProject: " + QString::number(idx);

  setCurrentProject(projects[idx]);
}

void TrayIcon::updateToolTip()
{
  setToolTip(tr("Project: ") + currentProject->name() + ", " +
             currentProject->thisSession() + tr(" this session, ") +
             currentProject->sinceLastBreak() + tr(" since last break."));
}

void TrayIcon::newProject()
{
//  qDebug() << QString("TrayIcon::newProject: method stub");

  if (! QDir(QDir::currentPath() + "/projects").exists())
    QDir().mkdir("projects");
  QString filename = QFileDialog::getSaveFileName(
    NULL,
    tr("New project"),
    QDir::currentPath() + QString("/projects")
  );
//  qDebug() << filename;
  Project* new_ = new Project(QFileInfo(filename));
  projects.append(new_);
  setCurrentProject(new_);
  emit projectListChanged();
}

void TrayIcon::deleteCurrentProject()
{
//  qDebug() << QString("TrayIcon::deleteCurrentProject: method stub");
  if (currentProject)
  {
    // delete for sure?
    QMessageBox::StandardButton result =
      QMessageBox::question(0, tr("Delete project?"),
        tr("Really delete project ") + currentProject->name() + "?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (result != QMessageBox::Yes)
      return;

    currentProject->deleteFile();
    projects.removeAt(projects.indexOf(currentProject));
    setCurrentProject(NULL, true);
    emit projectListChanged();
  }
}

void TrayIcon::currentProjectDetails()
{
//  qDebug() << QString("TrayIcon::currentProjectDetails: method stub");

  details = new DetailsDialog(currentProject, runningSince);
  details->show();
}

void TrayIcon::exit()
{
//  qDebug() << QString("TrayIcon::exit: method stub");
  // destructor handles saving all projects:
//  if (currentProject)
//    currentProject->saveToFile();
  qApp->quit();
}
