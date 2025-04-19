#ifndef DATABASE_H
#define DATABASE_H

#include <QtCore>
#include <QtSql>

class Database : public QObject
{
  Q_OBJECT
public:
  static int version();
  static void initialization();
  static QSqlDatabase connection(const QString &connectionName = QString());
  static void sqliteDBMemFile(QSqlDatabase &db, bool save = true);
  static void setVacuum();

private:
  static void setPragma(QSqlDatabase &db);
  static void createTables(QSqlDatabase &db);
  static void prepareDatabase();
  static void createLabels(QSqlDatabase &db);
  static void addColumnsToFeedsTables(QSqlDatabase &db);

  static QStringList tablesList() {
    QStringList tables;
    tables << "feeds" << "news" << "feeds_ex"
           << "news_ex" << "filters" << "filterConditions"
           << "filterActions" << "filters_ex" << "labels"
           << "passwords" << "info";
    return tables;
  }

};

#endif // DATABASE_H
