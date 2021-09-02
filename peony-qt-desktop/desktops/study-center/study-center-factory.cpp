//
// Created by hxf on 2021/8/31.
//

#include "study-center-factory.h"
#include "study-center-mode.h"

using namespace Peony;

static StudyCenterFactory *g_studyCenterModeFactory = nullptr;
static StudyCenterMode    *g_studyCenterMode        = nullptr;

StudyCenterFactory *StudyCenterFactory::getInstance(QObject *parent)
{
    if (!g_studyCenterModeFactory) {
        g_studyCenterModeFactory = new StudyCenterFactory(parent);
    }

    return g_studyCenterModeFactory;
}


StudyCenterFactory::StudyCenterFactory(QObject *parent) : QObject(parent)
{

}

StudyCenterFactory::~StudyCenterFactory()
{

}

DesktopType StudyCenterFactory::getDesktopType()
{
    return DesktopType::StudyCenter;
}

QString StudyCenterFactory::getFactoryName()
{
    return QString("study center mode");
}

bool StudyCenterFactory::isEnable()
{
    return true;
}

DesktopWidgetBase *StudyCenterFactory::createDesktop(QWidget *parent)
{
    if (!g_studyCenterMode) {
        g_studyCenterMode = new StudyCenterMode(parent);
    }

    return g_studyCenterMode;
}

DesktopWidgetBase *StudyCenterFactory::createNewDesktop(QWidget *parent)
{
    return new StudyCenterMode(parent);
}

bool StudyCenterFactory::closeFactory()
{
    if (g_studyCenterMode) {
        g_studyCenterMode->deleteLater();
    }

    if (g_studyCenterModeFactory) {
        g_studyCenterModeFactory->deleteLater();
    }
    return false;
}
