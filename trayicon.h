#ifndef TRAYICON_H
#define TRAYICON_H

#include <QtCore>
#include <QtGui>
#include <QSystemTrayIcon>

#include "details.h"
#include "project.h"
#include "selectaction.h"

class TrayIcon: public QSystemTrayIcon
{
  Q_OBJECT

  public:
    TrayIcon();
    virtual ~TrayIcon();

  signals:
    void projectListChanged();

  private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void selectProject(int idx);
    void updateCounterView(bool running);
    void updateToolTip();

    // action callbacks

    /* Create a new project and set it as the current project. */
    void newProject();
    void deleteCurrentProject();
    /* Show a dialog with detailed information about current project. */
    void currentProjectDetails();
    void exit();

    void setupMenu();

  private:
    void setupActions();
    void setupConnections();
    void setupProjects();

    void setCurrentProject(Project*, bool delete_=false);

    QMenu* menu;

    QPointer<DetailsDialog> details;

    QIcon  offIcon;
    QIcon  onIcon;

    QList<Project*> projects;
    Project*        currentProject;

    QString runningSince;

    // actions
    QAction* newProjectAction;
    QAction* deleteCurrentProjectAction;
    QAction* currentProjectDetailsAction;
    QAction* exitAction;
};

#endif // TRAYICON_H
