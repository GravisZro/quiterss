#ifndef FILTERRULES_H
#define FILTERRULES_H

#include "dialog.h"
#include "itemcondition.h"
#include "itemaction.h"

class FilterRulesDialog : public Dialog
{
  Q_OBJECT
public:
  explicit FilterRulesDialog(QWidget *parent, int filterId, int feedId = -1);

  LineEdit *filterName_;
  int filterId_;
  bool itemNotChecked_;

private slots:
  void feedItemChanged(QTreeWidgetItem *item, int column);
  void setCheckStateItem(QTreeWidgetItem *item, Qt::CheckState state);
  void closeDialog();
  void acceptDialog();

  void filterNameChanged(const QString &text);

  ItemCondition *addCondition();
  void deleteCondition(ItemCondition *item);

  ItemAction *addAction();
  void deleteAction(ItemAction *item);

  void selectMatch(int index);

private:
  void setData();

  QTreeWidget *feedsTree_;

  QComboBox *matchComboBox_;

  QScrollArea *conditionScrollArea_;
  QVBoxLayout *conditionLayout_;
  QWidget *conditionWidget_;

  QScrollArea *actionsScrollArea_;
  QVBoxLayout *actionsLayout_;
  QWidget *actionsWidget_;

  QLabel *textWarning_;
  QWidget *warningWidget_;

};

#endif // FILTERRULES_H
