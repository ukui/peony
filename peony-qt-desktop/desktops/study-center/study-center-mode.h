/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 */

#ifndef STUDYCENTERMODE_H
#define STUDYCENTERMODE_H

#include "desktop-widget-base.h"
#include "common.h"
#include "../../tablet/data/tablet-app-manager.h"
#include "study-status-widget.h"
#include "study-directory-widget.h"
#include "syn-and-daemon.h"
#include <QDBusInterface>
class QGridLayout;

namespace Peony {

class TabletAppManager;

class StudyCenterMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    explicit StudyCenterMode(QWidget *parent = nullptr);

    ~StudyCenterMode();

    void setActivated(bool activated);

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

public:
    QList<TABLETAPP>   getTimeOrder(QMap<QString, QList<TabletAppEntity*>> studyCenterDataMap );
    void  initTime();
    long int getStudyTime(QString strMethod, QString appName);
    QString  getTime(QString strMethod, QStringList appList);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUi();
    void updateAppData();
    void screenRotation();
    void initPageButton();
    void updatePageButton();
    void changeTheme();
    void screenChange();

private:
    QGridLayout* m_mainGridLayout=nullptr;
    StudyDirectoryWidget* practiceWidget=nullptr;
    SynAndDaemon* guradWidget=nullptr;
    SynAndDaemon* synWidget=nullptr;
    StudyStatusWidget* statusWidget=nullptr;

    TabletAppManager* m_tableAppMangager=nullptr;
    QList<QString> m_appList;

    QDBusInterface *m_statusManagerDBus = nullptr;
    QPoint m_pressPoint;
    QPoint m_releasePoint;
    QPoint m_lastPressPoint;
    bool   m_leftButtonPressed = false;
    bool m_isTabletMode;
    QString m_direction;

    QWidget *m_pageButtonWidget = nullptr;
    QHBoxLayout *m_buttonLayout = nullptr;
    QButtonGroup *m_buttonGroup = nullptr;

public Q_SLOTS:
   void updateTimeSlot();
   void updateRotationsValue(QString rotation);
   void updateTabletModeValue(bool mode);

   void pageButtonClicked(QAbstractButton *button);
protected Q_SLOTS:
   void centerToScreen(QWidget *widget);

    /**
     * @brief mdm 禁用或解除禁用应用时，刷新界面
     */
    void updateUiSlot();

Q_SIGNALS:
    void valueChangedSingal(QList<TABLETAPP> applist);
    void timeChangedSingal(QString strMethod ,QString strTime);
    void markTimeSingal();
};

}

#endif // STUDYCENTERMODE_H
