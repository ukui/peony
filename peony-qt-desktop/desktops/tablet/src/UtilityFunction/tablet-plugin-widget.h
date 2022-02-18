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
 * Modified By: Wenfei He <hewenfei@kylinos.cn>
 *
 */

//
// Created by hxf on 2021/8/16.
//

#ifndef PEONY_TABLET_PLUGIN_WIDGET_H
#define PEONY_TABLET_PLUGIN_WIDGET_H

#include "src/Interface/currenttimeinterface.h"
#include "src/Style/style.h"
#include "KySmallPluginInterface.h"


#include <QWidget>
#include <QBoxLayout>
#include <QGSettings>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>

class TabletPluginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TabletPluginWidget(QWidget *parent = nullptr);

    void initDateTimeWidget();

    void initSearchWidget();

    void initPluginWidget();

    void initGSettings();

    void setSearchOpacityEffect(const qreal& num);

    void hidePluginWidget(bool hide);

    void updateMainLayout() {
        m_mainLayout->setContentsMargins(Style::TimeWidgetLeft, Style::TimeWidgetTop, 0, 0);
        if (Style::ScreenRotation) {
            m_mainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        } else {
            m_mainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        }
    }

protected:
    void changeSearchBoxBackground();


private Q_SLOTS:
    void startUKUISearch();

private:
    qreal m_opacity = 1;
    QString m_themeName;
    QGSettings *m_themeSetting = nullptr;

    QGraphicsDropShadowEffect *m_effect = nullptr;
    //插件
    QWidget *m_pluginWidget = nullptr;

    QGSettings *m_timeSetting = nullptr;
    CurrentTimeInterface *m_timeInterface = nullptr;

    QVBoxLayout *m_mainLayout = nullptr; //主布局

    QWidget *m_dateTimeContainer = nullptr; //时间日期容器
    QHBoxLayout *m_dateTimeLayout = nullptr; //时间日期容器的布局
    QVBoxLayout *m_weekDateLayout = nullptr;

    QTimer* m_timer = nullptr; //日期计数器

    //日期
    QLabel* m_timeLabel;
    QLabel* m_weekLabel;
    QLabel* m_dateLabel;

    //搜索
    QWidget *m_searchContainer = nullptr; //搜索组件容器
    QHBoxLayout *m_searchLayout = nullptr; //搜索组件容器
    QPushButton *m_searchEditBtn = nullptr;




};


#endif //PEONY_TABLET_PLUGIN_WIDGET_H
