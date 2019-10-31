#ifndef DESKTOP_WINDOW_H
#define DESKTOP_WINDOW_H

#include <QStackedWidget>
#include <QTimer>

class QLabel;
class QListView;
class QGraphicsOpacityEffect;

namespace Peony {

class DesktopIconView;

class DesktopWindow : public QStackedWidget
{
    Q_OBJECT

public:
    DesktopWindow(QWidget *parent = nullptr);
    ~DesktopWindow();

public:
    const QString getCurrentBgPath();

public Q_SLOTS:
    void setBg(const QString &bgPath);

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
};

}

#endif // DESKTOP-WINDOW_H
