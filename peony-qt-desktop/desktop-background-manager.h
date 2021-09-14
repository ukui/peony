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
    static DesktopBackgroundManager *globalInstance();

    QPixmap getBackPixmap() const;

    QPixmap getFrontPixmap() const;

    QVariantAnimation *getAnimation() const;

    bool getUsePureColor() const;

    QColor getColor() const;

    bool getPaintBackground() const;

    const QString &getBackgroundOption();

Q_SIGNALS:
    void screensUpdated();

private:
    explicit DesktopBackgroundManager(QObject *parent = nullptr);
    void initGSettings();
    void updateScreens();
    void initBackground();
    void setBackground();
    QString getAccountBackground();
    void setAccountBackground();

protected:
    void switchBackground();

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
    QString m_backgroundOption;
};

#endif // DESKTOPBACKGROUNDMANAGER_H
