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
    explicit PeonyDesktopDbusService(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * @brief 获取当前桌面的类型 id。
     * @return 桌面的类型id
     */
    int getCurrentDesktopType();

Q_SIGNALS:
    /**
    * @param desktopType {
         0: 普通PC桌面
         1: 空白桌面
         2: 平板桌面
         3: 学习专区
     }
    */
    void desktopChangedSignal(int desktopType);
};

}

#endif //PEONY_PEONY_DESKTOP_DBUS_SERVICE_H
