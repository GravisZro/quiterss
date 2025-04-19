#ifndef NETWORKMANAGERPROXY_H
#define NETWORKMANAGERPROXY_H

#include <QNetworkAccessManager>

#include "networkmanager.h"

class WebPage;

class NetworkManagerProxy : public QNetworkAccessManager
{
  Q_OBJECT
public:
  explicit NetworkManagerProxy(WebPage* page, QObject* parent = 0);

  QNetworkReply* createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice* outgoingData);

  void disconnectObjects();

private slots:
  void slotSslError(QNetworkReply *reply, QList<QSslError> errors);

private:
  WebPage* page_;

};

#endif // NETWORKMANAGERPROXY_H
