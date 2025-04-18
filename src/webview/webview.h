#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include <QWebHistory>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>

#define LEFT_BUTTON 0
#define MIDDLE_BUTTON 1
#define MIDDLE_BUTTON_MOD 2
#define LEFT_BUTTON_CTRL 3
#define LEFT_BUTTON_SHIFT 4
#define LEFT_BUTTON_ALT 5

class WebView : public QWebView
{
  Q_OBJECT
public:
  explicit WebView(QWidget *parent);

  void disconnectObjects();

  int buttonClick_;

  bool isLoading() { return isLoading_; }
  bool hasRss() { return hasRss_; }

signals:
  void showContextMenu(const QPoint &);
  void signalGoHome();
  void rssChanged(bool);

protected:
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void wheelEvent(QWheelEvent*);
  void mouseMoveEvent(QMouseEvent* event);

private slots:
  void slotLoadStarted();
  void slotLoadProgress(int value);
  void slotLoadFinished();
  void checkRss();

private:
  bool isLoading_;
  bool rssChecked_;
  bool hasRss_;
  int posX_;
  QPoint dragStartPos_;

};

#endif // WEBVIEW_H
