/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 * Authors: yanling <yanling@kylinos.cn>
 *
 */

#ifndef USERSHARE_MANAGER_H
#define USERSHARE_MANAGER_H

#include <QMap>
#include <QMutex>
#include <QObject>
//#include <QProcess>
#include "peony-core_global.h"
namespace Peony {

class PEONYCORESHARED_EXPORT ShareInfo
{
public:
    ShareInfo& operator= (const ShareInfo*);

    QString name;
    QString comment;
    QString originalPath;

    bool readOnly   = true;
    bool allowGuest = false;
    bool isShared   = false;
};


class PEONYCORESHARED_EXPORT UserShareInfoManager : public QObject
{
    Q_OBJECT
public:
    static UserShareInfoManager* getInstance ();
    QString exectueCommand (QStringList& args, bool* ret /* out */, QString sharedPath="");

    bool hasSharedInfo (QString& name);
    void removeShareInfo (QString& name);
    bool addShareInfo (ShareInfo* shareInfo);
    bool updateShareInfo (ShareInfo& shareInfo);
    const ShareInfo* getShareInfo (QString& name);

private:
    explicit UserShareInfoManager (QObject* parent = nullptr) : QObject(parent) {};

Q_SIGNALS:
    void signal_addSharedFolder(const ShareInfo& shareInfo, bool successed);
    void signal_deleteSharedFolder(const QString& originalPath, bool successed);

private:
    bool                            m_bInit = false;
    QMutex                          m_mutex;
    QMap <QString, ShareInfo*>      m_sharedInfoMap;
    static UserShareInfoManager*    g_shareInfo;
};
}
#endif // USERSHARE_MANAGER_H
