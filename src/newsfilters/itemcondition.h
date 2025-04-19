#ifndef ITEMCONDITION_H
#define ITEMCONDITION_H

#include <QtWidgets>

#include "lineedit.h"
#include "toolbutton.h"

class ItemCondition : public QWidget
{
  Q_OBJECT
public:
  explicit ItemCondition(QWidget * parent = 0);

  QComboBox *comboBox1_;
  QComboBox *comboBox2_;
  QComboBox *comboBox3_;
  LineEdit *lineEdit_;

  ToolButton *addButton_;

signals:
  void signalDeleteCondition(ItemCondition *item);

private slots:
  void deleteFilterRules();
  void currentIndexChanged(const QString &str);

private:
  ToolButton *deleteButton_;

};

#endif // ITEMCONDITION_H
