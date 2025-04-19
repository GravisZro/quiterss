#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QNetworkAccessManager>
#include <QWebPage>
#include <QSslCertificate>

class NetworkManagerProxy;
class AdBlockRule;

class WebPage : public QWebPage
{
  Q_OBJECT
public:
  struct AdBlockedEntry {
    const AdBlockRule* rule;
    QUrl url;

    bool operator==(const AdBlockedEntry &other) const {
      return (this->rule == other.rule && this->url == other.url);
    }
  };

  explicit WebPage(QObject *parent);
  ~WebPage();

  void disconnectObjects();

  bool acceptNavigationRequest(QWebFrame *frame,
                               const QNetworkRequest &request,
                               NavigationType type);
  void populateNetworkRequest(QNetworkRequest &request);

  void scheduleAdjustPage();
  bool isLoading() const;

  static bool isPointerSafeToUse(WebPage* page);
  void addAdBlockRule(const AdBlockRule* rule, const QUrl &url);
  QVector<AdBlockedEntry> adBlockedEntries() const;

  void addRejectedCerts(const QList<QSslCertificate> &certs);
  bool containsRejectedCerts(const QList<QSslCertificate> &certs);

protected slots:
  QWebPage *createWindow(WebWindowType type);
  void handleUnsupportedContent(QNetworkReply* reply);

private slots:
  void progress(int prog);
  void finished();
  void downloadRequested(const QNetworkRequest &request);
  void cleanBlockedObjects();
  void urlChanged(const QUrl &url);
  void slotFullScreenRequested(QWebFullScreenRequest fullScreenRequest);

private:
  NetworkManagerProxy *networkManagerProxy_;

  QWebPage::NavigationType lastRequestType_;
  QUrl lastRequestUrl_;

  bool adjustingScheduled_;
  static QList<WebPage*> livingPages_;
  QVector<AdBlockedEntry> adBlockedEntries_;

  int loadProgress_;

};

#endif // WEBPAGE_H
