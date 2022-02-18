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
