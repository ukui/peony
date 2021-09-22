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

//keys
#define TABLET_MODE "tabletMode"

//platform keys
#define V10_SP1                   "V10SP1"
#define V10_SP1_EDU               "V10SP1-edu"

#include <QObject>

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

private:
    explicit DesktopGlobalSettings(QObject *parent = nullptr);

};

}

#endif //PEONY_DESKTOP_GLOBAL_SETTINGS_H
