#ifndef NEWSVIEW_H
#define NEWSVIEW_H

#include <QtWidgets>

#include "newsmodel.h"

class NewsView : public QTreeView
{
  Q_OBJECT
public:
  NewsView(QWidget * parent = 0);

signals:
  void signalSetItemRead(QModelIndex index, int read);
  void signalSetItemStar(QModelIndex index, int starred);
  void signalDoubleClicked(QModelIndex index);
  void signalMiddleClicked(QModelIndex index);
  void signaNewslLabelClicked(QModelIndex index);
  void pressKeyUp(const QModelIndex &index);
  void pressKeyDown(const QModelIndex &index);
  void pressKeyHome(const QModelIndex &index);
  void pressKeyEnd(const QModelIndex &index);
  void pressKeyPageUp(const QModelIndex &index);
  void pressKeyPageDown(const QModelIndex &index);

protected:
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent*);
  virtual void keyPressEvent(QKeyEvent*);

private:
  QModelIndex indexClicked_;

};

#endif // NEWSVIEW_H
