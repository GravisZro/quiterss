#include "splashscreen.h"
#include "VersionNo.h"

SplashScreen::SplashScreen(const QPixmap &pixmap, Qt::WindowFlags flag)
  : QSplashScreen(pixmap, flag)
{
  setFixedSize(pixmap.width(), pixmap.height());
  setContentsMargins(5, 0, 5, 0);
  setEnabled(false);
  showMessage("Prepare loading...   " %
              QString("%1").arg(STRPRODUCTVER),
              Qt::AlignRight | Qt::AlignTop, Qt::darkGray);
  setAttribute(Qt::WA_DeleteOnClose);

  QFont font = this->font();
  font.setPixelSize(12);
  setFont(font);

  splashProgress_.setObjectName("splashProgress");
  splashProgress_.setTextVisible(false);
  splashProgress_.setFixedHeight(10);
  splashProgress_.setMaximum(100);
  splashProgress_.setValue(10);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addStretch(1);
  layout->addWidget(&splashProgress_);
  setLayout(layout);
}

void SplashScreen::setProgress(int value)
{
  qApp->processEvents();
  splashProgress_.setValue(value);
  showMessage("Loading: " % QString::number(value) % "%   " %
              QString("%1").arg(STRPRODUCTVER),
              Qt::AlignRight | Qt::AlignTop, Qt::darkGray);
}
