/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "style.h"
#include <QDebug>


Style::Style()
{

}


int Style::appLine=0;
int Style::appColumn=6;
int Style::appPage=1;
int Style::appNum=0;
int Style::nowpagenum=1;
bool Style::ScreenRotation=false;
int Style::AppListItemSizeWidth=216;
int Style::AppListItemSizeHeight=245;
//int Style::ScreenWidth=1920;
//int Style::ScreenHeight=1080;
int Style::AppListViewHeight=245 * 4;
int Style::AppListViewWidth=216 * 6;
int Style::AppListViewTopMargin=50;
int Style::AppListViewLeftMargin=50;
int Style::AppListViewBottomMargin=5;
int Style::AppListViewRightMargin=50;
int Style::TimeWidgetTop=50;
int Style::TimeWidgetLeft=110;

int Style::AppListIconSize=96;
int Style::AppTopSpace=60;
int Style::AppLeftSpace=60;

int Style::GroupAppColumn=1;
int Style::GroupAppRow=1;

int Style::ButtonWidgetWidth=56;
int Style::ButtonWidgetHeight=90;

//int Style::WideAppListItemSizeWidth=284;
//int Style::WideAppListItemSizeHeight=248;
//int Style::WideAppViewLeftMargin=44;
int Style::ToolWidgetLeft=24;
bool Style::IsWideScreen=false;

int Style::BigIconSize=106;
int Style::SmallIconSize=96;
int Style::Margin=74;
int Style::topSpace=16;
int Style::GridHeight=170;
int Style::GridWidth=155;
int Style::itemWidth=120;
int Style::itemHeight=145;

void Style::initWidStyle()
{
    AppListViewTopMargin=50;
    AppListViewLeftMargin=50;
    AppListViewRightMargin=50;
    AppListViewBottomMargin=5;


    if (ScreenRotation) {
        AppListItemSizeWidth = 152;
        AppListItemSizeHeight = 216;
        AppTopSpace = 40;
        AppLeftSpace = 28;

        TimeWidgetTop = 84;
        TimeWidgetLeft = 84;

    } else {
        AppListItemSizeWidth = 216;
        AppListItemSizeHeight = 245;
        AppTopSpace = 60;
        AppLeftSpace = 60;

        TimeWidgetTop = 90;
        TimeWidgetLeft = 0;
    }

//    QGSettings* setting=new QGSettings(QString("org.mate.interface").toLocal8Bit());
//    QString value=setting->get("font-name").toString();
//    QStringList valstr=value.split(" ");
//    int fontSize=valstr.at(valstr.count()-1).toInt();

//    int position=0;
//    int panelSize=0;
//    if(QGSettings::isSchemaInstalled(QString("org.ukui.panel.settings").toLocal8Bit()))
//    {
//        QGSettings* gsetting=new QGSettings(QString("org.ukui.panel.settings").toLocal8Bit());
//        if(gsetting->keys().contains(QString("panelposition")))
//            position=gsetting->get("panelposition").toInt();
//        else
//            position=0;
////        if(gsetting->keys().contains(QString("panelsize")))
////            panelSize=gsetting->get("panelsize").toInt();
////        else
////            panelSize=46;
//    }
//    else
//    {
//        position=0;
////        panelSize=46;
//    }


//    if(position==0 || position==2)
//    {
//        ScreenWidth=QApplication::primaryScreen()->geometry().width();
//        ScreenHeight=QApplication::primaryScreen()->geometry().height();
//    }
//    if(position==4 || position==6)
//    {
//        ScreenWidth=QApplication::primaryScreen()->geometry().width();
//        ScreenHeight=QApplication::primaryScreen()->geometry().height();
//    }

//    int len=0;
//    QString locale = QLocale::system().name();
//    if (locale == "zh_CN")
//        len=0;
//    else
//        len=10;


//    if (ScreenWidth>=1920 && ScreenWidth<2000) {


//        AppListViewTopMargin=20;
//        AppListViewLeftMargin=52;
//        AppListViewBottomMargin=20;
//        AppListViewRightMargin=0;
//        AppListItemSizeWidth=216;
//        AppListItemSizeHeight=248;
//        AppTopSpace=60;
//        AppLeftSpace=60;
//        TimeWidgetTop=104;
//        TimeWidgetLeft=104;
//        ButtonWidgetWidth=56;


//    }


}




