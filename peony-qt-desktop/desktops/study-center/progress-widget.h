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

#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>
#include "common.h"
class QProgressBar;
class QString;
class QLabel;
class QGridLayout;
//class QHBoxLayout;

class ProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWidget(const TABLETAPP &AppInfo,QWidget *parent = nullptr);
    ~ProgressWidget();
    void initUi();

private:
    long int m_iTime ;
    quint32 m_serialNumber; //排序序号
    QString m_strAppName;
    QString m_strAppIcon;
    QProgressBar* m_processBar=nullptr;
    QLabel* m_nameLabel=nullptr;
    QLabel* m_iconLabel=nullptr;
    QLabel* m_timeLabel=nullptr;
    QGridLayout* m_timeGridLayout = nullptr;
  //  QHBoxLayout* m_iconGridLayout = nullptr;
public Q_SLOTS:
   void paintSlot(TABLETAPP &applist);
   void setMaximum(int iMaxValue);

   /**
    * 点击主题切换发送信号，字体样式发生改变
    * @param QString 主题样式
    */
   void changeThemeSlot(QString strTheme);

};

#endif // PROGRESSWIDGET_H
