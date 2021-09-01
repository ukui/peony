//
// Created by hxf on 2021/8/16.
//

#ifndef PEONY_TABLET_PLUGIN_WIDGET_H
#define PEONY_TABLET_PLUGIN_WIDGET_H

#include "src/Interface/currenttimeinterface.h"
#include "src/Style/style.h"
#include "pluginwidget.h"


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

    pluginwidget *getPluginWidget();

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
    pluginwidget *m_focusPlug = nullptr;

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
