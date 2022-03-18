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

#include "tablet-plugin-widget.h"
#include "desktop-global-settings.h"
#include "src/Style/style.h"

#include <QProcess>
#include <QDebug>
#include <QPluginLoader>
#include <QDBusInterface>

#define TIME_FORMAT "org.ukui.control-center.panel.plugins"
#define TABLED_SCHEMA "org.ukui.SettingsDaemon.plugins.tablet-mode"
#define TIME_FORMAT_KEY "hoursystem"

TabletPluginWidget::TabletPluginWidget(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(Style::TimeWidgetLeft,Style::TimeWidgetTop,0,0);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setLayout(m_mainLayout);

    this->initDateTimeWidget();
    this->initSearchWidget();
    this->initPluginWidget();

    m_mainLayout->addStretch(1);

    this->initGSettings();

    this->changeSearchBoxBackground();
}

void TabletPluginWidget::initDateTimeWidget()
{
    m_dateTimeContainer = new QWidget(this);
    m_dateTimeContainer->setFixedSize(400,112);

    m_dateTimeLayout = new QHBoxLayout(m_dateTimeContainer);
    m_dateTimeLayout->setContentsMargins(0, 0, 0, 24);

    m_dateTimeContainer->setLayout(m_dateTimeLayout);

    m_timeLabel = new QLabel(m_dateTimeContainer);
    m_weekLabel = new QLabel(m_dateTimeContainer);
    m_dateLabel = new QLabel(m_dateTimeContainer);

    m_timeLabel->setStyleSheet("border:0px;background:transparent;font-size:96px;color:white;");
    m_weekLabel->setStyleSheet("border:0px;background:transparent;font-size:32px;color:white;");
    m_dateLabel->setStyleSheet("border:0px;background:transparent;font-size:32px;color:white;");

    m_weekDateLayout = new QVBoxLayout(m_dateTimeContainer);
    m_weekDateLayout->setSpacing(1);
    m_weekDateLayout->setAlignment(Qt::AlignVCenter);
    m_weekDateLayout->setContentsMargins(24, 0, 0, 0);

    m_weekDateLayout->addWidget(m_weekLabel);
    m_weekDateLayout->addWidget(m_dateLabel);

    m_dateTimeLayout->addWidget(m_timeLabel);
    m_dateTimeLayout->addStretch(1);
    m_dateTimeLayout->addLayout(m_weekDateLayout);

    //添加到主布局中
    m_mainLayout->addWidget(m_dateTimeContainer);
}

void TabletPluginWidget::initGSettings()
{
    m_timeInterface = new CurrentTimeInterface;

    m_timeLabel->setText(m_timeInterface->currentTime);
    m_weekLabel->setText(m_timeInterface->currentWeek);
    m_dateLabel->setText(m_timeInterface->currentDate);

    m_timer = new QTimer();
    m_timer->start(10000);
    connect(m_timer, &QTimer::timeout, [this] () {
        m_timeLabel->setText(m_timeInterface->currentTime);
        m_weekLabel->setText(m_timeInterface->currentWeek);
        m_dateLabel->setText(m_timeInterface->currentDate);
    });

    m_timeSetting = new QGSettings(TIME_FORMAT);
    connect(m_timeSetting, &QGSettings::changed, this, [=](const QString &key) {
        m_timeLabel->setText(m_timeInterface->currentTime);
        m_weekLabel->setText(m_timeInterface->currentWeek);
        m_dateLabel->setText(m_timeInterface->currentDate);
    });

    if(QGSettings::isSchemaInstalled("org.ukui.style")) {
        m_themeSetting = new QGSettings("org.ukui.style");
        m_themeName = m_themeSetting->get("style-name").toString();

        connect(m_themeSetting, &QGSettings::changed, this, [=](){
            changeSearchBoxBackground();
        });
    }
}

void TabletPluginWidget::initSearchWidget()
{
    m_searchContainer = new QWidget(this);
    m_searchContainer->setFixedSize(400, 104);
    m_searchLayout = new QHBoxLayout(m_searchContainer);

    m_searchLayout->setContentsMargins(0,0,0,32);

    //搜索
    m_searchEditBtn=new QPushButton(m_searchContainer);
    m_searchEditBtn->setFocusPolicy(Qt::NoFocus);
    m_searchEditBtn->setFixedSize(400,80);
    m_searchEditBtn->setIcon(QIcon(":/img/ukui-search-blue.svg"));
    m_searchEditBtn->setIconSize(QPixmap(":/img/ukui-search-blue.svg").size());
    m_searchEditBtn->setText(tr("Search"));

    connect(m_searchEditBtn, &QPushButton::clicked, this, &TabletPluginWidget::startUKUISearch);

    m_searchLayout->addWidget(m_searchEditBtn);

    m_mainLayout->addWidget(m_searchContainer);
}

void TabletPluginWidget::hidePluginWidget(bool hide)
{
    if (m_pluginWidget) {
        if (hide) {
            m_pluginWidget->hide();
        } else {
            m_pluginWidget->show();
        }
    }
}

void TabletPluginWidget::initPluginWidget()
{
    if (!Peony::DesktopGlobalSettings::globalInstance()->getValue(ENABLE_SMALL_PLUGIN).toBool()) {
        return;
    }
    QPluginLoader loader("/opt/small-plugin/bin/libsmall-plugin-manage.so");
    QObject * plugin=loader.instance();
    if(plugin)
    {
        KySmallPluginInterface * app=qobject_cast<KySmallPluginInterface *>(plugin);
        m_pluginWidget = app->createWidget(this);
        m_pluginWidget->setFixedSize(400,640);

        m_mainLayout->addWidget(m_pluginWidget);
    }
}

void TabletPluginWidget::changeSearchBoxBackground()
{
    if (!m_themeSetting) {
        return;
    }

    QString styleSheetDark = QString("border-radius:40px;background:rgba(44,50,57,%1);"
                                     "color:white;font-size:24px;text-align:left;padding-left:24px;").arg(m_opacity);

    QString styleSheetLight = QString("border-radius:40px;background:rgba(255,255,255,%1);"
                                      "color:rgba(58,67,78,0.25);font-size:24px;text-align:left;padding-left:24px;").arg(m_opacity);

    m_themeName = m_themeSetting->get("style-name").toString();
    if(m_themeName == "ukui-dark") {
        m_searchEditBtn->setStyleSheet(styleSheetDark);

    } else {
        m_searchEditBtn->setStyleSheet(styleSheetLight);
    }

}

void TabletPluginWidget::setSearchOpacityEffect(const qreal &num)
{
    m_opacity = num;
    changeSearchBoxBackground();
}

void TabletPluginWidget::startUKUISearch()
{
    QDBusInterface session("com.ukui.search.service", "/", "org.ukui.search.service");
    session.call("showWindow");
}
