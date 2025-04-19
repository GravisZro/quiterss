#ifndef DIALOG_H
#define DIALOG_H

#include <QtWidgets>

class Dialog : public QDialog
{
  Q_OBJECT
public:
  explicit Dialog(QWidget *parent, Qt::WindowFlags flag = Qt::WindowFlags());

  QVBoxLayout *mainLayout;
  QVBoxLayout *pageLayout;
  QHBoxLayout *buttonsLayout;

  QDialogButtonBox *buttonBox;

};

#endif // DIALOG_H
