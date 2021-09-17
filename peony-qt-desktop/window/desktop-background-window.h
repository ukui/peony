#ifndef DESKTOPBACKGROUNDWINDOW_H
#define DESKTOPBACKGROUNDWINDOW_H

#include "desktop-widget-base.h"

#include <QMainWindow>

namespace Peony {

class DesktopBackgroundWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DesktopBackgroundWindow(QScreen *screen, QWidget *parent = nullptr);

    int id() const;

    QScreen *screen() const;

    /**
     * @brief 初始化桌面环境，添加view组件
     */
    void setWindowDesktop(DesktopWidgetBase *desktop);

    DesktopWidgetBase *getCurrentDesktop()
    {
        return m_currentDesktop;
    }

    void blurBackground(bool blur);

public Q_SLOTS:
    void setWindowGeometry(const QRect &geometry);

protected Q_SLOTS:
    void updateWindowGeometry();

    /**
     * 对窗口当前桌面的移动请求进行响应。
     * 通过解析动画类型和移动长度计算出桌面的下一个位置。
     * @brief
     * @param animationType
     * @param moveLength
     * @param duration 动画持续时间，如果为0，则代表不需要动画。单位 ms
     */
    void desktopMoveProcess(AnimationType animationType, quint32 moveLength, quint32 duration);

    //回弹处理
    void desktopReboundProcess();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    //kwin 临时模糊效果
    qreal m_backgroundOpacity = 0.5;
    bool  m_backgroundBlur    = false;
    QWidget *m_lastEffectWidget = nullptr;

    int m_id = -1;
    QScreen *m_screen = nullptr;

    DesktopWidgetBase *m_currentDesktop = nullptr;

};

}
#endif // DESKTOPBACKGROUNDWINDOW_H
