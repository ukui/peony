//
// Created by hxf on 2021/8/6.
//

#include "desktop-mode-factory.h"
#include "desktop-mode.h"

using namespace Peony;

static DesktopModeFactory *g_desktopModeFactory = nullptr;
static DesktopMode        *g_desktopMode        = nullptr;

DesktopModeFactory *DesktopModeFactory::getInstance(QObject *parent)
{
    if (!g_desktopModeFactory) {
        g_desktopModeFactory = new DesktopModeFactory(parent);
    }

    return g_desktopModeFactory;
}

DesktopModeFactory::DesktopModeFactory(QObject *parent) : QObject(parent)
{

}

DesktopModeFactory::~DesktopModeFactory()
{

}

DesktopType DesktopModeFactory::getDesktopType()
{
    return DesktopType::Desktop;
}

QString DesktopModeFactory::getFactoryName()
{
    return QString(tr("desktop icon mode"));
}

bool DesktopModeFactory::isEnable()
{
    //桌面模式默认为启用状态
    return true;
}

DesktopWidgetBase *DesktopModeFactory::createDesktop(QWidget *parent)
{
    if (!g_desktopMode) {
        g_desktopMode = new DesktopMode(parent);
    }

    return g_desktopMode;
}

DesktopWidgetBase *DesktopModeFactory::createNewDesktop(QWidget *parent)
{
    return new DesktopMode(parent);
}

bool DesktopModeFactory::closeFactory()
{
    if (g_desktopModeFactory) {
        g_desktopModeFactory->deleteLater();
    }

    if (g_desktopMode) {
        g_desktopMode->deleteLater();
    }

    return true;
}


