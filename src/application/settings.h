#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVariant>

class Settings
{
public:
  explicit Settings();
  ~Settings();

  static void createSettings(const QString &fileName = QString());
  static QSettings* getSettings();
  static void syncSettings();
  QString fileName();

  void beginGroup(const QString &prefix);
  void endGroup();

  void setValue(const QString &key, const QVariant &defaultValue = QVariant());
  QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
  bool contains(const QString &key);

private:
  static QSettings* settings_;

};

#endif // SETTINGS_H
