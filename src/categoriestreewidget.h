#ifndef CATEGORIESTREEWIDGET_H
#define CATEGORIESTREEWIDGET_H

#include <QtWidgets>

class CategoriesTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  explicit CategoriesTreeWidget(QWidget *parent = 0);

  enum Items {UnreadItem, StarredItem, DeletedItem, LabelsItem};
  enum LabelRole {ImageRole = Qt::UserRole+1, NumRole, colorBgRole, colorTextRole};

  QList<QTreeWidgetItem *> getLabelListItems() const {
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < topLevelItem(LabelsItem)->childCount(); ++i) {
      items.append(topLevelItem(LabelsItem)->child(i));
    }
    return items;
  }
  QTreeWidgetItem* getLabelItem(int id) const {
    for (int i = 0; i < topLevelItem(LabelsItem)->childCount(); ++i) {
      QTreeWidgetItem *item = topLevelItem(LabelsItem)->child(i);
      if (item->text(2).toInt() == id)
        return item;
    }
    return 0;
  }
  int labelsCount() const {
    return topLevelItem(LabelsItem)->childCount();
  }

signals:
  void signalMiddleClicked();
  void signalClearDeleted();
  void signalMarkRead(QTreeWidgetItem *item);
//  void pressKeyUp();
//  void pressKeyDown();

protected:
  virtual void mousePressEvent(QMouseEvent*);
//  virtual void keyPressEvent(QKeyEvent*);

private slots:
  void showContextMenuCategory(const QPoint &pos);
  void openCategoryNewTab();

  void slotMarkRead();

private:
  QTreeWidgetItem *itemClicked_;

};

#endif // CATEGORIESTREEWIDGET_H
