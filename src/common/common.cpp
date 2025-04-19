#include "common.h"

#include <QtCore>
#include <QApplication>
#if defined Q_OS_WIN
#include <windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif

bool Common::removePath(const QString &path)
{
  bool result = true;
  QFileInfo info(path);
  if (info.isDir()) {
    QDir dir(path);
    foreach (const QString &entry, dir.entryList(QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot)) {
      result &= removePath(dir.absoluteFilePath(entry));
    }
    if (!info.dir().rmdir(info.fileName()))
      return false;
  } else {
    result = QFile::remove(path);
  }
  return result;
}

/** @brief Matches domain (assumes both pattern and domain not starting with dot)
 * @param pattern = domain to be matched
 * @param domain = site domain
 *----------------------------------------------------------------------------*/
bool Common::matchDomain(const QString &pattern, const QString &domain)
{
  if (pattern == domain) {
    return true;
  }

  if (!domain.endsWith(pattern)) {
    return false;
  }

  int index = domain.indexOf(pattern);

  return index > 0 && domain[index - 1] == QLatin1Char('.');
}

QString Common::filterCharsFromFilename(const QString &name)
{
  QString value = name;

  value.replace(QLatin1Char('/'), QLatin1Char('-'));
  value.remove(QLatin1Char('\\'));
  value.remove(QLatin1Char(':'));
  value.remove(QLatin1Char('*'));
  value.remove(QLatin1Char('?'));
  value.remove(QLatin1Char('"'));
  value.remove(QLatin1Char('<'));
  value.remove(QLatin1Char('>'));
  value.remove(QLatin1Char('|'));

  return value;
}

QString Common::ensureUniqueFilename(const QString &name, const QString &appendFormat)
{
  if (!QFile::exists(name)) {
    return name;
  }

  QString tmpFileName = name;
  int i = 1;
  while (QFile::exists(tmpFileName)) {
    tmpFileName = name;
    int index = tmpFileName.lastIndexOf(QLatin1Char('.'));

    QString appendString = appendFormat.arg(i);
    if (index == -1) {
      tmpFileName.append(appendString);
    }
    else {
      tmpFileName = tmpFileName.left(index) + appendString + tmpFileName.mid(index);
    }
    i++;
  }
  return tmpFileName;
}

/** Create backup copy of file
 *
 *  Backup filename format:
 *  <old-filename>_<file-version>_<backup-creation-time>.bak
 * @param oldFilename absolute path of file to backup
 * @param oldVersion version of file to backup
 *----------------------------------------------------------------------------*/
void Common::createFileBackup(const QString &oldFilename, const QString &oldVersion)
{
  QFileInfo fileInfo(oldFilename);

  // Create backup folder inside DB-file folder
  QDir backupDir(fileInfo.absoluteDir());
  if (!backupDir.exists("backup"))
    backupDir.mkpath("backup");
  backupDir.cd("backup");

  // Delete old files
  QStringList fileNameList = backupDir.entryList(QStringList(QString("%1*").arg(fileInfo.fileName())),
                                                 QDir::Files, QDir::Time);
  int count = 0;
  foreach (QString fileName, fileNameList) {
    count++;
    if (count >= 4) {
      QFile::remove(backupDir.absolutePath() + '/' + fileName);
    }
  }

  // Create backup
  QString backupFilename(backupDir.absolutePath() + '/' + fileInfo.fileName());
  backupFilename.append(QString("_%1_%2.bak")
                        .arg(oldVersion)
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss")));
  QFile::copy(oldFilename, backupFilename);
}

QString Common::readAllFileContents(const QString &filename)
{
  return QString::fromUtf8(readAllFileByteContents(filename));
}

QByteArray Common::readAllFileByteContents(const QString &filename)
{
  QFile file(filename);

  if (!filename.isEmpty() && file.open(QFile::ReadOnly)) {
    const QByteArray a = file.readAll();
    file.close();
    return a;
  }

  return QByteArray();
}

void Common::sleep(int ms)
{
#if defined(Q_OS_WIN)
  Sleep(DWORD(ms));
#else
  struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
  nanosleep(&ts, NULL);
#endif
}
