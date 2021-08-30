#ifndef STUDYCENTERMODE_H
#define STUDYCENTERMODE_H

#include "desktop-widget-base.h"

namespace Peony {

class TabletAppManager;

class StudyCenterMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    StudyCenterMode();

    ~StudyCenterMode();

    void setActivated(bool activated);

    DesktopWidgetBase *initDesktop(const QRect &rect) override;
private:
    void initUi();
    TabletAppManager* m_tableAppMangager=nullptr;

};

}

#endif // STUDYCENTERMODE_H
