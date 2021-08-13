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
