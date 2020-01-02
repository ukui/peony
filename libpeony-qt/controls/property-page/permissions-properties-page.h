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

#ifndef PERMISSIONSPROPERTIESPAGE_H
#define PERMISSIONSPROPERTIESPAGE_H

#include <QWidget>

#include "peony-core_global.h"
#include <memory>
#include <gio/gio.h>

class QLabel;
class QTableWidget;
class QCheckBox;

namespace Peony {

class FileWatcher;

class PEONYCORESHARED_EXPORT PermissionsPropertiesPage : public QWidget
{
    Q_OBJECT
public:
    explicit PermissionsPropertiesPage(const QStringList &uris, QWidget *parent = nullptr);
    ~PermissionsPropertiesPage();

protected:
    static GAsyncReadyCallback async_query_permisson_callback(GObject *obj,
                                                              GAsyncResult *res,
                                                              PermissionsPropertiesPage *p_this);

    void queryPermissionsAsync(const QString&, const QString &uri);

protected Q_SLOTS:
    void changePermission(int row, int column, bool checked);

Q_SIGNALS:
    void checkBoxChanged(int row, int column, bool checked);

private:
    QString m_uri;
    std::shared_ptr<FileWatcher> m_watcher;

    QLabel *m_label;
    QLabel *m_message;
    QTableWidget *m_table;

    bool m_permissions[3][3];
};

}

#endif // PERMISSIONSPROPERTIESPAGE_H
