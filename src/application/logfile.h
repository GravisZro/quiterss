#ifndef LOGFILE_H
#define LOGFILE_H

#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QTextStream>

const size_t maxLogFileSize = 1 * 1024 * 1024; //1 MB

class LogFile
{
public:
  static void msgHandler(QtMsgType type, const QMessageLogContext &, const QString &msg);

private:
  explicit LogFile();

};

#endif // LOGFILE_H
