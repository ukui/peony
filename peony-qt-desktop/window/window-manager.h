//
// Created by hxf on 2021/8/9.
//

#ifndef PEONY_WINDOW_MANAGER_H
#define PEONY_WINDOW_MANAGER_H

#include "desktop-background-window.h"

#include <QObject>
#include <QList>
#include <QMutex>

namespace Peony {
/**
 * \brief 管理窗口与屏幕的关系，创建与删除窗口。
 */
class WindowManager : public QObject
{
    Q_OBJECT
public:
    static WindowManager *getInstance(QObject *parent = nullptr);

    virtual ~WindowManager();

    /**
     * \brief 在指定的屏幕上创建一个窗口
     * \param screen
     * \return
     */
    DesktopBackgroundWindow *createWindowForScreen(QScreen *screen, QWidget *parent = nullptr);

    /**
     * \brief 删除屏幕对应的窗口,会保留窗口上的centralWidget
     * \return
     */
    bool removeWindowByScreen(QScreen *screen);

    /**
     * \brief 通过屏幕获取对应的窗口
     * \param screen
     * \return
     */
    DesktopBackgroundWindow *getWindowByScreen(QScreen *screen);

    /**
     * \brief 返回窗口集合
     * \return
     */
    const QList<DesktopBackgroundWindow*> &windowList();

    void updateAllWindowGeometry();

    void updateWindowView(QScreen *screen);

    /**
     * \brief 获取当前窗口的数量
     * \return
     */
    quint32 getWindowCount() {
        return m_windowList.size();
    }

private:
    explicit WindowManager(QObject *parent = nullptr);

private:
    QMutex m_mutex;
    QList<DesktopBackgroundWindow*> m_windowList;

};

}

#endif //PEONY_WINDOW_MANAGER_H
