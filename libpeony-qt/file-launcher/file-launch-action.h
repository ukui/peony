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

#ifndef FILELAUNCHACTION_H
#define FILELAUNCHACTION_H

#include <QAction>
#include <gio/gio.h>

#include "peony-core_global.h"

namespace Peony {

/*!
 * \brief The FileLaunchAction class
 * \todo
 * add error reprot GUI.
 */
class FileLaunchAction : public QAction
{
    Q_OBJECT
public:
    explicit FileLaunchAction(const QString &uri,
                              GAppInfo *app_info,
                              bool forceWithArg = false,
                              QObject *parent = nullptr);
    ~FileLaunchAction() override;
    const QString getUri();
    bool isDesktopFileAction();
    const QString getAppInfoName();
    const QString getAppInfoDisplayName();
    GAppInfo *gAppInfo() {return m_app_info;}

protected:
    bool isValid();

public Q_SLOTS:
    void lauchFileSync(bool forceWithArg = false);
    void lauchFileAsync(bool forceWithArg = false);

private:
    QString m_uri;
    bool m_is_desktop_file;
    GAppInfo *m_app_info;

    QIcon m_icon;
    QString m_info_name;
    QString m_info_display_name;

    bool m_force_with_arg = false;
};

}

#endif // FILELAUNCHACTION_H
