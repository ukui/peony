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

#ifndef SIDEBARVFSITEM_H
#define SIDEBARVFSITEM_H

#include "peony-core_global.h"
#include "side-bar-abstract-item.h"
#include "gerror-wrapper.h"

namespace Peony {

class SideBarModel;
class VFSPluginIface;
class FileEnumerator;

class PEONYCORESHARED_EXPORT SideBarVFSItem : public SideBarAbstractItem
{
    Q_OBJECT
public:
    explicit SideBarVFSItem(const QString& uri, SideBarVFSItem* parentItem, SideBarModel *model, QObject *parent = nullptr);
    ~SideBarVFSItem();

    Type type() override {
        return SideBarAbstractItem::FileSystemItem;
    }

    QString uri() override;
    QString displayName() override;
    QString iconName() override;

    bool hasChildren() override {
        return true;
    }
    SideBarAbstractItem *parent() override {
        return m_parentItem;
    }

public Q_SLOTS:
    void findChildren() override;
    void findChildrenAsync() override;
    //void clearChildren() override;
    void slot_enumeratorFinish(bool successed);

private:
    SideBarVFSItem *m_parentItem = nullptr;
    FileEnumerator *m_enumerator = nullptr;
};

}

#endif // SIDEBARVFSITEM_H
