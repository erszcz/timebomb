#ifndef SELECTACTION_H
#define SELECTACTION_H

#include <QtGui>

class SelectAction: public QAction
{
  Q_OBJECT

  public:
    SelectAction(const QString& text, int id, QObject* parent);

  signals:
    void selected(int);

  private slots:
    void select();

  private:
    int id;
};

#endif // SELECTACTION_H
