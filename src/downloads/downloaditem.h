#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QtWidgets>
#include "qftp.h"
#include <QTimer>
#include <QNetworkReply>
#include <QAuthenticator>

class QListWidgetItem;
class FtpDownloader;

class DownloadItem : public QWidget
{
  Q_OBJECT
public:
  explicit DownloadItem(QListWidgetItem *item, QNetworkReply *reply,
                        const QString &fileName, bool openAfterDownload);
  ~DownloadItem();

  void startDownloading();
  void startDownloadingFromFtp(const QUrl &url);
  bool isDownloading() { return downloading_; }
  QTime remainingTime() { return remTime_; }
  static QString remaingTimeToString(QTime time);
  static QString currentSpeedToString(double speed);

signals:
  void deleteItem(DownloadItem*);
  void downloadFinished(bool success);

protected:
  virtual void mouseDoubleClickEvent(QMouseEvent*);

private slots:
  void finished();
  void metaDataChanged();
  void updateInfo();
  void downloadProgress(qint64 received, qint64 total);
  void stop(bool askForDeleteFile = true);
  void openFile();
  void openFolder();
  void readyRead();
  void error();
  void updateDownload();
  void customContextMenuRequested(const QPoint &pos);
  void clear();

  void copyDownloadLink();

private:
  QString fileSizeToString(qint64 size);

  QListWidgetItem *item_;
  QNetworkReply *reply_;
  FtpDownloader *ftpDownloader_;
  QString fileName_;
  QElapsedTimer downloadTimer_;
  QTime remTime_;
  QTimer updateInfoTimer_;
  QFile outputFile_;
  QUrl downloadUrl_;

  bool downloading_;
  bool openAfterFinish_;
  bool downloadStopped_;
  double curSpeed_;
  qint64 received_;
  qint64 total_;

  QLabel *fileNameLabel_;
  QProgressBar *progressBar_;
  QFrame *progressFrame_;
  QLabel *downloadInfo_;
};

class FtpDownloader : public QFtp
{
  Q_OBJECT

public:
  FtpDownloader(QObject* parent = 0);

  void download(const QUrl &url, QIODevice* dev);
  inline bool isFinished() {return isFinished_;}
  inline QUrl url() const {return url_;}
  inline QIODevice* device() const {return dev_;}
  void setError(QFtp::Error err, const QString &errStr);
  void abort();
  QFtp::Error error();
  QString errorString() const;

private slots:
  void processCommand(int id, bool err);
  void onDone(bool err);

private:
  int ftpLoginId_;
  bool anonymousLoginChecked_;
  bool isFinished_;
  QUrl url_;
  QIODevice* dev_;
  QFtp::Error lastError_;
  QString lastErrorString_;

  static QAuthenticator *ftpAuthenticator(const QUrl &url);
  static QHash<QString, QAuthenticator*> ftpAuthenticatorsCache_;

signals:
  void ftpAuthenticationRequierd(const QUrl &, QAuthenticator*);
  void finished();
  void errorOccured(QFtp::Error);
};

#endif // DOWNLOADITEM_H
