#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QFile>
#include <QStringList>
#include <QNetworkCookieJar>

enum UseCookies {
  BlockCookies,
  SaveCookies,
  DeleteCookiesOnClose
};

class CookieJar : public QNetworkCookieJar
{
  Q_OBJECT
public:
  explicit CookieJar(QObject *parent);

  bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

  QList<QNetworkCookie> getAllCookies();
  void setAllCookies(const QList<QNetworkCookie> &cookieList);

  void saveCookies();
  void loadCookies();

  UseCookies useCookies() const;
  void setUseCookies(UseCookies value);

public slots:
  void clearCookies();

private:
  UseCookies useCookies_;

};

#endif // COOKIEJAR_H
