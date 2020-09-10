/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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

#include "bookmark-manager.h"

#include <QtConcurrent>
#include <glib.h>

#include <QDebug>

using namespace Peony;

static BookMarkManager *global_instance = nullptr;

BookMarkManager *BookMarkManager::getInstance()
{
    if (!global_instance) {
        global_instance = new BookMarkManager;
    }
    return global_instance;
}

BookMarkManager::BookMarkManager(QObject *parent) : QObject(parent)
{
    QtConcurrent::run([=]() {
        m_book_mark = new QSettings(QSettings::UserScope, "org.ukui", "peony-qt");
        m_uris = m_book_mark->value("uris").toStringList();
        m_is_loaded = true;
        //m_uris<<"computer:///";
        //qDebug()<<"====================ok============\n\n\n\n"<<m_uris;
        Q_EMIT this->urisLoaded();
    });
}

BookMarkManager::~BookMarkManager()
{
    if (m_book_mark) {
        m_book_mark->deleteLater();
    }
}

void BookMarkManager::addBookMark(const QString &uri)
{
    QtConcurrent::run([=]() {
        while (!this->isLoaded()) {
            g_usleep(100);
        }
        QUrl url = uri;
        QString origin_path = "file://" + url.path();
        if (m_mutex.tryLock(1000)) {
            bool successed = !m_uris.contains(origin_path);
            if (successed) {
                m_uris<<origin_path;
                m_uris.removeDuplicates();
                m_book_mark->setValue("uris", m_uris);
                m_book_mark->sync();
                qDebug()<<"addBookMark"<<origin_path;
                Q_EMIT this->bookMarkAdded(origin_path, true);
            } else {
                Q_EMIT this->bookMarkAdded(origin_path, false);
            }
            m_mutex.unlock();
        } else {
            Q_EMIT this->bookMarkAdded(origin_path, false);
        }
    });
}

void BookMarkManager::removeBookMark(const QString &uri)
{
    QtConcurrent::run([=]() {
        while (!this->isLoaded()) {
            g_usleep(100);
        }
        QUrl url = uri;
        QString origin_path = "file://" + url.path();
        if (m_mutex.tryLock(1000)) {
            bool successed = m_uris.contains(origin_path);
            if (successed) {
                m_uris.removeOne(origin_path);
                m_uris.removeDuplicates();
                m_book_mark->setValue("uris", m_uris);
                m_book_mark->sync();
                qDebug()<<"removeBookMark"<<origin_path;
                Q_EMIT this->bookMarkRemoved(origin_path, true);
            } else {
                Q_EMIT this->bookMarkRemoved(origin_path, false);
            }
            m_mutex.unlock();
        } else {
            Q_EMIT this->bookMarkRemoved(origin_path, false);
        }
    });
}
