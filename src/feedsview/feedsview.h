#ifndef FEEDSVIEW_H
#define FEEDSVIEW_H

#include <QtWidgets>
#include <feedsmodel.h>

class FeedsView : public QTreeView
{
  Q_OBJECT
public:
  explicit FeedsView(QWidget * parent = 0);
  int selectId_;
  bool selectIdEn_;
  bool autocollapseFolder_;

  void setSourceModel(FeedsModel *sourceModel);
  void refresh();
  void setColumnHidden(const QString& column, bool hide);
  int columnIndex(const QString& fieldName) const;
  bool isFolder(const QModelIndex &index) const;
  QModelIndex indexNextUnread(const QModelIndex &indexCur, int nextCondition = 0);
  QModelIndex firstFeedInFolder(const QModelIndex &indexFolder);
  QModelIndex lastFeedInFolder(const QModelIndex &indexFolder);
  QModelIndex indexPrevious(const QModelIndex &indexCur, bool isParent = false);
  QModelIndex indexNext(const QModelIndex &indexCur, bool isParent = false);
  QModelIndex lastFolderInFolder(const QModelIndex &indexFolder);
  QModelIndex indexPreviousFolder(const QModelIndex &indexCur);
  QModelIndex firstFolderInFolder(const QModelIndex &indexFolder);
  QModelIndex indexNextFolder(const QModelIndex &indexCur, bool isParent = false);

public slots:
  void restoreExpanded();
  void expandAll();
  void collapseAll();
  QPersistentModelIndex selectIndex();
  void updateCurrentIndex(const QModelIndex &index);

signals:
  void signalDoubleClicked();
  void signalMiddleClicked();
  void pressKeyUp();
  void pressKeyDown();
  void pressKeyHome();
  void pressKeyEnd();
  void pressKeyPageUp();
  void pressKeyPageDown();
  void signalDropped(const QModelIndex &where, int how);

protected:
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*event);
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent*);
  virtual void keyPressEvent(QKeyEvent*);
  virtual void currentChanged(const QModelIndex &current,
                              const QModelIndex &previous);
  void dragEnterEvent(QDragEnterEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  void paintEvent(QPaintEvent *event);

private slots:
  void slotExpanded(const QModelIndex&index);
  void slotCollapsed(const QModelIndex&index);

private:
  FeedsModel *sourceModel_;
  QPoint dragPos_;
  QPoint dragStartPos_;
  QList<int> expandedList;
  int expandedOldId_;
  QModelIndex indexClicked_;

  void handleDrop(QDropEvent *e);
  bool shouldAutoScroll(const QPoint &pos) const;

};

#endif // FEEDSVIEW_H
