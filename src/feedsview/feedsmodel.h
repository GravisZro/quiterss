#ifndef FEEDSMODEL_H
#define FEEDSMODEL_H

#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QTreeView>

struct UserData
{
  UserData(int id, int parid, const QSqlRecord &record)
    : id(id)
    , parid(parid),
      record(record) {
  }
  ~UserData() {
  }
  int id;
  int parid;
  QSqlRecord record;
};

class FeedsModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit FeedsModel(QObject *parent = 0);
  ~FeedsModel();

  void setView(QTreeView *view);

  QVariant dataField(const QModelIndex &index, const QString &fieldName) const;
  bool isFolder(const QModelIndex &index) const;
  QModelIndex indexSibling(const QModelIndex &index, const QString &fieldName) const;

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  Qt::ItemFlags flags(const QModelIndex &index) const;
  Qt::DropActions supportedDropActions() const;

  int idByIndex(const QModelIndex &index) const;
  QModelIndex indexById(int id) const;
  int paridByIndex(const QModelIndex &index) const;

  int indexColumnOf(int column) const;
  int indexColumnOf(const QString &name) const;

  QFont font_;
  QString formatDate_;
  QString formatTime_;
  bool defaultIconFeeds_;
  QString textColor_;
  QString backgroundColor_;
  QString feedWithNewNewsColor_;
  QString countNewsUnreadColor_;
  QString focusedFeedTextColor_;
  QString focusedFeedBGColor_;
  QString feedDisabledUpdateColor_;

public slots:
  void refresh();

private:
  void clear();
  int rowById(int id) const;
  int rowByParid(int parid) const;
  UserData * userDataById(int id) const;

  QTreeView *view_;
  QSqlQueryModel queryModel_;
  int rootParentId_;
  int indexId_;
  int indexParid_;

  QMap<int,int> id2RowList_;
  QMap<int,int> parid2RowList_;
  QMap<int,UserData*> userDataList_;
  QHash<int,int> columnsList_;


};

#endif  // FEEDSMODEL_H
