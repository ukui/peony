//
// Created by hxf on 2021/8/17.
//

#ifndef PEONY_DESKTOP_GLOBAL_SETTINGS_H
#define PEONY_DESKTOP_GLOBAL_SETTINGS_H

#define TABLET_SCHEMA "org.ukui.SettingsDaemon.plugins.tablet-mode"
#define TABLET_MODE   "tabletMode"

#include <QObject>

namespace Peony {

class DesktopGlobalSettings : public QObject
{
    Q_OBJECT
public:
    explicit DesktopGlobalSettings(QObject *parent = nullptr);

};

}

#endif //PEONY_DESKTOP_GLOBAL_SETTINGS_H
