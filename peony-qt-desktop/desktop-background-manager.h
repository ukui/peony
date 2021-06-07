#ifndef DESKTOPBACKGROUNDMANAGER_H
#define DESKTOPBACKGROUNDMANAGER_H

#include <QObject>
#include <QPixmap>

class QGSettings;
class QVariantAnimation;
class QTimeLine;

class DesktopBackgroundManager : public QObject
{
    friend class DesktopWindow;
    Q_OBJECT
public:
    explicit DesktopBackgroundManager(QObject *parent = nullptr);

private:
    void initGSettings();
    void updateScreens();
    void initBackground();
    void setBackground();
    QString getAccountBackground();
    void setAccountBackground();

protected:
    void switchBackground();
    void connectScreensChangement();

    QPixmap m_backPixmap;
    QPixmap m_frontPixmap;
    QPixmap m_pendingPixmap;

    QGSettings *m_backgroundSettings = nullptr;
    QVariantAnimation *m_animation = nullptr;

    bool m_usePureColor = false;
    QColor m_color = Qt::black;

    bool m_paintBackground = false;

    QTimeLine *m_timeLine = nullptr;

    QString m_current_bg_path;
};

#endif // DESKTOPBACKGROUNDMANAGER_H
