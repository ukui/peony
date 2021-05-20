#ifndef DESKTOPBACKGROUND_H
#define DESKTOPBACKGROUND_H

#include <QWidget>

class QGSettings;
class QVariantAnimation;
class QTimeLine;

class DesktopBackground : public QWidget
{
    Q_OBJECT
public:
    explicit DesktopBackground(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *e);

    void updateScreens();
    void initBackground();
    void setBackground();
    QString getAccountBackground();
    void setAccountBackground();

protected:
    void switchBackground();
    void connectScreensChangement();

private:
    QPixmap m_frontPixmap;
    QPixmap m_backPixmap;
    QPixmap m_pendingPixmap;

    QGSettings *m_backgroundSettings = nullptr;
    QVariantAnimation *m_animation = nullptr;

    bool m_usePureColor = false;
    QColor m_color = Qt::black;

    bool m_paintBackground = false;

    QTimeLine *m_timeLine = nullptr;

    QString m_current_bg_path;
};

#endif // DESKTOPBACKGROUND_H
