#ifndef RSSDETECTIONWIDGET_H
#define RSSDETECTIONWIDGET_H

#include <QFrame>
#include <QLayout>
#include <QMenu>

class WebView;

class RSSDetectionWidget : public QFrame
{
  Q_OBJECT

public:
  explicit RSSDetectionWidget(WebView* view, QWidget* parent = 0);
  ~RSSDetectionWidget();

  void showAt(QWidget* parent);

private slots:
  void addRss();

private:
  WebView* view_;
  QGridLayout* gridLayout_;

};

#endif // RSSDETECTIONWIDGET_H
