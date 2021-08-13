//
// Created by hxf on 2021/8/10.
//

#include "tablet-mode-factory.h"
#include "tablet-mode.h"

using namespace Peony;

static TabletModeFactory *g_tabletModeFactory = nullptr;
static TabletMode        *g_tabletMode        = nullptr;


TabletModeFactory *TabletModeFactory::getInstance(QObject *parent)
{
    if (!g_tabletModeFactory) {
        g_tabletModeFactory = new TabletModeFactory(parent);
    }

    return g_tabletModeFactory;
}

Peony::TabletModeFactory::~TabletModeFactory()
{

}

Peony::DesktopType Peony::TabletModeFactory::getDesktopType()
{
    return DesktopType::Tablet;
}

QString Peony::TabletModeFactory::getFactoryName()
{
    return QString("平板模式");
}

bool Peony::TabletModeFactory::isEnable()
{
    return true;
}

Peony::DesktopWidgetBase *Peony::TabletModeFactory::createDesktop(QWidget *parent)
{
    if (!g_tabletMode) {
        g_tabletMode = new TabletMode(parent);
    }

    return g_tabletMode;
}

Peony::DesktopWidgetBase *Peony::TabletModeFactory::createNewDesktop(QWidget *parent)
{
    return nullptr;
}

bool Peony::TabletModeFactory::closeFactory()
{
    if (g_tabletMode) {
        g_tabletMode->deleteLater();
    }

    if (g_tabletModeFactory) {
        g_tabletModeFactory->deleteLater();
    }
    return false;
}

TabletModeFactory::TabletModeFactory(QObject *parent) : QObject(parent)
{

}
