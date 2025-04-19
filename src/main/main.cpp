#include "globals.h"
#include "mainapplication.h"
#include "logfile.h"
#if defined(Q_OS_WIN)
#include <windows.h>
#endif

int main(int argc, char **argv)
{
  if (globals.logFileOutput_) {
    qInstallMessageHandler(LogFile::msgHandler);
  }

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  qputenv("QT_DEVICE_PIXEL_RATIO", "auto");

  MainApplication app(argc, argv);

  if (app.isClosing())
    return 0;

  return app.exec();
}
