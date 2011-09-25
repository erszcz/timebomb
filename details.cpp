#include <QtGui>
#include "project.h"
#include "details.h"

DetailsDialog::DetailsDialog(Project* project,
                             QString runningSince,
                             QWidget* parent,
                             Qt::WindowFlags f)
  : QDialog(parent, f),
    project(project),
    titleLabel(NULL),
    totalLabel(NULL),
    thisSessionLabel(NULL),
    sinceLastBreakLabel(NULL)
{
  setWindowTitle("Details of: " + project->name());
  QGridLayout* layout = new QGridLayout;

  QLabel* runningConstLabel         = new QLabel(tr("Running since:"));
  QLabel* titleConstLabel           = new QLabel(tr("Project:"));
  QLabel* totalConstLabel           = new QLabel(tr("Total time:"));
  QLabel* thisSessionConstLabel     = new QLabel(tr("This session:"));
  QLabel* sinceLastBreakConstLabel  = new QLabel(tr("Since last break:"));
  
  QLabel* runningLabel              = new QLabel(runningSince);

  titleLabel          = new QLabel(project->name());
  totalLabel          = new QLabel(project->total());
  thisSessionLabel    = new QLabel(project->thisSession());
  sinceLastBreakLabel = new QLabel(project->sinceLastBreak());

//  layout->addWidget(runningConstLabel,        0, 0, Qt::AlignRight);
  layout->addWidget(runningConstLabel,        0, 0);
  layout->addWidget(runningLabel,             0, 1);
//  layout->addWidget(titleConstLabel,          1, 0, Qt::AlignRight);
  layout->addWidget(titleConstLabel,          1, 0);
  layout->addWidget(titleLabel,               1, 1);
//  layout->addWidget(totalConstLabel,          2, 0, Qt::AlignRight);
  layout->addWidget(totalConstLabel,          2, 0);
  layout->addWidget(totalLabel,               2, 1);
//  layout->addWidget(thisSessionConstLabel,    3, 0, Qt::AlignRight);
  layout->addWidget(thisSessionConstLabel,    3, 0);
  layout->addWidget(thisSessionLabel,         3, 1);
//  layout->addWidget(sinceLastBreakConstLabel, 4, 0, Qt::AlignRight);
  layout->addWidget(sinceLastBreakConstLabel, 4, 0);
  layout->addWidget(sinceLastBreakLabel,      4, 1);

  toggler = new QPushButton;
  toggler->setCheckable(true);
  updateToggler(project->running());

  layout->addWidget(toggler, 5, 0, 1, 2);

  setLayout(layout);

  setupConnections();
}

void DetailsDialog::setupConnections()
{
  connect(project, SIGNAL(counterTicked()),
          this, SLOT(updateLabels()));

  // toggler connections
  connect(project->toggleCounterAction, SIGNAL(toggled(bool)),
          this, SLOT(updateToggler(bool)));
  connect(toggler, SIGNAL(toggled(bool)),
          project->toggleCounterAction, SLOT(setChecked(bool)));
}

void DetailsDialog::updateLabels()
{
  totalLabel->setText(project->total());
  thisSessionLabel->setText(project->thisSession());
  sinceLastBreakLabel->setText(project->sinceLastBreak());
}

void DetailsDialog::updateToggler(bool running)
{
  if (running)
  {
    toggler->setIcon(QIcon(":/gfx/stop.png"));
    toggler->setChecked(true);
  }
  else
  {
    toggler->setIcon(QIcon(":/gfx/start.png"));
    toggler->setChecked(false);
  }
}
