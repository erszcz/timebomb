#ifndef DETAILS_H
#define DETAILS_H

#include <QtGui>
#include "project.h"

class DetailsDialog: public QDialog
{
  Q_OBJECT

  public:
    DetailsDialog(Project* project,
                  QString runningSince,
                  QWidget* parent=0,
                  Qt::WindowFlags f=0);

    QPushButton* toggler;

  private slots:
    void updateLabels();
    void updateToggler(bool running);

  private:
    void setupConnections();

    Project* project;

    QLabel* titleLabel;
    QLabel* totalLabel;
    QLabel* thisSessionLabel;
    QLabel* sinceLastBreakLabel;
};

#endif // DETAILS_H
