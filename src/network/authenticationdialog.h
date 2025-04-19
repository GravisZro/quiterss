#ifndef AUTHENTICATIONDIALOG_H
#define AUTHENTICATIONDIALOG_H

#include <QAuthenticator>
#include <QNetworkReply>

#include "dialog.h"
#include "lineedit.h"

class AuthenticationDialog : public Dialog
{
  Q_OBJECT

public:
  explicit AuthenticationDialog(const QUrl &url,
                                QAuthenticator *auth,
                                QWidget *parent = 0);
  QCheckBox* save_;

private slots:
  void acceptDialog();

private:
  QAuthenticator *auth_;

  QString server_;
  LineEdit *user_;
  LineEdit *pass_;

};

#endif // AUTHENTICATIONDIALOG_H
