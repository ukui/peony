//
// Created by hxf on 2021/8/25.
//

#ifndef PEONY_PEONY_DESKTOP_DBUS_SERVICE_H
#define PEONY_PEONY_DESKTOP_DBUS_SERVICE_H

#include <QObject>

namespace Peony {

class PeonyDesktopDbusService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.peonyQtDesktop")

public:
    PeonyDesktopDbusService(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * @brief 窗管未实现小窗模糊背景功能，由桌面暂时代替实现。
     * @param status
     * @return
     */
    quint32 blurBackground(quint32 status);

Q_SIGNALS:
    void blurBackGroundSignal(quint32 status);
};

}

#endif //PEONY_PEONY_DESKTOP_DBUS_SERVICE_H
