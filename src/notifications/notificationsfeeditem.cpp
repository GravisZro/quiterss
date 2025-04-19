#include "notificationsfeeditem.h"

FeedItem::FeedItem(int width, QWidget * parent)
  : QWidget(parent)
{
  iconLabel_ = new QLabel(this);
  titleLabel_ = new QLabel(this);
  titleLabel_->setFixedWidth(width + 21*1);

  QWidget *mainWidget = new QWidget(this);
  mainWidget->setObjectName("feedItemNotification");
  QHBoxLayout *layout = new QHBoxLayout(mainWidget);
  layout->setMargin(4);
  layout->setSpacing(5);
  layout->addWidget(iconLabel_);
  layout->addWidget(titleLabel_);
  layout->addStretch();

  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->setMargin(0);
  mainLayout->addWidget(mainWidget);
  setLayout(mainLayout);
}

FeedItem::~FeedItem()
{

}

void FeedItem::setIcon(const QPixmap &icon)
{
  iconLabel_->setPixmap(icon);
}

void FeedItem::setTitle(const QString &text, int cntNews)
{
  const int wight = titleLabel_->fontMetrics().horizontalAdvance(QString(" (%1)").arg(cntNews));
  QString titleStr = titleLabel_->fontMetrics().elidedText(
        text, Qt::ElideRight, titleLabel_->sizeHint().width() - wight);
  titleLabel_->setText(QString("%1 (%2)").arg(titleStr).arg(cntNews));
}

void FeedItem::setFontTitle(const QFont &font)
{
  titleLabel_->setFont(font);
}

void FeedItem::setColorText(const QColor &color)
{
  titleLabel_->setStyleSheet(QString("QLabel {color: %1;}").arg(color.name()));
}
