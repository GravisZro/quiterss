#include "webpluginfactory.h"

#include "adblockmanager.h"
#include "webpage.h"

#include <QDebug>
#include <QNetworkRequest>

WebPluginFactory::WebPluginFactory(WebPage *page)
  : QWebPluginFactory(page)
  , page_(page)
{

}

QObject* WebPluginFactory::create(const QString &mimeType, const QUrl &url,
                                  const QStringList &argumentNames,
                                  const QStringList &argumentValues) const
{
  Q_UNUSED(mimeType);
  Q_UNUSED(argumentNames);
  Q_UNUSED(argumentValues);
  if (url.isEmpty()) {
    return new QObject();
  }

  // AdBlock
  AdBlockManager* manager = AdBlockManager::instance();
  QNetworkRequest request(url);
  request.setAttribute(QNetworkRequest::Attribute(QNetworkRequest::User + 150), QString("object"));
  if (manager->isEnabled() && manager->block(request)) {
    return new QObject();
  }

  return 0;
}

QList<QWebPluginFactory::Plugin> WebPluginFactory::plugins() const
{
  QList<Plugin> plugins;
  return plugins;
}
