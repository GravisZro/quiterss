#include "settings.h"

#include <QCoreApplication>

QSettings *Settings::settings_ = 0;

Settings::Settings()
{
  if (!settings_->group().isEmpty())
    settings_->endGroup();
}

Settings::~Settings()
{
  if (!settings_->group().isEmpty())
    settings_->endGroup();
}

void Settings::createSettings(const QString &fileName)
{
  if (!fileName.isEmpty()) {
    settings_ = new QSettings(fileName, QSettings::IniFormat);
  } else {
    settings_ = new QSettings(QSettings::IniFormat,
                              QSettings::UserScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName());
  }
}

QSettings* Settings::getSettings()
{
    return settings_;
}

void Settings::syncSettings()
{
  settings_->sync();
}

QString Settings::fileName()
{
  return settings_->fileName();
}

void Settings::beginGroup(const QString &prefix)
{
  settings_->beginGroup(prefix);
}

void Settings::endGroup()
{
  settings_->endGroup();
}

void Settings::setValue(const QString &key, const QVariant &defaultValue)
{
  settings_->setValue(key, defaultValue);
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue)
{
  return settings_->value(key, defaultValue);
}

bool Settings::contains(const QString &key)
{
  return settings_->contains(key);
}
