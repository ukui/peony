#include "study-center-mode.h"

using namespace Peony;

StudyCenterMode::StudyCenterMode()
{

}

StudyCenterMode::~StudyCenterMode()
{

}

void StudyCenterMode::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);
}

DesktopWidgetBase *StudyCenterMode::initDesktop(const QRect &rect)
{
    return DesktopWidgetBase::initDesktop(rect);
}
