#include "logfile.h"

#include <QStandardPaths>
#include <QDir>

#include "globals.h"
#include "settings.h"

LogFile::LogFile()
{
}

void LogFile::msgHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
  if (!globals.isInit_)
    return;
  if (msg.startsWith("libpng warning: iCCP"))
    return;

  if (type == QtDebugMsg) {
    if (globals.noDebugOutput_)
      return;
  }

  QFile file;
  file.setFileName(globals.dataDir_ + "/debug.log");
  QIODevice::OpenMode openMode = QIODevice::WriteOnly | QIODevice::Text;

  if (file.exists() && (file.size() < (qint64)maxLogFileSize)) {
    openMode |= QIODevice::Append;
  }

  file.open(openMode);

  QTextStream stream;
  stream.setDevice(&file);
  stream.setCodec("UTF-8");

  if (file.isOpen()) {
    QString currentDateTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    switch (type) {
    case QtDebugMsg:
      stream << currentDateTime << " DEBUG: " << msg << "\n";
      break;
    case QtWarningMsg:
      stream << currentDateTime << " WARNING: " << msg << "\n";
      break;
    case QtCriticalMsg:
      stream << currentDateTime << " CRITICAL: " << msg << "\n";
      break;
    case QtFatalMsg:
      stream << currentDateTime << " FATAL: " << msg << "\n";
      qApp->exit(EXIT_FAILURE);
    default:
      break;
    }

    stream.flush();
    file.flush();
    file.close();
  }
}
