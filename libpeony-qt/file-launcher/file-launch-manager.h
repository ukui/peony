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

#ifndef FILELAUNCHMANAGER_H
#define FILELAUNCHMANAGER_H

#include <QObject>
#include "peony-core_global.h"

namespace Peony {

class FileLaunchAction;

/*!
 * \brief The FileLaunchManager class
 * \todo
 * support open multi-files.
 */
class FileLaunchManager : public QObject
{
    Q_OBJECT
public:
    static FileLaunchAction *getDefaultAction(const QString &uri);
    static const QList<FileLaunchAction*> getRecommendActions(const QString &uri);
    static const QList<FileLaunchAction*> getFallbackActions(const QString &uri);
    static const QList<FileLaunchAction*> getAllActionsForType(const QString &uri);
    static const QList<FileLaunchAction*> getAllActions(const QString &uri);
    /*!
     * \brief setDefaultLauchAction
     * \param uri
     * \param action
     * \note
     * set the files default lauch action which
     * have same mime type with the file's
     * type passing uri represent.
     */
    static void setDefaultLauchAction(const QString &uri, FileLaunchAction *action);

    static void openSync(const QString &uri);
    static void openAsync(const QString &uri);

private:
    explicit FileLaunchManager(QObject *parent = nullptr);
};

}

#endif // FILELAUNCHMANAGER_H
