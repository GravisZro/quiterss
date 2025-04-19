#ifndef NOTIFICATIONSFEEDITEM_H
#define NOTIFICATIONSFEEDITEM_H

#include <QtWidgets>

class FeedItem : public QWidget
{
  Q_OBJECT
public:
  FeedItem(int width, QWidget * parent = 0);
  ~FeedItem();

  void setIcon(const QPixmap &icon);
  void setTitle(const QString &text, int cntNews);
  void setFontTitle(const QFont &font);
  void setColorText(const QColor &color);

private:
  QLabel *iconLabel_;
  QLabel *titleLabel_;

};

#endif // NOTIFICATIONSFEEDITEM_H
