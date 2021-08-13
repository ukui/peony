#ifndef STUDYCENTERMODE_H
#define STUDYCENTERMODE_H

#include "desktop-widget-base.h"

namespace Peony {

class StudyCenterMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    StudyCenterMode();

    ~StudyCenterMode() override;

    void setActivated(bool activated) override;

    DesktopWidgetBase *initDesktop(const QRect &rect) override;
};

}

#endif // STUDYCENTERMODE_H
