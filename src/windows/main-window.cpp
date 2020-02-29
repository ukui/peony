/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "main-window.h"

#include "border-shadow-effect.h"
#include <private/qwidgetresizehandler_p.h>

#include <QVariant>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_effect = new BorderShadowEffect(this);
    m_effect->setPadding(4);
    m_effect->setBorderRadius(6);
    m_effect->setBlurRadius(4);
    setGraphicsEffect(m_effect);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);
    setContentsMargins(4, 4, 4, 4);

    //bind resize handler
    auto handler = new QWidgetResizeHandler(this);
    handler->setMovingEnabled(false);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    validBorder();
    update();
    QMainWindow::resizeEvent(e);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    validBorder();
    QColor color = this->palette().base().color();
    color.setAlphaF(0.5);
    m_effect->setWindowBackground(color);
    QMainWindow::paintEvent(e);
}

void MainWindow::validBorder()
{
    if (this->isMaximized()) {
        setContentsMargins(0, 0, 0, 0);
        m_effect->setPadding(0);
        setProperty("blurRegion", QVariant());
    } else {
        setContentsMargins(4, 4, 4, 4);
        m_effect->setPadding(4);
        QPainterPath path;
        auto rect = this->rect();
        rect.adjust(4, 4, -4, -4);
        path.addRoundedRect(rect, 6, 6);
        setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    }
}
