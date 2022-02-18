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

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QGSettings>
#include <QHBoxLayout>
#include "src/Interface/currenttimeinterface.h"

class ToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit ToolBox(QWidget *parent,int w, int h);
    ~ToolBox();
    int type=-1;
    void setToolOpacityEffect(const qreal& num);

protected:
    void initUi();
    void openSearchBox();
    void openFocusMode();
    void openNote();
    void drawfocusStatus();
    void changeToolBoxBackground();
private:
    int m_width=0;
    int m_height=0;
    QGSettings *themeSetting=nullptr;
    QString themeName;
    QWidget* timeWidget=nullptr;
    QWidget* toolWidget=nullptr;
    QVBoxLayout* timeLayout=nullptr;
    QVBoxLayout* toolLayout=nullptr;
    QLabel* hourLabel=nullptr;
    QLabel* minuteLabel=nullptr;
    QGSettings* pluginSetting=nullptr;
    QGSettings *timeSetting=nullptr;
    QTimer* myTimer=nullptr;

    bool isFocusmodeRun=false;
    QLabel* focusStatus=nullptr;
    QHBoxLayout* focusBoxLayout=nullptr;

    CurrentTimeInterface* Time=nullptr;

    QPushButton* searchBtn;
    QPushButton* focusBtn;
    QPushButton* noteBtn;

    QWidget* searchWidget=nullptr;
    QWidget* noteWidget=nullptr;

    QGraphicsDropShadowEffect* effect=nullptr;

    qreal opacity;


Q_SIGNALS:
    void pageSpread(); //右滑页面展开
};

#endif // TOOLBOX_H
