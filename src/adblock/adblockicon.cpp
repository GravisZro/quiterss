#include "adblockicon.h"
#include "adblockrule.h"
#include "adblockmanager.h"
#include "adblocksubscription.h"
#include "mainwindow.h"
#include "webpage.h"

#include <QMenu>
#include <QTimer>

AdBlockIcon::AdBlockIcon(MainWindow *window, QWidget *parent)
  : QToolButton(parent)
  , m_window(window)
  , m_menuAction(0)
  , m_flashTimer(0)
  , m_timerTicks(0)
  , m_enabled(false)
{
  setFocusPolicy(Qt::NoFocus);
  setStyleSheet("QToolButton { border: none; padding: 0px; }");

  connect(this, SIGNAL(clicked(QPoint)), this, SLOT(showMenu(QPoint)));
  connect(AdBlockManager::instance(), SIGNAL(enabledChanged(bool)), this, SLOT(setEnabled(bool)));
}

void AdBlockIcon::retranslateStrings()
{
  setToolTip(tr("AdBlock lets you block unwanted content on web pages"));

  if (!m_enabled)
    return;

  AdBlockManager::instance()->customList()->retranslateStrings();
}

void AdBlockIcon::popupBlocked(const QString &ruleString, const QUrl &url)
{
  int index = ruleString.lastIndexOf(QLatin1String(" ("));

  const QString subscriptionName = ruleString.left(index);
  const QString filter = ruleString.mid(index + 2, ruleString.size() - index - 3);
  AdBlockSubscription* subscription = AdBlockManager::instance()->subscriptionByName(subscriptionName);
  if (filter.isEmpty() || !subscription) {
    return;
  }

  QPair<AdBlockRule*, QUrl> pair;
  pair.first = new AdBlockRule(filter, subscription);
  pair.second = url;
  m_blockedPopups.append(pair);

  //!** FIXME
  //    mApp->desktopNotifications()->showNotification(QPixmap(":images/images/adblock_big.png"), tr("Blocked popup window"), tr("AdBlock blocked unwanted popup window."));

  if (!m_flashTimer) {
    m_flashTimer = new QTimer(this);
  }

  if (m_flashTimer->isActive()) {
    stopAnimation();
  }

  m_flashTimer->setInterval(500);
  m_flashTimer->start();

  connect(m_flashTimer, SIGNAL(timeout()), this, SLOT(animateIcon()));
}

QAction* AdBlockIcon::menuAction()
{
  if (!m_menuAction) {
    m_menuAction = new QAction(tr("AdBlock"), this);
    m_menuAction->setMenu(new QMenu);
    connect(m_menuAction->menu(), SIGNAL(aboutToShow()), this, SLOT(createMenu()));
  }

  m_menuAction->setIcon(QIcon(m_enabled ? ":images/images/adblock.png" : ":images/images/adblock-disabled.png"));

  return m_menuAction;
}

void AdBlockIcon::createMenu(QMenu* menu)
{
  if (!menu) {
    menu = qobject_cast<QMenu*>(sender());
    if (!menu) {
      return;
    }
  }

  menu->clear();

  AdBlockManager* manager = AdBlockManager::instance();
  AdBlockCustomList* customList = manager->customList();

  WebPage* page = qobject_cast<WebPage*>(m_window->currentNewsTab->webView_->page());
  const QUrl pageUrl = page->mainFrame()->url();

  menu->addAction(tr("Show AdBlock &Settings"), manager, SLOT(showDialog()));
  menu->addSeparator();

  if (!pageUrl.host().isEmpty() && m_enabled && manager->canRunOnScheme(pageUrl.scheme())) {
    const QString host = pageUrl.host().contains(QLatin1String("www.")) ? pageUrl.host().mid(4) : pageUrl.host();
    const QString hostFilter = QString("@@||%1^$document").arg(host);
    const QString pageFilter = QString("@@|%1|$document").arg(pageUrl.toString());

    QAction* act = menu->addAction(tr("Disable on %1").arg(host));
    act->setCheckable(true);
    act->setChecked(customList->containsFilter(hostFilter));
    act->setData(hostFilter);
    connect(act, SIGNAL(triggered()), this, SLOT(toggleCustomFilter()));

    act = menu->addAction(tr("Disable only on this page"));
    act->setCheckable(true);
    act->setChecked(customList->containsFilter(pageFilter));
    act->setData(pageFilter);
    connect(act, SIGNAL(triggered()), this, SLOT(toggleCustomFilter()));

    menu->addSeparator();
  }

  if (!m_blockedPopups.isEmpty()) {
    menu->addAction(tr("Blocked Popup Windows"))->setEnabled(false);
    for (int i = 0; i < m_blockedPopups.count(); i++) {
      const QPair<AdBlockRule*, QUrl> &pair = m_blockedPopups.at(i);

      QString address = pair.second.toString().right(55);
      QString actionText = tr("%1 with (%2)").arg(address, pair.first->filter()).replace(QLatin1Char('&'), QLatin1String("&&"));

      QAction* action = menu->addAction(actionText, manager, SLOT(showRule()));
      action->setData(QVariant::fromValue((void*)pair.first));
    }
  }

  menu->addSeparator();

  QVector<WebPage::AdBlockedEntry> entries = page->adBlockedEntries();
  if (entries.isEmpty()) {
    menu->addAction(tr("No content blocked"))->setEnabled(false);
  }
  else {
    menu->addAction(tr("Blocked URL (AdBlock Rule) - click to edit rule"))->setEnabled(false);
    foreach (const WebPage::AdBlockedEntry &entry, entries) {
      QString address = entry.url.toString().right(55);
      QString actionText = tr("%1 with (%2)").arg(address, entry.rule->filter()).replace(QLatin1Char('&'), QLatin1String("&&"));

      QAction* action = menu->addAction(actionText, manager, SLOT(showRule()));
      action->setData(QVariant::fromValue((void*)entry.rule));
    }
  }
}

void AdBlockIcon::showMenu(const QPoint &pos)
{
  QMenu menu;
  createMenu(&menu);

  menu.exec(pos);
}

void AdBlockIcon::toggleCustomFilter()
{
  QAction* action = qobject_cast<QAction*>(sender());
  if (!action) {
    return;
  }

  const QString filter = action->data().toString();
  AdBlockManager* manager = AdBlockManager::instance();
  AdBlockCustomList* customList = manager->customList();

  if (customList->containsFilter(filter)) {
    customList->removeFilter(filter);
  }
  else {
    AdBlockRule* rule = new AdBlockRule(filter, customList);
    customList->addRule(rule);
  }
}

void AdBlockIcon::animateIcon()
{
  ++m_timerTicks;
  if (m_timerTicks > 10) {
    stopAnimation();
    return;
  }

  if (icon().isNull()) {
    setIcon(QIcon(":images/images/adblock.png"));
  }
  else {
    setIcon(QIcon());
  }
}

void AdBlockIcon::stopAnimation()
{
  m_timerTicks = 0;
  m_flashTimer->stop();
  disconnect(m_flashTimer, SIGNAL(timeout()), this, SLOT(animateIcon()));

  setEnabled(m_enabled);
}

void AdBlockIcon::setEnabled(bool enabled)
{
  if (enabled) {
    setIcon(QIcon(":images/images/adblock.png"));
  }
  else {
    setIcon(QIcon(":images/images/adblock-disabled.png"));
  }

  m_enabled = enabled;
}

void AdBlockIcon::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
    if (event->modifiers() != Qt::ControlModifier) {
      emit clicked(event->globalPos());
    }
  }
  else {
    QToolButton::mouseReleaseEvent(event);
  }
}

AdBlockIcon::~AdBlockIcon()
{
}
