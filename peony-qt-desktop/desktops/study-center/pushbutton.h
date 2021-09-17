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

#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QSettings>
//#include "src/Style/style.h"
//#include "src/Interface/ukuimenuinterface.h"
//#include "src/Style/style.h"
//#include "src/RightClickMenu/rightclickmenu.h"

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    /**
     * @param classify为0时为分类按钮、1为应用按钮
     * @param module为0时为常用模块，1为字母模块、2为功能模块
     */
    PushButton(QWidget *parent,QString name,int width,int height);
    ~PushButton();

private:
    QString name;
    int width;
    int height;

protected:
    void initAppBtn();

};

#endif // PUSHBUTTON_H
