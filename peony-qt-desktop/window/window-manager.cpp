//
// Created by hxf on 2021/8/9.
//

#include "window-manager.h"

#include <QScreen>
#include <QDebug>

using namespace Peony;

static WindowManager *g_windowManager = nullptr;

WindowManager *WindowManager::getInstance(QObject *parent)
{
    if (!g_windowManager) {
        g_windowManager = new WindowManager(parent);
    }

    return g_windowManager;
}

WindowManager::WindowManager(QObject *parent) : QObject(parent)
{
    qDebug() << "[WindowManager::WindowManager] create WindowManager !";
}

DesktopBackgroundWindow *WindowManager::createWindowForScreen(QScreen *screen, QWidget *parent)
{
    if (!screen) {
        return nullptr;
    }
    qDebug() << "[WindowManager::createWindowForScreen] screen geometry:" << screen->geometry();

    DesktopBackgroundWindow *desktopWindow = this->getWindowByScreen(screen);

    m_mutex.lock();

    if (desktopWindow == nullptr) {
        desktopWindow = new DesktopBackgroundWindow(screen, parent);
        m_windowList.append(desktopWindow);
    }

    m_mutex.unlock();

    return desktopWindow;
}

DesktopBackgroundWindow *WindowManager::getWindowByScreen(QScreen *screen)
{
    if (!screen) {
        return nullptr;
    }

    for (DesktopBackgroundWindow *window : m_windowList) {
        if (window->screen() == screen) {
            m_mutex.unlock();
            return window;
        }
    }

    return nullptr;
}

WindowManager::~WindowManager()
{
    for (DesktopBackgroundWindow *window : m_windowList) {
        window->deleteLater();
    }
}

const QList<DesktopBackgroundWindow *> &WindowManager::windowList()
{
    return m_windowList;
}

void WindowManager::updateAllWindowGeometry()
{
    for (auto window : m_windowList) {
        //window->updateWinGeometry();
    }
}

void WindowManager::updateWindowView(QScreen *screen)
{
    DesktopBackgroundWindow *window = this->getWindowByScreen(screen);
    if (window) {
        //window->updateView();
    }
}

bool WindowManager::removeWindowByScreen(QScreen *screen)
{
    DesktopBackgroundWindow *desktopWindow = getWindowByScreen(screen);

    if (desktopWindow) {
        if (desktopWindow->getCurrentDesktop()) {
            desktopWindow->getCurrentDesktop()->setActivated(false);
        }
        //去除中心的view
        desktopWindow->takeCentralWidget();
        //删除window
        m_windowList.removeOne(desktopWindow);
        desktopWindow->deleteLater();
        return true;
    }

    return false;
}

