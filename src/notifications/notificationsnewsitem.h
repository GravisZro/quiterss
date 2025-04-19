#ifndef NOTIFICATIONSNEWSITEM_H
#define NOTIFICATIONSNEWSITEM_H

#include <QtWidgets>
#include <QtSql>

#include "toolbutton.h"

class NewsItem : public QWidget
{
  Q_OBJECT
public:
  NewsItem(int idFeed, int idNews, int width, QWidget * parent = 0);
  ~NewsItem();

  void setText(const QString &text);
  void setFontText(const QFont & font);
  void setColorText(const QString &color, const QString &linkColor);

  QLabel *iconLabel_;
  ToolButton *readButton_;
  ToolButton *openExternalBrowserButton_;
  ToolButton *deleteButton_;

signals:
  void signalOpenExternalBrowser(const QUrl &url);
  void signalMarkRead(int feedId, int newsId, int read);
  void signalTitleClicked(int feedId, int newsId);
  void signalDeleteNews(int feedId, int newsId);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

private slots:
  void openExternalBrowser();
  void markRead();
  void deleteNews();

private:
  int feedId_;
  int newsId_;
  bool read_;

  QLabel *textLabel_;

};

#endif // NOTIFICATIONSNEWSITEM_H
