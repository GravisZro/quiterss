#include "cookiejar.h"

#include "mainapplication.h"
#include "settings.h"

#include <QNetworkCookie>
#include <QDateTime>
#include <QDir>

CookieJar::CookieJar(QObject *parent)
  : QNetworkCookieJar(parent)
{
  loadCookies();
}

/** @brief Load cookies from file
 *----------------------------------------------------------------------------*/
void CookieJar::loadCookies()
{
  Settings settings;
  useCookies_ = (UseCookies)settings.value("Settings/saveCookies", SaveCookies).toInt();

  if (useCookies_ != SaveCookies) return;

  if (!QFile::exists(mainApp->dataDir() + "/cookies.dat")) {
    return;
  }

  QDateTime now = QDateTime::currentDateTime();

  QList<QNetworkCookie> loadedCookies;
  QFile file(mainApp->dataDir() + "/cookies.dat");
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  int count;

  stream >> count;
  for (int i = 0; i < count; i++) {
    QByteArray rawForm;
    stream >> rawForm;
    const QList<QNetworkCookie> &cookieList = QNetworkCookie::parseCookies(rawForm);
    if (cookieList.isEmpty()) {
      continue;
    }

    const QNetworkCookie &cookie = cookieList.at(0);
    if (cookie.expirationDate() < now) {
      continue;
    }
    loadedCookies.append(cookie);
  }

  file.close();
  setAllCookies(loadedCookies);
}

/** @brief Save cookies to file
 *----------------------------------------------------------------------------*/
void CookieJar::saveCookies()
{
  Settings settings;
  settings.setValue("Settings/saveCookies", useCookies_);

  if (useCookies_ != SaveCookies) return;

  QList<QNetworkCookie> allCookies = getAllCookies();

  QFile file(mainApp->dataDir() + "/cookies.dat");
  file.open(QIODevice::WriteOnly);
  QDataStream stream(&file);
  int count = allCookies.count();

  stream << count;
  for (int i = 0; i < count; i++) {
    const QNetworkCookie &cookie = allCookies.at(i);

    if (cookie.isSessionCookie()) {
      continue;
    }
    stream << cookie.toRawForm();
  }

  file.close();
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
  if (useCookies_ == BlockCookies)
    return false;
  return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
}

/** @brief Clear all cookies
 *----------------------------------------------------------------------------*/
void CookieJar::clearCookies()
{
  setAllCookies(QList<QNetworkCookie>());
}

/** @brief Retrive all cookies
 *----------------------------------------------------------------------------*/
QList<QNetworkCookie> CookieJar::getAllCookies()
{
  return QNetworkCookieJar::allCookies();
}

/** @brief Setup cookies from list
 *----------------------------------------------------------------------------*/
void CookieJar::setAllCookies(const QList<QNetworkCookie> &cookieList)
{
  QNetworkCookieJar::setAllCookies(cookieList);
}

UseCookies CookieJar::useCookies() const
{
  return useCookies_;
}

void CookieJar::setUseCookies(UseCookies value)
{
  useCookies_ = value;
}
