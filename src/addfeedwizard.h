#ifndef ADDFEEDDIALOG_H
#define ADDFEEDDIALOG_H

#include <QtWidgets>
#include <QtSql>

#include "lineedit.h"
#include "updatefeeds.h"

class AddFeedWizard : public QWizard
{
  Q_OBJECT
public:
  explicit AddFeedWizard(QWidget *parent, int curFolderId);
  ~AddFeedWizard();

  void setUrlFeed(const QString &feedUrl);

  LineEdit *nameFeedEdit_;
  LineEdit *urlFeedEdit_;
  QString htmlUrlString_;
  QString feedUrlString_;
  int feedId_;
  int feedParentId_;
  int newCount_;

public slots:
  void getUrlDone(int result, int feedId, QString feedUrlStr,
                  QString error, QByteArray data,
                  QDateTime dtReply, QString codecName);
  void slotUpdateFeed(int feedId, bool, int newCount, QString);

signals:
  void xmlReadyParse(QByteArray data, int feedId,
                     QDateTime dtReply, QString codecName);
  void signalRequestUrl(int feedId, QString urlString,
                        QDateTime date, QString userInfo);

protected:
  virtual bool validateCurrentPage();
  virtual void done(int result);
  void changeEvent(QEvent *event);

private slots:
  void urlFeedEditChanged(const QString&);
  void nameFeedEditChanged(const QString&);
  void backButtonClicked();
  void nextButtonClicked();
  void finishButtonClicked();
  void slotCurrentIdChanged(int);
  void titleFeedAsNameStateChanged(int);
  void slotProgressBarUpdate();
  void newFolder();

private:
  void addFeed();
  void deleteFeed();
  void showProgressBar();
  void finish();

  UpdateFeeds *updateFeeds_;
  QWizardPage *createUrlFeedPage();
  QWizardPage *createNameFeedPage();
  QCheckBox *titleFeedAsName_;
  QGroupBox *authentication_;
  LineEdit *user_;
  LineEdit *pass_;
  QLabel *textWarning;
  QWidget *warningWidget_;
  QProgressBar *progressBar_;
  bool selectedPage;
  bool finishOn;
  QTreeWidget *foldersTree_;
  int curFolderId_;

};

#endif // ADDFEEDDIALOG_H
