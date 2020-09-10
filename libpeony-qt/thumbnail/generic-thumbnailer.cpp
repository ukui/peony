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

#include "generic-thumbnailer.h"
#include <QIcon>

#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include<QDir>
#include <QPainter>
#include <QMessageAuthenticationCode>
#include<QDesktopServices>

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

QIcon GenericThumbnailer::generateThumbnail(const QUrl &url, bool shadow, const QSize &size)
{
    QIcon icon;
    QFile file(url.path());
    if (!file.exists())
        return icon;

    //skip svg
    if (url.path().endsWith(".svg")) {
        if (file.size() < 1024*1024*8)
            icon.addFile(url.path());
        return icon;
    }

    QImage img(url.path());

    if (img.rect().size().width() > 128) {
        //scale large size image.
        if (size.isValid()) {
            img = img.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            img = img.scaledToWidth(128, Qt::SmoothTransformation);
        }
    }

    if (img.hasAlphaChannel()) {
        //skip shadow
        icon.addPixmap(QPixmap::fromImage(img));
        return icon;
    }

    if (shadow) {
        QPixmap pixmap = QPixmap::fromImage(img);
        pixmap = pixmap.scaled(img.rect().adjusted(4, 4, -4, -4).size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QImage newImg(img.size(), QImage::Format_ARGB32);
        newImg.fill(Qt::transparent);
        QPainter p(&newImg);

        p.setPen(Qt::transparent);
        p.setBrush(Qt::gray);
        p.drawRect(newImg.rect().adjusted(4, 4, -4, -4));

        qt_blurImage(newImg, 4, false, false);
        p.drawPixmap(newImg.rect().adjusted(4, 4, -4, -4), pixmap);

        p.end();
        icon.addPixmap(QPixmap::fromImage(newImg));
    } else {
        icon.addPixmap(QPixmap::fromImage(img));
    }

    return icon;
}

QIcon GenericThumbnailer::generateThumbnail(const QString &path, bool shadow, const QSize &size)
{
    QIcon icon;
    QFile file(path);
    if (!file.exists())
        return icon;

    //skip svg
    if (path.endsWith(".svg")) {
        if (file.size() < 1024*1024*8)
            icon.addFile(path);
        return icon;
    }

    QImage img(path);
    if (img.rect().size().width() > 128) {
        //scale large size image.
        if (size.isValid()) {
            img = img.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            img = img.scaledToWidth(128, Qt::SmoothTransformation);
        }
    }

    if (img.hasAlphaChannel()) {
        //skip shadow
        icon.addPixmap(QPixmap::fromImage(img));
        return icon;
    }

    if (shadow) {
        QPixmap pixmap = QPixmap::fromImage(img);
        pixmap = pixmap.scaled(img.rect().adjusted(4, 4, -4, -4).size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QImage newImg(img.size(), QImage::Format_ARGB32);
        newImg.fill(Qt::transparent);
        QPainter p(&newImg);

        p.setPen(Qt::transparent);
        p.setBrush(Qt::gray);
        p.drawRect(newImg.rect().adjusted(4, 4, -4, -4));

        qt_blurImage(newImg, 4, false, false);
        p.drawPixmap(newImg.rect().adjusted(4, 4, -4, -4), pixmap);

        p.end();
        icon.addPixmap(QPixmap::fromImage(newImg));
    } else {
        icon.addPixmap(QPixmap::fromImage(img));
    }

    return icon;
}

QIcon GenericThumbnailer::generateThumbnail(const QPixmap &pixmap, bool shadow, const QSize &size)
{
    QIcon icon;
    QPixmap tmp = pixmap;
    if (pixmap.isNull())
        return icon;

    QSize realSize;
    if (size.isValid()) {
        realSize = size;
        tmp = tmp.scaled(size);
    } else {
        tmp = tmp.scaledToWidth(128, Qt::SmoothTransformation);
        realSize = tmp.size();
    }

    if (shadow) {
        QImage newImg(realSize, QImage::Format::Format_ARGB32);
        newImg.fill(Qt::transparent);

        tmp = tmp.scaled(tmp.rect().adjusted(4, 4, -4, -4).size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QPainter p(&newImg);

        p.setPen(Qt::transparent);
        p.setBrush(Qt::gray);
        p.drawRect(newImg.rect().adjusted(4, 4, -4, -4));

        qt_blurImage(newImg, 4, false, false);
        p.drawPixmap(newImg.rect().adjusted(4, 4, -4, -4), tmp);

        p.end();
        icon.addPixmap(QPixmap::fromImage(newImg));
    } else {
        icon.addPixmap(tmp);
    }
    return icon;
}

GenericThumbnailer::GenericThumbnailer(QObject *parent) : QObject(parent)
{

}

QString GenericThumbnailer::codeMd5(QString fileName)
{
    QMessageAuthenticationCode code(QCryptographicHash::Md5);
    code.addData(fileName.toUtf8());
    return code.result().toHex();
}

QString GenericThumbnailer::codeMd5WithModifyTime(QString fileName, quint64 &modifyTime)
{
    QString contentText = fileName + QString::number(modifyTime);
    QMessageAuthenticationCode code(QCryptographicHash::Md5);
    code.addData(contentText.toUtf8());
    return code.result().toHex();
}

QString GenericThumbnailer::cachDir()
{
    QString location;
#if QT_VERSION >= 0x050000
    location=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#else
    location=QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif

    QDir dir(location);
    if(!dir.exists())
        dir.mkpath(".");
    return location;
}

QString GenericThumbnailer::thumbnaileCachDir()
{
    QString location="/tmp/.cache/peony";
    location+="/thumbnails";

    QDir dir(location);
    if(!dir.exists())
    {
        dir.mkpath(".");
    }

    return location;
}
