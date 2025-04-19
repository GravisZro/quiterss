#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDir>

#define ADBLOCK_EASYLIST_URL "https://easylist-downloads.adblockplus.org/easylist.txt"

#ifndef Q_UNLIKELY
#define Q_UNLIKELY(x) x
#endif

#ifndef QSL
#define QSL(x) QStringLiteral(x)
#endif

#ifndef QL1S
#define QL1S(x) QLatin1String(x)
#endif

#ifndef QL1C
#define QL1C(x) QLatin1Char(x)
#endif

namespace Common
{
#if defined(Q_OS_WIN) || defined(Q_OS_OS2)
  static const QString Qz_RAWVCS = "https://raw.githubusercontent.com/QupZilla/qupzilla/v1.8";
#endif

  bool removePath(const QString &path);
  bool matchDomain(const QString &pattern, const QString &domain);
  QString filterCharsFromFilename(const QString &name);
  QString ensureUniqueFilename(const QString &name, const QString &appendFormat = QString("(%1)"));
  void createFileBackup(const QString &oldFilename, const QString &oldVersion);

  QString readAllFileContents(const QString &filename);
  QByteArray readAllFileByteContents(const QString &filename);

  void sleep(int ms);
}

#endif // COMMON_H
