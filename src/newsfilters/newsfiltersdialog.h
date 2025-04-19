#ifndef NEWSFILTERSDIALOG_H
#define NEWSFILTERSDIALOG_H

#include <QTreeWidget>

#include "dialog.h"

class NewsFiltersDialog : public Dialog
{
  Q_OBJECT
public:
  explicit NewsFiltersDialog(QWidget *parent);
  QTreeWidget *filtersTree_;

private slots:
  void closeDialog();
  void newFilter();
  void editFilter();
  void deleteFilter();

  void moveUpFilter();
  void moveDownFilter();

  void slotCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *);
  void slotItemChanged(QTreeWidgetItem *item,int column);

  void applyFilter();

private:
  QPushButton *editButton_;
  QPushButton *deleteButton_;
  QPushButton *moveUpButton_;
  QPushButton *moveDownButton_;
  QPushButton *runFilterButton_;

};

#endif // NEWSFILTERSDIALOG_H
