#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QtWidgets>

class SplashScreen : public QSplashScreen
{
  Q_OBJECT
public:
  explicit SplashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags flag = Qt::WindowFlags());

  void setProgress(int value);

private:
  QProgressBar splashProgress_;

};

#endif // SPLASHSCREEN_H
