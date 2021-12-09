#ifndef DESKTOPBACKGROUNDWINDOW_H
#define DESKTOPBACKGROUNDWINDOW_H

#include <QMainWindow>
#include <QGSettings>

class DesktopBackgroundWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DesktopBackgroundWindow(QScreen *screen, QWidget *parent = nullptr);

    int id() const;

    QScreen *screen() const;

public Q_SLOTS:
    void setWindowGeometry(const QRect &geometry);

protected Q_SLOTS:
    void updateWindowGeometry();

protected:
    void paintEvent(QPaintEvent *event) override;
    QPoint getRelativePos(const QPoint &pos);
    /**
     * 图片填充方式
     * @brief 从给定的图片中，截取一个与屏幕比例相同的矩形。(Rect,居中)。
     * @param pixmap 图片
     * @return
     */
    QRect getSourceRect(const QPixmap &pixmap);

private:
    int m_id = -1;
    QScreen *m_screen = nullptr;
    QGSettings *m_panelSetting = nullptr;
};

#endif // DESKTOPBACKGROUNDWINDOW_H
