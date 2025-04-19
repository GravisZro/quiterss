#ifndef SSLERRORDIALOG_H
#define SSLERRORDIALOG_H

#include "dialog.h"

#include <QSslCertificate>

class SslErrorDialog : public Dialog
{
  Q_OBJECT

public:
  enum Result { Yes, No, OnlyForThisSession };

  explicit SslErrorDialog(QWidget* parent = 0);
  ~SslErrorDialog();

  void setText(const QString &text);
  Result result();

  static QString certificateItemText(const QSslCertificate &cert);
  static QString clearCertSpecialSymbols(const QString &string);
  static QString clearCertSpecialSymbols(const QStringList &stringList);

private slots:
  void buttonClicked(QAbstractButton* button);

private:
  QLabel *errorLabel_;
  Result result_;

};

#endif // SSLERRORDIALOG_H
