#ifndef TABBAR_H
#define TABBAR_H

#include <QtWidgets>

class TabBar : public QTabBar
{
  Q_OBJECT
public:
  explicit TabBar(QWidget *parent = 0);

  enum CloseTabsState {
    CloseTabIdle = 0,
    CloseTabCurrentIndex,
    CloseTabOtherIndex
  };

  CloseTabsState closingTabState_;

public slots:
  void slotCloseTab();
  void slotCloseOtherTabs();
  void slotCloseAllTab();
  void slotNextTab();
  void slotPrevTab();

signals:
  void closeTab(int index);

protected:
  bool eventFilter(QObject *obj, QEvent *ev);

private slots:
  void showContextMenuTabBar(const QPoint &pos);

private:
  int indexClickedTab_;
  bool tabFixed_;

};

#endif // TABBAR_H
