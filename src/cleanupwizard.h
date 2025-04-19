#ifndef CLEANUPWIZARD_H
#define CLEANUPWIZARD_H

#include <QtWidgets>

class CleanUpWizard : public QWizard
{
  Q_OBJECT
public:
  explicit CleanUpWizard(QWidget *parent);
  ~CleanUpWizard();

signals:
  void signalStartCleanUp(bool isShutdown, QStringList feedsIdList,
                          QList<int> foldersIdList);

public slots:
  void finishCleanUp(int countDeleted);

protected:
  virtual void closeEvent(QCloseEvent*);
  virtual bool validateCurrentPage();

private slots:
  void currentIdChanged(int);
  void finishButtonClicked();
  void feedItemChanged(QTreeWidgetItem *item, int column);
  void setCheckStateItem(QTreeWidgetItem *item, Qt::CheckState state);

private:
  QWizardPage *createChooseFeedsPage();
  QWizardPage *createCleanUpOptionsPage();

  bool selectedPage_;
  bool itemNotChecked_;
  QTreeWidget *feedsTree_;
  QCheckBox *dayCleanUpOn_;
  QSpinBox *maxDayCleanUp_;
  QCheckBox *newsCleanUpOn_;
  QSpinBox *maxNewsCleanUp_;
  QCheckBox *readCleanUp_;
  QCheckBox *neverUnreadCleanUp_;
  QCheckBox *neverStarCleanUp_;
  QCheckBox *neverLabelCleanUp_;
  QCheckBox *cleanUpDeleted_;
  QCheckBox *fullCleanUp_;
  QProgressBar *progressBar_;

};

#endif // CLEANUPWIZARD_H
