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
int Style::appColumn=0;
int Style::appPage=1;
int Style::appNum=0;
int Style::nowpagenum=1;
int Style::appRows=0;
bool Style::ScreenRotation=false;
int Style::AppListItemSizeWidth=216;
int Style::AppListItemSizeHeight=270;
//int Style::ScreenWidth=1920;
//int Style::ScreenHeight=1080;
int Style::AppListViewTopMargin=0;
int Style::AppListViewLeftMargin=52;
int Style::AppListViewBottomMargin=20;
int Style::AppListViewRightMargin=0;
int Style::TimeWidgetTop=81;
int Style::TimeWidgetLeft=112;

int Style::AppListIconSize=96;
int Style::AppTopSpace=60;
int Style::AppLeftSpace=60;

int Style::GroupAppColumn=1;
int Style::GroupAppRow=1;

int Style::ButtonWidgetWidth=56;

//int Style::WideAppListItemSizeWidth=284;
//int Style::WideAppListItemSizeHeight=248;
//int Style::WideAppViewLeftMargin=44;
int Style::ToolWidgetLeft=24;
bool Style::IsWideScreen=false;

void Style::initWidStyle()
{

    if(ScreenRotation)
    {

        AppListViewTopMargin=0;
        AppListViewLeftMargin=84;
        AppListViewRightMargin=0;
        AppListViewBottomMargin=32;
        AppListItemSizeWidth=152;
        AppListItemSizeHeight=216;
        AppTopSpace=40;
        AppLeftSpace=28;
        TimeWidgetTop=96;
        TimeWidgetLeft=104;
        ButtonWidgetWidth=32;
    }
    else
    {

        AppListViewTopMargin=0;
        AppListViewLeftMargin=52;
        AppListViewBottomMargin=20;
        AppListViewRightMargin=0;
        AppListItemSizeWidth=216;
        AppListItemSizeHeight=270;
        AppTopSpace=60;
        AppLeftSpace=60;
        TimeWidgetTop=81;
        TimeWidgetLeft=112;
        ButtonWidgetWidth=56;
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




