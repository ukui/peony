/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#ifndef NETUSERSHAREHELPER_H
#define NETUSERSHAREHELPER_H

#include <QObject>

class ShareInfo
{
public:
    ShareInfo(const QString &item = nullptr, bool init = false);

    bool operator ==(const ShareInfo &info) {return this->name == info.name;}

    QString name;
    QString originalPath;
    QString comment;
    bool readOnly = true;
    bool allowGuest = false;

    bool isShared = false;
};

class NetUsershareHelper : public QObject
{
    Q_OBJECT
public:
    static const QStringList getSharedItems();
    static ShareInfo getShareItemInfo(const QString &item);

    /*!
     * \brief addShareCmd
     * \param itemDisplayName
     * \param orignalPath
     * \param comment
     * \param readonly
     * \param allowGuest
     *
     * \details
     * Add/modify user defined share
     */
    static void addShareCmd(const QString &itemDisplayName,
                            const QString &orignalPath,
                            const QString &comment,
                            bool readonly,
                            bool allowGuest);

    static bool updateShareInfo(ShareInfo &info);

    static void removeShared(const QString &item);

private:
    explicit NetUsershareHelper(QObject *parent = nullptr);
};

#endif // NETUSERSHAREHELPER_H
