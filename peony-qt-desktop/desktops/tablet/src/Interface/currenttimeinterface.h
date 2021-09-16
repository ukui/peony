#ifndef CURRENTTIMEINTERFACE_H
#define CURRENTTIMEINTERFACE_H
#include <QString>
#include <QLocale>
#include <QGSettings>
#include <QTimer>
#include <QDateTime>
#include <QObject>

class CurrentTimeInterface : QObject
{

public:
    CurrentTimeInterface();
    ~CurrentTimeInterface();

    QString currentTime;
    QString currentWeek;
    QString currentDate;

protected:
    void twelveHourTime();
    void twentyFourHourTime();
    void getDateTime(QString timeFormat);

private:
    QDateTime* dateTime;
    QTimer* myTimer;
    QGSettings *timeSetting=nullptr;
    QString timeFormat;
    QString timeSlash;

};

#endif // CURRENTTIMEINTERFACE_H
