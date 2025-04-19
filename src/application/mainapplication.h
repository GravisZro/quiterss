#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#define mainApp MainApplication::getInstance()

#include <QtWidgets>
#include <qtsingleapplication.h>
#include <QNetworkDiskCache>
#include <QLocale>
#include <QLibraryInfo>
#include <QNetworkProxy>

#include "cookiejar.h"
#include "downloadmanager.h"
#include "mainwindow.h"

class NetworkManager;
class SplashScreen;
class UpdateFeeds;

class MainApplication : public QtSingleApplication
{
  Q_OBJECT
public:
  explicit MainApplication(int &argc, char** argv);
  ~MainApplication();

  static MainApplication *getInstance();

  bool isPortable() const;
  bool isPortableAppsCom() const;
  void setClosing();
  bool isClosing() const;
  bool isNoDebugOutput() const;
  void showClosingWidget();

  QString resourcesDir() const;
  QString dataDir() const;
  QString absolutePath(const QString &path) const;
  QString dbFileName() const;
  QString cacheDefaultDir() const;
  QString soundNotifyDefaultFile() const;
  QString styleSheetNewsDefaultFile() const;
  QString styleSheetWebDarkFile() const;

  bool storeDBMemory() const;
  bool dbFileExists() const { return dbFileExists_; }
  bool isSaveDataLastFeed() const;
  void sqlQueryExec(const QString &query);

  MainWindow *mainWindow();
  NetworkManager *networkManager();
  CookieJar *cookieJar();
  void setDiskCache();
  UpdateFeeds *updateFeeds();
  void runUserFilter(int feedId, int filterId);
  DownloadManager *downloadManager();

  void setTranslateApplication();
  QString language() const { return langFileName_; }
  void setLanguage(const QString &lang) { langFileName_ = lang; }

  QNetworkProxy networkProxy() const { return networkProxy_; }
  void proxyLoadSettings();
  void proxySaveSettings(const QNetworkProxy &proxy);

public slots:
  void receiveMessage(const QString &message);
  void quitApplication();
  void reloadUserStyleBrowser();

signals:
  void signalRunUserFilter(int feedId, int filterId);
  void signalSqlQueryExec(const QString &query);

private slots:
  void commitData(QSessionManager &manager);

private:
  void createSettings();
  void connectDatabase();
  void loadSettings();
  void setStyleApplication();
  void showSplashScreen();
  void closeSplashScreen();
  void setProgressSplashScreen(int value);

  QUrl userStyleSheet(const QString &filePath) const;

  void setProxy();

  bool isPortableAppsCom_;
  bool isClosing_;

  bool storeDBMemory_;
  bool dbFileExists_;
  bool isSaveDataLastFeed_;
  QString styleApplication_;
  bool showSplashScreen_;
  bool updateFeedsStartUp_;

  QTranslator *translator_;
  QTranslator *qt_translator_;
  QString langFileName_;
  SplashScreen *splashScreen_;
  MainWindow *mainWindow_;
  NetworkManager *networkManager_;
  CookieJar *cookieJar_;
  QNetworkDiskCache *diskCache_;
  UpdateFeeds *updateFeeds_;
  DownloadManager *downloadManager_;
  QWidget *closingWidget_;

  QNetworkProxy networkProxy_;
};

#endif // MAINAPPLICATION_H
