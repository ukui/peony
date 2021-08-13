#ifndef TABLETMODE_H
#define TABLETMODE_H

#include "desktop-widget-base.h"

namespace Peony {

class TabletMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    TabletMode(QWidget *parent = nullptr);

    ~TabletMode() override;

    void setActivated(bool activated) override;

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint m_startPoint;
    QPoint m_endPoint;
    QPoint m_lastEndPoint;//上一次move事件鼠标停留的位置

    bool m_leftKeyPressed = false;
};

}

#endif // TABLETMODE_H
