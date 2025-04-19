#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QtWidgets>
#include <QNetworkAccessManager>

class DownloadItem;

class DownloadManager : public QWidget
{
  Q_OBJECT

public:
  explicit DownloadManager(QWidget *parent = 0);
  ~DownloadManager();

  void download(const QNetworkRequest &request);
  void handleUnsupportedContent(QNetworkReply *reply, bool askDownloadLocation);
  void startExternalApp(const QString &executable, const QUrl &url);
  void retranslateStrings();

public slots:
  void ftpAuthentication(const QUrl &url, QAuthenticator *auth);

signals:
  void signalItemCreated(QListWidgetItem* item, DownloadItem* downItem);
  void signalShowDownloads(bool activate);
  void signalUpdateInfo(const QString &text);

private slots:
  QString getFileName(QNetworkReply* reply);
  void itemCreated(QListWidgetItem* item, DownloadItem* downItem);
  void clearList();
  void deleteItem(DownloadItem* item);
  void updateInfo();

private:
  QListWidget *listWidget_;
  QAction *listClaerAct_;
  QTimer updateInfoTimer_;

};

#endif // DOWNLOADMANAGER_H
