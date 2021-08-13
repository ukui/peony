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

DesktopWindow *WindowManager::createWindowForScreen(QScreen *screen, bool isPrimaryScreen)
{
    if (!screen) {
        return nullptr;
    }
    qDebug() << "[WindowManager::createWindowForScreen] screen geometry:" << screen->geometry();
    m_mutex.lock();

    DesktopWindow *desktopWindow = new DesktopWindow(screen, isPrimaryScreen);
    m_windowList.append(desktopWindow);

    m_mutex.unlock();

    return desktopWindow;
}

DesktopWindow *WindowManager::getWindowByScreen(QScreen *screen)
{
    if (!screen) {
        return nullptr;
    }

    m_mutex.lock();

    for (DesktopWindow *window : m_windowList) {
        if (window->getScreen() == screen) {
            m_mutex.unlock();
            return window;
        }
    }

    m_mutex.unlock();
    return nullptr;
}

WindowManager::~WindowManager()
{
    for (DesktopWindow *window : m_windowList) {
        window->deleteLater();
    }
}

const QList<DesktopWindow *> &WindowManager::windowList()
{
    qDebug() << "===地址1：" << &m_windowList;
    return m_windowList;
}

void WindowManager::updateAllWindowGeometry()
{
    for (auto window : m_windowList) {
        window->updateWinGeometry();
    }
}

void WindowManager::updateWindowView(QScreen *screen)
{
    DesktopWindow *window = this->getWindowByScreen(screen);
    if (window) {
        window->updateView();
    }
}

bool WindowManager::removeWindowByScreen(QScreen *screen)
{
    DesktopWindow *desktopWindow = getWindowByScreen(screen);

    if (desktopWindow) {
        //删除window
        m_windowList.removeOne(desktopWindow);
        desktopWindow->deleteLater();
        return true;
    }

    return false;
}

