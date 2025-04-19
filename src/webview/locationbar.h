#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H

#include <QLineEdit>
#include <QToolButton>

class WebView;

class LocationBar : public QLineEdit
{
  Q_OBJECT
public:
  LocationBar(WebView *view, QWidget *parent = 0);

public slots:
  void updateTextMargins();
  void showRssIcon(bool show);

private slots:
  void rssIconClicked();

private:
  void mouseReleaseEvent(QMouseEvent*);
  void focusInEvent(QFocusEvent *event);

  WebView *view_;
  bool focus_;
  QWidget *rightWidget_;
  QToolButton *rssButton_;

};

#endif // LOCATIONBAR_H
