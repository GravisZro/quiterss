#ifndef FAVICONOBJECT_H
#define FAVICONOBJECT_H

#include <QDateTime>
#include <QObject>
#include <QQueue>
#include <QNetworkReply>
#include <QTimer>

#include "networkmanager.h"

class FaviconObject : public QObject
{
  Q_OBJECT

public:
  explicit FaviconObject(QObject *parent = 0);

  void disconnectObjects();

public slots:
  void requestUrl(QString urlString, QString feedUrl);
  void slotGet(const QUrl &getUrl, const QString &feedUrl, const int &count);

signals:
  void startTimer();
  void signalGet(const QUrl &getUrl, const QString &feedUrl, const int &count);
  void signalIconRecived(QString feedUrl, QByteArray byteArray, QString format);

private slots:
  void getQueuedUrl();
  void finished(QNetworkReply *reply);
  void slotRequestTimeout();

private:
  NetworkManager *networkManager_;

  QQueue<QString> urlsQueue_;
  QQueue<QString> feedsQueue_;

  QTimer *timeout_;
  QTimer *getUrlTimer_;
  QList<QUrl> currentUrls_;
  QList<QString> currentFeeds_;
  QList<int> currentCntRequests_;
  QList<int> currentTime_;
  QList<QUrl> requestUrl_;
  QList<QNetworkReply*> networkReply_;
  QList<QString> hostList_;

};

#endif // FAVICONOBJECT_H
