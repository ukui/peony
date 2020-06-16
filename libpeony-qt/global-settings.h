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

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutex>

#include "peony-core_global.h"

#define FORBID_THUMBNAIL_IN_VIEW "do-not-thumbnail"
#define SHOW_HIDDEN_PREFERENCE "show-hidden"
#define SORT_CHINESE_FIRST "chinese-first"
#define SORT_FOLDER_FIRST "folder-first"
#define RESIDENT_IN_BACKEND "resident"
#define LAST_DESKTOP_SORT_ORDER "last-desktop-sort-order"
#define ALLOW_FILE_OP_PARALLEL "allow-file-op-parallel"
#define DEFAULT_WINDOW_SIZE "default-window-size"
#define DEFAULT_SIDEBAR_WIDTH "default-sidebar-width"

//gsettings
#define SIDEBAR_BG_OPACITY "sidebar-bg-opacity"

class QGSettings;

namespace Peony {

/*!
 * \brief The GlobalSettings class
 * \details
 * this class provide a global settings for peony-qt.
 * it most be used to save and load preferences of peony-qt, such as default view,
 * sort type, etc.
 *
 * you can also save another kind of datas using by extensions. such as enable properties.
 * this class instance is shared in both peony-qt and its plugins.
 */
class PEONYCORESHARED_EXPORT GlobalSettings : public QObject
{
    Q_OBJECT
public:
    static GlobalSettings *getInstance();
    const QVariant getValue(const QString &key);
    bool isExist(const QString &key);

Q_SIGNALS:
    void valueChanged(const QString &key);

public Q_SLOTS:
    void setValue(const QString &key, const QVariant &value);
    void reset(const QString &key);
    void resetAll();

    /*!
     * \brief forceSync
     * \param key
     * \details
     * use force sync to ensure the settings key-value and cached map's data
     * keep same. this may be used in out progress, such as peony-qt-desktop.
     */
    void forceSync(const QString &key = nullptr);

private:
    explicit GlobalSettings(QObject *parent = nullptr);
    ~GlobalSettings();

    QSettings *m_settings;
    QMap<QString, QVariant> m_cache;

    QGSettings *m_gsettings = nullptr;

    QMutex m_mutex;
};

}

#endif // GLOBALSETTINGS_H
