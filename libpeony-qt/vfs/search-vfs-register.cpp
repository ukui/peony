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

#include "search-vfs-register.h"
#include "peony-search-vfs-file.h"
#include "peony-search-vfs-file-enumerator.h"
#include "search-vfs-manager.h"

#include <gio/gio.h>
#include <QDebug>

using namespace Peony;

bool is_registed = false;

static GFile *
test_vfs_parse_name (GVfs       *vfs,
                     const char *parse_name,
                     gpointer    user_data)
{
    QString tmp = parse_name;
    if (tmp.contains("real-uri:")) {
        QString realUri = tmp.split("real-uri:").last();
        return g_file_new_for_uri(realUri.toUtf8().constData());
    }
    return peony_search_vfs_file_new_for_uri(parse_name);
}

static GFile *
test_vfs_lookup (GVfs       *vfs,
                 const char *uri,
                 gpointer    user_data)
{
    return test_vfs_parse_name(vfs, uri, user_data);
}

void SearchVFSRegister::registSearchVFS()
{
    if (is_registed)
        return;

    //init manager
    Peony::SearchVFSManager::getInstance();

    GVfs *vfs;
    const gchar * const *schemes;
    gboolean res;

    vfs = g_vfs_get_default ();
    schemes = g_vfs_get_supported_uri_schemes(vfs);
    const gchar * const *p;
    p = schemes;
    while (*p) {
        qDebug()<<*p;
        p++;
    }

    res = g_vfs_register_uri_scheme (vfs, "search",
                                     test_vfs_lookup, NULL, NULL,
                                     test_vfs_parse_name, NULL, NULL);
}

SearchVFSRegister::SearchVFSRegister()
{

}
