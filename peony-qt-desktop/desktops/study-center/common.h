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

#ifndef COMMON_H
#define COMMON_H
typedef struct TabletApp
{
    long int iTime;
    quint32 serialNumber; //排序序号
    QString desktopName;  //desktop文件名
    QString appName;      //应用名称
    QString appIcon;      //应用图标路径
    QString execCommand;  //应用的执行路径

}TABLETAPP;
Q_DECLARE_METATYPE(TABLETAPP)
#endif // COMMON_H
