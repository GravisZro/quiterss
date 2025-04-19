#ifndef ITEMACTION_H
#define ITEMACTION_H

#include <QtWidgets>

#include "lineedit.h"
#include "toolbutton.h"

class ItemAction : public QWidget
{
  Q_OBJECT
public:
  explicit ItemAction(QWidget * parent = 0);

  QComboBox *comboBox1_;
  QComboBox *comboBox2_;
  QWidget *soundWidget_;
  QLineEdit *soundPathEdit_;
  ToolButton *selectionSound_;
  ToolButton *playSound_;
  ToolButton *colorButton_;

  ToolButton *addButton_;

signals:
  void signalDeleteAction(ItemAction *item);
  void signalPlaySound(const QString &soundPath);

private slots:
  void deleteFilterAction();
  void currentIndexChanged(int index);
  void selectionSound();
  void slotPlaySound();
  void selectColorText();

private:
  ToolButton *deleteButton_;

};

#endif // ITEMACTION_H
