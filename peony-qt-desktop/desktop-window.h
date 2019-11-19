#ifndef DESKTOP_WINDOW_H
#define DESKTOP_WINDOW_H

#include <QStackedWidget>
#include <QTimer>
#include <QStackedLayout>

class QLabel;
class QListView;
class QGraphicsOpacityEffect;

namespace Peony {

class DesktopIconView;

class DesktopWindow : public QStackedWidget
{
    Q_OBJECT

public:
    DesktopWindow(QScreen *screen, bool is_primary, QWidget *parent = nullptr);
    ~DesktopWindow();

public:
    const QString getCurrentBgPath();
    bool getIsPrimary(){return m_is_primary;}
    void setIsPrimary(bool is_primary);
    QScreen *getScreen(){return m_screen;}
    void setScreen(QScreen *screen);
    DesktopIconView *getView(){return m_view;}

Q_SIGNALS:
    void changeBg(const QString &bgPath);

public Q_SLOTS:
    void setBg(const QString &bgPath);
    void availableGeometryChangedProcess(const QRect &geometry);
    void virtualGeometryChangedProcess(const QRect &geometry);
    void geometryChangedProcess(const QRect &geometry);
    void scaleBg(const QRect &geometry);
    void updateView();
    void updateWinGeometry();

    void connectSignal();
    void disconnectSignal();

protected:
    void initShortcut();

private:
    QString m_current_bg_path;

    QLabel *m_bg_font;
    QLabel *m_bg_back;
    DesktopIconView *m_view;

    QPixmap m_bg_font_pixmap;
    QPixmap m_bg_back_pixmap;

    QTimer m_trans_timer;
    QGraphicsOpacityEffect *m_opacity_effect;

    qreal m_opacity = 1.0;

    QScreen *m_screen;
    QStackedLayout *m_layout;
    bool m_is_primary;
};

}

#endif // DESKTOP-WINDOW_H
