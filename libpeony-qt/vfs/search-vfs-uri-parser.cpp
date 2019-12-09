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
 * Authors: Meihong He <hemeihong@kylinos.cn>
 *
 */

#include "search-vfs-uri-parser.h"
#include "file-utils.h"

#include <QStringList>
#include <QDebug>
using namespace Peony;

SearchVFSUriParser::SearchVFSUriParser()
{

}

const QString SearchVFSUriParser::parseSearchKey(const QString &uri, const QString &key)
{
    return QString("search:///search_uris="+uri+"&name_regexp="+key+"&recursive=1");
}

const QString SearchVFSUriParser::getSearchUriNameRegexp(const QString &searchUri)
{
    auto string = searchUri;
    string.remove("search:///");
    auto list = string.split("&");
    for (auto arg : list) {
        if (arg.startsWith("name_regexp=")) {
            qDebug()<<arg;
            auto tmp = arg.remove("name_regexp=");
            return tmp;
        }
    }
    return nullptr;
}

const QString SearchVFSUriParser::getSearchUriTargetDirectory(const QString &searchUri)
{
    auto string = searchUri;
    string.remove("search:///");
    auto list = string.split("&");
    for (auto arg : list) {
        if (arg.startsWith("search_uris=")) {
            qDebug()<<"arg:"<<arg;
            auto tmp = arg.remove("search_uris=");
            auto uris = tmp.split(",");
            if (uris.count() == 1) {
                return FileUtils::getFileDisplayName(tmp);
            }
            tmp = nullptr;
            QStringList names;
            for (auto uri : uris) {
                auto displayName = FileUtils::getFileDisplayName(uri);
                if (tmp == nullptr)
                    tmp = displayName;
                else
                    tmp = tmp + ", " + displayName;
            }
            return tmp;
        }
    }
    return nullptr;
}
