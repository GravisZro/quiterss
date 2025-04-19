#include "globals.h"

#include <QStandardPaths>
#include <QWebPage>
#include <QCoreApplication>
#include <QDir>
#include <QStringBuilder>

#include "settings.h"

Globals globals;

Globals::Globals()
  : logFileOutput_(true)
  , noDebugOutput_(true)
  , isInit_(false)
  , isPortable_(false)
  , resourcesDir_()
  , dataDir_()
  , cacheDir_()
  , soundNotifyDir_()
{

}

void Globals::init()
{
  // isPortable ...
#if defined(Q_OS_WIN)
  isPortable_ = true;
  QString fileName(QCoreApplication::applicationDirPath() + "/portable.dat");
  if (!QFile::exists(fileName)) {
    isPortable_ = false;
  }
#endif

  // Check Dir ...
#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
  resourcesDir_ = QCoreApplication::applicationDirPath();
#else
#if defined(Q_OS_MAC)
  resourcesDir_ = QCoreApplication::applicationDirPath() + "/../Resources";
#else
  resourcesDir_ = RESOURCES_DIR;
#endif
#endif

  if (isPortable_) {
    dataDir_ = QCoreApplication::applicationDirPath();
    cacheDir_ = "cache";
    soundNotifyDir_ = "sound";
  } else {
    dataDir_ = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    cacheDir_ = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    soundNotifyDir_ = resourcesDir_ % "/sound";

    QDir dir(dataDir_);
    dir.mkpath(dataDir_);
  }

  // settings ...
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QString settingsFileName;
  if (isPortable_)
    settingsFileName = dataDir_ % "/" % QCoreApplication::applicationName() % ".ini";
  Settings::createSettings(settingsFileName);

  Settings settings;
  settings.beginGroup("Settings");
  noDebugOutput_ = settings.value("noDebugOutput", true).toBool();
  userAgent_ = settings.value("userAgent", DEFAULT_USER_AGENT).toString();

  isInit_ = true;
}

void Globals::setUserAgent(const QString userAgent)
{
  userAgent_ = userAgent;
}
