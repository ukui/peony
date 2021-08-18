/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 * Authors: Meihong He <hemeihong@kylinos.cn>
 *
 */

#ifndef DESKTOP_WINDOW_H
#define DESKTOP_WINDOW_H

#include "desktop-manager.h"

#include <QMainWindow>
#include <QTimer>
#include <QStackedLayout>
#include <QDebug>

class QVariantAnimation;
class QLabel;
class QListView;
class QGraphicsOpacityEffect;
class QGSettings;
class QSettings;

namespace Peony {

typedef enum {
    PEONY_BOOT_START,
    PEONY_BOOT_UPDATE,
    PEONY_BOOT_PAINT,
    PEONY_BOOT_FINSH,
}PEONY_BOOT_STAGE;

class DesktopIconView;

class DesktopWindow : public QMainWindow
{
    Q_OBJECT

public:
    DesktopWindow(QScreen *screen, bool is_primary, QWidget *parent = nullptr);
    ~DesktopWindow();

public:
    const QString getCurrentBgPath();
    const QColor getCurrentColor();
    bool getIsPrimary() {
        return m_is_primary;
    }
    void setIsPrimary(bool is_primary);
    QScreen *getScreen() {
        return m_screen;
    }
    void setScreen(QScreen *screen);

    static void gotoSetBackground();

    void gotoSetThisBackground() {
        DesktopWindow::gotoSetBackground();
    }

    /**
     * \brief 初始化桌面环境，添加view组件
     */
    void setWindowDesktop(DesktopWidgetBase *desktop);

    DesktopWidgetBase *getCurrentDesktop() {
        qDebug() << "===DesktopWidgetBase *getCurrentDesktop:" << m_currentDesktop->geometry();
        return m_currentDesktop;
    }

Q_SIGNALS:
    void changeBg(const QString &bgPath);
    void checkWindow();

public Q_SLOTS:
    void setBgPath(const QString &bgPath);
    void availableGeometryChangedProcess(const QRect &geometry);
    void virtualGeometryChangedProcess(const QRect &geometry);
    void geometryChangedProcess(const QRect &geometry);

    void scaleBg(const QRect &geometry);
    void updateView();
    void updateWinGeometry();
    void updateScreenVisible();

    void connectSignal();
    void disconnectSignal();
    void bootStageUpdate();

    void testSig() {
        printf("1111");
    }
protected:
    void paintEvent(QPaintEvent *e);

protected Q_SLOTS:
    void setBg(const QString &bgPath);
    void setBg(const QColor &color);

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
    void initShortcut();
    void initGSettings();

private:
    QString m_current_bg_path;
    QString m_picture_option;

    QPixmap m_bg_font_pixmap;
    QPixmap m_bg_back_pixmap;

    QPixmap m_bg_font_cache_pixmap;
    QPixmap m_bg_back_cache_pixmap;

    QGraphicsOpacityEffect *m_opacity_effect;

    QScreen *m_screen;
    QStackedLayout *m_layout;
    bool m_is_primary;

    QGSettings *m_bg_settings = nullptr;
    QSettings *m_backup_setttings = nullptr;

    QColor m_last_pure_color = Qt::transparent;
    QColor m_color_to_be_set = Qt::transparent;

    bool m_use_pure_color = false;
    bool m_used_pure_color = false;
    bool m_tabletmode = false;

    PEONY_BOOT_STAGE m_boot_stage;
    QTimer *m_boot_timer;

    QVariantAnimation *m_opacity = nullptr;

    DesktopWidgetBase *m_currentDesktop = nullptr;
};

}

#endif // DESKTOP-WINDOW_H
