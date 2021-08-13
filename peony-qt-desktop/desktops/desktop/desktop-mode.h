#ifndef DESKTOPMODE_H
#define DESKTOPMODE_H

#include "desktop-widget-base.h"
#include "desktop-icon-view.h"

#include <QVBoxLayout>

namespace Peony {

class DesktopMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    explicit DesktopMode(QWidget *parent = nullptr);

    ~DesktopMode() override;

    void setActivated(bool activated) override;

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

    void initUI();

    void initMenu();

private:
    DesktopIconView *m_view = nullptr;
    QVBoxLayout *m_boxLayout = nullptr;
};

}

#endif // DESKTOPMODE_H
