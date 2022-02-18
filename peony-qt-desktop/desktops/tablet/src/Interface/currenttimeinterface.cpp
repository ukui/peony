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

#include "currenttimeinterface.h"
#include <QTimer>
#define TIME_FORMAT "org.ukui.control-center.panel.plugins"
#define TIME_FORMAT_KEY "hoursystem"
#include <QDebug>
#include <QGSettings>

CurrentTimeInterface::CurrentTimeInterface()
{
    //获得语言环境
    dateTime = new QDateTime;
    myTimer=new QTimer();
    myTimer->start(1000);
    timeSetting=new QGSettings(TIME_FORMAT);
    timeFormat=timeSetting->get(TIME_FORMAT_KEY).toString();
    timeSlash=timeSetting->get("date").toString();
    getDateTime(timeFormat);

    connect(timeSetting,&QGSettings::changed,this, [=](const QString &key)
    {
        if(key=="hoursystem")
        {
            timeFormat=timeSetting->get(TIME_FORMAT_KEY).toString();
            getDateTime(timeFormat);
        }
        else if(key=="date")
        {
            timeSlash=timeSetting->get("date").toString();
            if(timeSlash=="en")
            {
                currentDate=currentDate.replace("/","-");
            }
            else
            {
                currentDate=currentDate.replace("-","/");
            }
        }
    });

    connect(myTimer,&QTimer::timeout,[this] () {
        getDateTime(timeFormat);
    });

}

CurrentTimeInterface::~CurrentTimeInterface()
{
    if(myTimer)
        delete myTimer;
    if(timeSetting)
        delete timeSetting;
    if(dateTime)
        delete dateTime;
    myTimer=nullptr;
    timeSetting=nullptr;
    dateTime=nullptr;

}

void CurrentTimeInterface::getDateTime(QString timeFormat)
{
    if(timeFormat=="12")
    {
        twelveHourTime();
    }
    else if(timeFormat=="24")
    {
        twentyFourHourTime();
    }
    if(timeSlash=="cn")
    {
       currentDate=currentDate.replace("-","/");
    }

}

//12制小时时间
void CurrentTimeInterface::twelveHourTime()
{
    currentTime=dateTime->currentDateTime().time().toString("hh:mm AP").split(" ").first();
    currentDate=dateTime->currentDateTime().date().toString("ddd MM-dd");
    currentWeek=dateTime->currentDateTime().time().toString("AP");


}

//24小时制时间
void CurrentTimeInterface::twentyFourHourTime()
{
    currentTime=dateTime->currentDateTime().time().toString("hh:mm");
    currentDate=dateTime->currentDateTime().date().toString("MM-dd");
    currentWeek=dateTime->currentDateTime().date().toString("ddd");
}
