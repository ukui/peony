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

#ifndef LINUXPWDHELPER_H
#define LINUXPWDHELPER_H

#include <QObject>

#include <pwd.h>
#include <sys/types.h>

class PWDItem
{
public:
    explicit PWDItem(passwd *user);
    ~PWDItem() {}

    const QString userName() {return m_user_name;}
    int userId() {return m_uid;}
    int groupId() {return m_gid;}

    const QString fullName() {return m_full_name;}
    const QString homeDir() {return m_home_dir;}
    const QString shellDir() {return m_shell_dir;}

private:
    QString m_user_name;
    QString m_full_name;
    QString m_home_dir;
    QString m_shell_dir;

    int m_uid = -1;
    int m_gid = -1;
};

class LinuxPWDHelper
{
public:
    static const QList<PWDItem> getAllUserInfos();
    static const PWDItem getCurrentUser();

private:
    LinuxPWDHelper();
};

#endif // LINUXPWDHELPER_H
