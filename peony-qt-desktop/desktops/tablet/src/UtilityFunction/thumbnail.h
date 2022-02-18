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

#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "src/Interface/ukuimenuinterface.h"
#define TEXT_LENGTH 100       //文字长度

class ThumbNail : public QWidget
{
    Q_OBJECT
public:
    ThumbNail(QWidget *parent = nullptr);
    ~ThumbNail();
    void setupthumbnail(QString desktopfp);
    QPixmap getIcon(QString desktopfp,int width,int height);
    bool appDisable(QString desktopfp);

private:
    QLabel *iconLabel=nullptr;
    QVBoxLayout *layout=nullptr;
    QLabel *m_icon=nullptr;
    QLabel *textLabel = nullptr;
    UkuiMenuInterface *pUkuiMenuInterface = nullptr;
    QString appIcon=nullptr;
    QSettings *setting=nullptr;
    QSettings *disableSetting=nullptr;

private:
    void initUi();
};

#endif // THUMBNAIL_H
