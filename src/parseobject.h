#ifndef PARSEOBJECT_H
#define PARSEOBJECT_H

#include <QtSql>
#include <QDateTime>
#include <QDomDocument>
#include <QQueue>
#include <QObject>
#include <QUrl>
#include <QMutex>

struct FeedItemStruct {
  QString title;
  QString updated;
  QString link;
  QString linkBase;
  QString language;
  QString author;
  QString authorUri;
  QString authorEmail;
  QString description;
};

struct NewsItemStruct {
  QString id;
  QString title;
  QString updated;
  QString link;
  QString linkAlternate;
  QString language;
  QString author;
  QString authorUri;
  QString authorEmail;
  QString description;
  QString content;
  QString category;
  QString eUrl;
  QString eType;
  QString eLength;
  QString comments;
};

struct FeedCountStruct{
  int feedId;
  int unreadCount;
  int newCount;
  int undeleteCount;
  QString updated;
  QString lastBuildDate;
  QString htmlUrl;
  QString xmlUrl;
  QString title;
};

Q_DECLARE_METATYPE(FeedCountStruct)

class ParseObject : public QObject
{
  Q_OBJECT
public:
  explicit ParseObject(QObject *parent = 0);
  ~ParseObject();

  void disconnectObjects();

public slots:
  void parseXml(QByteArray data, int feedId,
                QDateTime dtReply, QString codecName);
  void runUserFilter(int feedId, int filterId = -1);

signals:
  void signalReadyParse(const QByteArray &xml, const int &feedId,
                        const QDateTime &dtReply, const QString &codecName);
  void signalFinishUpdate(int feedId, bool changed, int newCount, QString status);
  void feedCountsUpdate(FeedCountStruct counts);
  void signalPlaySound(const QString &soundPath);
  void signalAddColorList(int id, const QString &color);

private slots:
  void getQueuedXml();
  void slotParse(const QByteArray &xmlData, const int &feedId,
                 const QDateTime &dtReply, const QString &codecName);
  void addAtomNewsIntoBase(NewsItemStruct *newsItem);
  void addRssNewsIntoBase(NewsItemStruct *newsItem);

private:
  void parseAtom(const QString &feedUrl, const QDomDocument &doc);
  void parseRss(const QString &feedUrl, const QDomDocument &doc);
  QString toPlainText(const QString &text);
  QString fromPlainText(QString text);
  QString getCommunity(const QDomNode &nodeContent);
  QString parseDate(const QString &dateString, const QString &urlString);
  int recountFeedCounts(int feedId, const QString &feedUrl,
                        const QString &updated, const QString &lastBuildDate);

  QSqlDatabase db_;
  QTimer *parseTimer_;
  QMutex mutex_;
  QQueue<int> idsQueue_;
  QQueue<QByteArray> xmlsQueue_;
  QQueue<QDateTime> dtReadyQueue_;
  QQueue<QString> codecNameQueue_;

  int parseFeedId_;
  bool duplicateNewsMode_;
  bool feedChanged_;
  bool addSingleNewsAnyDate_;
  bool avoidedOldSingleNews_;
  QDate avoidedOldSingleNewsDate_;

  QStringList guidList_;
  QStringList linkList_;
  QStringList titleList_;
  QStringList publishedList_;

  QDateTime lastBuildDate_;

};

#endif // PARSEOBJECT_H
