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

#ifndef PERMISSIONSPROPERTIESPAGE_H
#define PERMISSIONSPROPERTIESPAGE_H

#include <QVBoxLayout>
#include <QWidget>

#include "properties-window-tab-iface.h"
#include "peony-core_global.h"

#include <memory>
#include <gio/gio.h>

class QLabel;
class QTableWidget;
class QCheckBox;

namespace Peony {

class FileWatcher;

class PEONYCORESHARED_EXPORT PermissionsPropertiesPage : public PropertiesWindowTabIface
{
    Q_OBJECT
public:
    explicit PermissionsPropertiesPage(const QStringList &uris, QWidget *parent = nullptr);
    ~PermissionsPropertiesPage();

    /*!
     * init the main Widget
     * \brief initTabWidget
     */
    void initTableWidget();

    /**
     * \brief 创建一个自定义的表格单元组件，带图标和文字
     * \param parent
     * \param icon
     * \param text
     * \return
     */
    static QWidget* createCellWidget(QWidget* parent,QIcon icon, QString text);

    void savePermissions();

    void updateCheckBox();

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

    QVBoxLayout *m_layout = nullptr;

    QLabel *m_label         = nullptr;
    QLabel *m_message       = nullptr;
    QTableWidget *m_table   = nullptr;

    //防止错误修改权限
    bool m_enable = false;

    bool m_permissions[3][3];

    //unixmode能力，标识修改权限是否可使用gio接口
    bool m_has_unix_mode = false;

public:
    void thisPageChanged() override;

    // PropertiesWindowTabIface interface
public:
    void saveAllChange();
};

}

#endif // PERMISSIONSPROPERTIESPAGE_H
