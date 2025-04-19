#ifndef NEWSMODEL_H
#define NEWSMODEL_H

#include <QtWidgets>
#include <QtSql>

class NewsModel : public QSqlTableModel
{
  Q_OBJECT
public:
  NewsModel(QObject *parent, QTreeView *view);
  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  virtual void sort(int column, Qt::SortOrder order);
  virtual QModelIndexList match(
      const QModelIndex &start, int role, const QVariant &value, int hits = 1,
      Qt::MatchFlags flags =
      Qt::MatchFlags(Qt::MatchExactly|Qt::MatchWrap)
      ) const;
  QVariant dataField(int row, const QString &fieldName) const;
  void setFilter(const QString &filter);
  bool select();

  QString formatDate_;
  QString formatTime_;
  bool simplifiedDateTime_;
  QString textColor_;
  QString newNewsTextColor_;
  QString unreadNewsTextColor_;
  QString focusedNewsTextColor_;
  QString focusedNewsBGColor_;

signals:
  void signalSort(int column, int order);

private:
  QTreeView *view_;

};

#endif // NEWSMODEL_H
