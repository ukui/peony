//
// Created by hxf on 2021/8/17.
//

#ifndef PEONY_DESKTOP_GLOBAL_SETTINGS_H
#define PEONY_DESKTOP_GLOBAL_SETTINGS_H
//dbus
#define DBUS_STATUS_MANAGER_IF "com.kylin.statusmanager.interface"
//schemas
#define TABLED_ROTATION_SCHEMA "org.ukui.SettingsDaemon.plugins.xrandr"
#define TABLET_SCHEMA "org.ukui.SettingsDaemon.plugins.tablet-mode"

#define DESKTOP_SETTINGS_SCHEMA      "org.ukui.peony.desktop.settings"

//keys
#define TABLET_MODE "tabletMode"
#define DESKTOP_ANIMATION_DURATION   "desktopAnimationDuration"
#define DESKTOP_ITEM_ZOOM_RANGE      "desktopItemZoomRange"
#define ENABLE_SMALL_PLUGIN          "enableSmallPlugin"

#include <QObject>
#include <QMap>
#include <QVariant>

namespace Peony {

class DesktopGlobalSettings : public QObject
{
    Q_OBJECT
private:
    static const QString V10SP1;
    static const QString V10SP1Edu;

public:
    static DesktopGlobalSettings *globalInstance(QObject *parent = nullptr);

    const QString &getCurrentProjectName();

    QVariant getValue(const QString &key) const;
    void setValue(const QString &key, const QVariant &value);

Q_SIGNALS:
    void valueChanged(const QString &key, const QVariant &value);

private:
    explicit DesktopGlobalSettings(QObject *parent = nullptr);
    void initDesktopSetting();

private:
    QMap<QString, QVariant> m_cache;
};

}

#endif //PEONY_DESKTOP_GLOBAL_SETTINGS_H
