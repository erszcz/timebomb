#include <QtCore>

#include "selectaction.h"

SelectAction::SelectAction(const QString& text, int id, QObject* parent=0)
  : QAction(text, parent),
    id(id)
{
  connect(this, SIGNAL(triggered()),
          this, SLOT(select()));
}

void SelectAction::select()
{
  emit selected(id);
}
