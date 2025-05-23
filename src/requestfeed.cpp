#include "requestfeed.h"
#include "VersionNo.h"
#include "mainapplication.h"
#include "globals.h"
#include "common.h"

#include <QDebug>
#include <QtSql>
#include <qzregexp.h>

#define REPLY_MAX_COUNT 10

RequestFeed::RequestFeed(int timeoutRequest, int numberRequests,
                         int numberRepeats, QObject *parent)
  : QObject(parent)
  , networkManager_(NULL)
  , timeoutRequest_(timeoutRequest)
  , numberRequests_(numberRequests)
  , numberRepeats_(numberRepeats)
{
  setObjectName("requestFeed_");

  timeout_ = new QTimer(this);
  timeout_->setInterval(1000);
  connect(timeout_, SIGNAL(timeout()), this, SLOT(slotRequestTimeout()));

  getUrlTimer_ = new QTimer(this);
  getUrlTimer_->setSingleShot(true);
  getUrlTimer_->setInterval(50);
  connect(getUrlTimer_, SIGNAL(timeout()), this, SLOT(getQueuedUrl()));

  connect(this, SIGNAL(signalHead(QUrl,int,QString,QDateTime,int)),
          SLOT(slotHead(QUrl,int,QString,QDateTime,int)),
          Qt::QueuedConnection);
  connect(this, SIGNAL(signalGet(QUrl,int,QString,QDateTime,int)),
          SLOT(slotGet(QUrl,int,QString,QDateTime,int)),
          Qt::QueuedConnection);
}

RequestFeed::~RequestFeed()
{

}

void RequestFeed::disconnectObjects()
{
  disconnect(this);
  if (networkManager_)
    networkManager_->disconnect(networkManager_);
}

/** @brief Put URL in request queue
 *----------------------------------------------------------------------------*/
void RequestFeed::requestUrl(int id, QString urlString,
                              QDateTime date, QString userInfo)
{
  if (!networkManager_) {
    networkManager_ = new NetworkManager(true, this);
    connect(networkManager_, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(finished(QNetworkReply*)));
  }

  if (!timeout_->isActive())
    timeout_->start();

  idsQueue_.enqueue(id);
  feedsQueue_.enqueue(urlString);
  dateQueue_.enqueue(date);
  userInfo_.enqueue(userInfo);

  if (!getUrlTimer_->isActive())
    getUrlTimer_->start();

  qDebug() << "requestUrl() <<" << urlString << "countQueue=" << feedsQueue_.count();
}

void RequestFeed::stopRequest()
{
  while (!feedsQueue_.isEmpty()) {
    int feedId = idsQueue_.dequeue();
    QString feedUrl = feedsQueue_.dequeue();
    dateQueue_.clear();
    userInfo_.clear();

    emit getUrlDone(feedsQueue_.count(), feedId, feedUrl);
  }
}

/** @brief Process request queue on timer timeouts
 *----------------------------------------------------------------------------*/
void RequestFeed::getQueuedUrl()
{
  if ((currentFeeds_.count() >= numberRequests_) ||
      (currentFeeds_.count() >= REPLY_MAX_COUNT)) {
    getUrlTimer_->start();
    return;
  }

  if (!feedsQueue_.isEmpty()) {
    getUrlTimer_->start();
    QString feedUrl = feedsQueue_.head();

    if (hostList_.contains(QUrl(feedUrl).host())) {
      foreach (QString url, currentFeeds_) {
        if (QUrl(url).host() == QUrl(feedUrl).host()) {
          return;
        }
      }
    }
    int feedId = idsQueue_.dequeue();
    feedUrl = feedsQueue_.dequeue();

    emit setStatusFeed(feedId, "1 Update");

    QUrl getUrl = QUrl::fromEncoded(feedUrl.toUtf8());
    QString userInfo = userInfo_.dequeue();
    if (!userInfo.isEmpty()) {
      getUrl.setUserInfo(userInfo);
//      getUrl.addQueryItem("auth", getUrl.scheme());
    }

    qDebug() << "getQueuedUrl() >>" << feedUrl << "countQueue=" << feedsQueue_.count();
    QDateTime currentDate = dateQueue_.dequeue();
    if (currentDate.isValid())
      emit signalHead(getUrl, feedId, feedUrl, currentDate);
    else
      emit signalGet(getUrl, feedId, feedUrl, currentDate);
  }
}

/** @brief Prepare and send network request to get head
 *----------------------------------------------------------------------------*/
void RequestFeed::slotHead(const QUrl &getUrl, const int &id, const QString &feedUrl,
                            const QDateTime &date, const int &count)
{
  if (count)
    Common::sleep(30);

  qDebug() << objectName() << "::head:" << getUrl.toEncoded() << "feed:" << feedUrl << "countRepeats:" << count;
  QNetworkRequest request(getUrl);
  request.setRawHeader("User-Agent", globals.userAgent().toUtf8());

  currentUrls_.append(getUrl);
  currentIds_.append(id);
  currentFeeds_.append(feedUrl);
  currentDates_.append(date);
  currentCount_.append(count);
  currentHead_.append(true);
  currentTime_.append(timeoutRequest_);

  QNetworkReply *reply = networkManager_->head(request);
  reply->setProperty("feedReply", QVariant(true));
  requestUrl_.append(reply->url());
  networkReply_.append(reply);
}

/** @brief Prepare and send network request to get all data
 *----------------------------------------------------------------------------*/
void RequestFeed::slotGet(const QUrl &getUrl, const int &id, const QString &feedUrl,
                           const QDateTime &date, const int &count)
{
  if (count)
    Common::sleep(30);

  qDebug() << objectName() << "::get:" << getUrl.toEncoded() << "feed:" << feedUrl << "countRepeats:" <<count;
  QNetworkRequest request(getUrl);
  request.setRawHeader("Accept", "application/atom+xml,application/rss+xml;q=0.9,application/xml;q=0.8,text/xml;q=0.7,*/*;q=0.6");
  request.setRawHeader("User-Agent", globals.userAgent().toUtf8());

  currentUrls_.append(getUrl);
  currentIds_.append(id);
  currentFeeds_.append(feedUrl);
  currentDates_.append(date);
  currentCount_.append(count);
  currentHead_.append(false);
  currentTime_.append(timeoutRequest_);

  QNetworkReply *reply = networkManager_->get(request);
  reply->setProperty("feedReply", QVariant(true));
  requestUrl_.append(reply->url());
  networkReply_.append(reply);
}

/** @brief Process network reply
 *----------------------------------------------------------------------------*/
void RequestFeed::finished(QNetworkReply *reply)
{
  QUrl replyUrl = reply->url();

  qDebug() << "reply.finished():" << replyUrl.toString();
  qDebug() << reply->header(QNetworkRequest::ContentTypeHeader);
  qDebug() << reply->header(QNetworkRequest::ContentLengthHeader);
  qDebug() << reply->header(QNetworkRequest::LocationHeader);
  qDebug() << reply->header(QNetworkRequest::LastModifiedHeader);
  qDebug() << reply->header(QNetworkRequest::CookieHeader);
  qDebug() << reply->header(QNetworkRequest::SetCookieHeader);

  int currentReplyIndex = currentUrls_.indexOf(replyUrl);

  if (currentReplyIndex >= 0) {
    currentTime_.removeAt(currentReplyIndex);
    currentUrls_.removeAt(currentReplyIndex);
    int feedId    = currentIds_.takeAt(currentReplyIndex);
    QString feedUrl    = currentFeeds_.takeAt(currentReplyIndex);
    QDateTime feedDate = currentDates_.takeAt(currentReplyIndex);
    int count = currentCount_.takeAt(currentReplyIndex) + 1;
    bool headOk = currentHead_.takeAt(currentReplyIndex);

    if (reply->error() != QNetworkReply::NoError) {
      qDebug() << "  error retrieving RSS feed:" << reply->error() << reply->errorString();
      if (!headOk) {
        if (reply->error() == QNetworkReply::AuthenticationRequiredError)
          emit getUrlDone(-2, feedId, feedUrl, tr("Server requires authentication!"));
        else if (reply->error() == QNetworkReply::ContentNotFoundError)
          emit getUrlDone(-5, feedId, feedUrl, tr("Server replied: Not Found!"));
        else {
          if (reply->errorString().contains("Service Temporarily Unavailable")) {
            if (!hostList_.contains(QUrl(feedUrl).host())) {
              hostList_.append(QUrl(feedUrl).host());
              count--;
            }
          }

          if (count < numberRepeats_) {
            emit signalGet(replyUrl, feedId, feedUrl, feedDate, count);
          } else {
            emit getUrlDone(-1, feedId, feedUrl, QString("%1 (%2)").arg(reply->errorString()).arg(reply->error()));
          }
        }
      } else {
        emit signalGet(replyUrl, feedId, feedUrl, feedDate);
      }
    } else {
      QUrl redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
      if (redirectionTarget.isValid()) {
        if (count < (numberRepeats_ + 3)) {
          if (headOk && (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)) {
            emit signalGet(replyUrl, feedId, feedUrl, feedDate);
          } else {
            QString host(QUrl::fromEncoded(feedUrl.toUtf8()).host());
            if (redirectionTarget.host().isEmpty()) {
              if (redirectionTarget.path() == ".") {
                if (redirectionTarget.hasQuery()) {
                  QString query = redirectionTarget.query();
                  redirectionTarget.setUrl(replyUrl.scheme() + "://" + host + replyUrl.path());
                  redirectionTarget.setQuery(query);
                }
              } else {
                redirectionTarget.setUrl(replyUrl.scheme() + "://" + host + redirectionTarget.toString());
              }
            }
            if (redirectionTarget.scheme().isEmpty())
              redirectionTarget.setScheme(QUrl(feedUrl).scheme());
            if (reply->operation() == QNetworkAccessManager::HeadOperation) {
              qDebug() << objectName() << "  head redirect..." << redirectionTarget.toString();
              emit signalHead(redirectionTarget, feedId, feedUrl, feedDate, count);
            }
            else {
              qDebug() << objectName() << "  get redirect..." << redirectionTarget.toString();
              emit signalGet(redirectionTarget, feedId, feedUrl, feedDate, count);
            }
          }
        } else {
          emit getUrlDone(-4, feedId, feedUrl, tr("Redirect error!"));
        }
      } else {
        QDateTime replyDate = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
        QDateTime replyLocalDate = QDateTime(replyDate.date(), replyDate.time());

        qDebug() << feedDate << replyDate << replyLocalDate;
        qDebug() << feedDate.toMSecsSinceEpoch() << replyDate.toMSecsSinceEpoch() << replyLocalDate.toMSecsSinceEpoch();
        if ((reply->operation() == QNetworkAccessManager::HeadOperation) &&
            ((!feedDate.isValid()) || (!replyLocalDate.isValid()) ||
             (feedDate != replyLocalDate) || !replyDate.toMSecsSinceEpoch())) {
          emit signalGet(replyUrl, feedId, feedUrl, feedDate);
        }
        else {
          QString codecName;
          QzRegExp rx("charset=([^\t]+)$", Qt::CaseInsensitive);
          int pos = rx.indexIn(reply->header(QNetworkRequest::ContentTypeHeader).toString());
          if (pos > -1) {
            codecName = rx.cap(1);
          }

          QByteArray data = reply->readAll();
          data = data.trimmed();

          rx.setPattern("&(?!([a-z0-9#]+;))");
          pos = 0;
          while ((pos = rx.indexIn(QString::fromLatin1(data), pos)) != -1) {
            data.replace(pos, 1, "&amp;");
            pos += 1;
          }

          data.replace("<br>", "<br/>");

          if (data.indexOf("</rss>") > 0)
            data.resize(data.indexOf("</rss>") + 6);
          if (data.indexOf("</feed>") > 0)
            data.resize(data.indexOf("</feed>") + 7);
          if (data.indexOf("</rdf:RDF>") > 0)
            data.resize(data.indexOf("</rdf:RDF>") + 10);

          emit getUrlDone(feedsQueue_.count(), feedId, feedUrl, "", data, replyLocalDate, codecName);
        }
      }
    }
  } else {
    qCritical() << "Request Url error: " << replyUrl.toString() << reply->errorString();
  }

  int replyIndex = requestUrl_.indexOf(replyUrl);
  if (replyIndex >= 0) {
    requestUrl_.removeAt(replyIndex);
    networkReply_.removeAt(replyIndex);
  }

  reply->abort();
  reply->deleteLater();
}

/** @brief Timeout to delete network requests which has no answer
 *----------------------------------------------------------------------------*/
void RequestFeed::slotRequestTimeout()
{
  for (int i = currentTime_.count() - 1; i >= 0; i--) {
    int time = currentTime_.at(i) - 1;
    if (time <= 0) {
      QUrl url = currentUrls_.takeAt(i);
      int feedId    = currentIds_.takeAt(i);
      QString feedUrl = currentFeeds_.takeAt(i);
      QDateTime feedDate = currentDates_.takeAt(i);
      int count = currentCount_.takeAt(i) + 1;
      currentTime_.removeAt(i);
      currentHead_.removeAt(i);

      int replyIndex = requestUrl_.indexOf(url);
      if (replyIndex >= 0) {
        QUrl replyUrl = requestUrl_.takeAt(replyIndex);
        QNetworkReply *reply = networkReply_.takeAt(replyIndex);
        reply->deleteLater();

        if (count < numberRepeats_) {
          emit signalGet(replyUrl, feedId, feedUrl, feedDate, count);
        } else {
          emit getUrlDone(-3, feedId, feedUrl, tr("Request timeout!"));
        }
      }
    } else {
      currentTime_.replace(i, time);
    }
  }
}
