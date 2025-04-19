#ifndef NEWSHEADER_H
#define NEWSHEADER_H

#include <QtWidgets>
#include <QHeaderView>

#include "newsmodel.h"

class NewsTabWidget;

class NewsHeader : public QHeaderView
{
  Q_OBJECT

public:
  NewsHeader(NewsModel *model, QWidget *parent);

  void init();
  void retranslateStrings();
  void setColumns(const QModelIndex &indexFeed = QModelIndex());
  void saveStateColumns(NewsTabWidget *newsTabWidget);

  QMenu *viewMenu_;

protected:
  bool eventFilter(QObject *, QEvent *);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mouseDoubleClickEvent(QMouseEvent*);

private slots:
  void slotButtonColumnView();
  void slotColumnVisible(QAction*);
  void slotSectionMoved(int, int, int);

private:
  void createMenu();

  /**
   * @brief Adjust width for all columns
   * @param newWidth Widget's width for adjustment
   */
  void adjustAllColumnsWidths(int newWidth);
  QString columnsList();

  NewsModel *model_;
  QActionGroup *columnVisibleActGroup_;
  QPushButton *buttonColumnView_;
  bool move_;
  int idxCol_;
  int posX_;

};

#endif // NEWSHEADER_H
