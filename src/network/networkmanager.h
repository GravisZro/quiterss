#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QSslError>
#include <QStringList>

class AdBlockManager;

class NetworkManager : public QNetworkAccessManager
{
  Q_OBJECT
public:
  explicit NetworkManager(bool isThread, QObject *parent = 0);
  ~NetworkManager();

  QNetworkReply *createRequest(QNetworkAccessManager::Operation op,
                               const QNetworkRequest &request,
                               QIODevice *outgoingData);

  void loadSettings();
  void loadCertificates();

private slots:
  void slotAuthentication(QNetworkReply *reply, QAuthenticator *auth);
  void slotProxyAuthentication(const QNetworkProxy &proxy, QAuthenticator *auth);
  void slotSslError(QNetworkReply *reply, QList<QSslError> errors);

private:
  void addRejectedCerts(const QList<QSslCertificate> &certs);
  bool containsRejectedCerts(const QList<QSslCertificate> &certs);
  void addLocalCertificate(const QSslCertificate &cert);
  void removeLocalCertificate(const QSslCertificate &cert);

  QStringList certPaths_;
  QList<QSslCertificate> caCerts_;
  QList<QSslCertificate> localCerts_;
  QList<QSslCertificate> tempAllowedCerts_;
  bool ignoreAllWarnings_;
  QList<QSslCertificate> rejectedSslCerts_;

  AdBlockManager *adblockManager_;

};

#endif // NETWORKMANAGER_H
