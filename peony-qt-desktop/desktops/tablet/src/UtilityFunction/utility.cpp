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

#include "utility.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>

const QPixmap loadSvg(const QString &fileName, const int size)
{
    QPixmap pixmap(size, size);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    return pixmap;
}
void debugLog(QString strMsg)
{
    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.log";
    QFile confFile(path);
    if(confFile.open(QIODevice::Text | QIODevice::ReadWrite | QIODevice::Append))
    {
        QString text=QString("%1 %2")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(strMsg);
        QTextStream textStream(&confFile);
        textStream<<text<<endl;
        textStream.flush();
        confFile.close();
    }
}
