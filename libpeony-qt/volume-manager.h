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

#ifndef VOLUMEMANAGER_H
#define VOLUMEMANAGER_H

#include "peony-core_global.h"

#include <QObject>
#include <gio/gio.h>
#include <memory>

#include <QMetaType>

namespace Peony {

class PEONYCORESHARED_EXPORT Drive
{
public:
    Drive();//do not use this constructor
    Drive(GDrive *drive) {m_drive = drive;}
    ~Drive()
    {
        //do not unref the handle, it is managed by GVolumeMonitor.
        //g_object_unref(m_drive);
    }

    QString name()
    {
        if (!m_drive)
            return nullptr;
        char *name = g_drive_get_name(m_drive);
        QString value = name;
        g_free(name);
        return value;
    }
    QString iconName()
    {
        if (!m_drive)
            return nullptr;
        GThemedIcon *g_icon = G_THEMED_ICON(g_drive_get_icon(m_drive));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }
    QString symbolicIconName()
    {
        if (!m_drive)
            return nullptr;
        GThemedIcon *g_icon = G_THEMED_ICON(g_drive_get_symbolic_icon(m_drive));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }

    bool removable()
    {
        if (!m_drive)
            return false;
        return g_drive_is_removable(m_drive);
    }

    GDrive *getGDrive()
    {
        return m_drive;
    }

private:
    GDrive *m_drive = nullptr;
};

class PEONYCORESHARED_EXPORT Volume
{
public:
    Volume();//do not use this constructor
    Volume(GVolume *volume)
    {
        m_volume = volume;
    }

    ~Volume()
    {
        //do not unref the handle, it is managed by GVolumeMonitor.
        //g_object_unref(m_volume);
    }

    QString name()
    {
        char *name = g_volume_get_name(m_volume);
        QString value = name;
        g_free(name);
        return value;
    }

    QString iconName()
    {
        GThemedIcon *g_icon = G_THEMED_ICON(g_volume_get_icon(m_volume));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }

    QString symbolicIconName()
    {
        GThemedIcon *g_icon = G_THEMED_ICON(g_volume_get_symbolic_icon(m_volume));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }

    /*!
     * \brief uuid
     * \return the UUID for volume or NULL if no UUID can be computed.
     */
    QString uuid()
    {
        char *uuid = g_volume_get_uuid(m_volume);
        QString value = uuid;
        if (uuid)
            g_free(uuid);
        return value;
    }

    /*!
     * \brief rootUri
     * <br>
     * Gets the activation root uri for a GVolume
     * if it is known ahead of mount time.
     * Returns NULL otherwise.
     * </br>
     * \return the activation root uri or null.
     */
    QString rootUri()
    {
        GFile *root = g_volume_get_activation_root(m_volume);
        if (!G_IS_FILE (root))
            return nullptr;
        char *uri = g_file_get_uri(root);
        QString value = uri;
        g_free(uri);
        return value;
    }

    GVolume *getGVolume()
    {
        return m_volume;
    }

private:
    GVolume *m_volume = nullptr;
};

class PEONYCORESHARED_EXPORT Mount
{
public:
    Mount();//do not use this constructor
    Mount(GMount *mount)
    {
        m_mount = mount;
    }

    ~Mount()
    {
        //do not unref the handle, it is managed by GVolumeMonitor.
        //g_object_unref(m_mount);
    }

    QString name()
    {
        char *name = g_mount_get_name(m_mount);
        QString value = name;
        g_free(name);
        return value;
    }

    QString iconName()
    {
        GThemedIcon *g_icon = G_THEMED_ICON(g_mount_get_icon(m_mount));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }

    QString symbolicIconName()
    {
        GThemedIcon *g_icon = G_THEMED_ICON(g_mount_get_symbolic_icon(m_mount));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }

    /*!
     * \brief uuid
     * \return the UUID for mount or NULL if no UUID can be computed.
     */
    QString uuid()
    {
        char *uuid = g_mount_get_uuid(m_mount);
        QString value = uuid;
        if (uuid)
            g_free(uuid);
        return value;
    }

    /*!
     * \brief rootUri
     * \return
     * \note As volume has mounted, the rootUri never return null.
     */
    QString rootUri()
    {
        GFile *root = g_mount_get_root(m_mount);
        char *uri = g_file_get_uri(root);
        QString value = uri;
        g_free(uri);
        return value;
    }

    GMount *getGMount()
    {
        return m_mount;
    }

private:
    GMount *m_mount = nullptr;
};

class PEONYCORESHARED_EXPORT VolumeManager : public QObject
{
    Q_OBJECT
public:
    static VolumeManager *getInstance();

Q_SIGNALS:
    /*!
     * \brief driveConnected
     * \param drive
     * \note moc will failed when sending a GDrive handle into signal/slot.
     * so I have to take it with a custom class.
     * This case is also happend in GVolme and GMount handle.
     */
    void driveConnected(const std::shared_ptr<Drive> &drive);
    void driveDisconnected(const std::shared_ptr<Drive> &drive);
    void volumeAdded(const std::shared_ptr<Volume> &volume);
    void volumeRemoved(const std::shared_ptr<Volume> &volume);
    void mountAdded(const std::shared_ptr<Mount> &mount);
    void mountRemoved(const std::shared_ptr<Mount> &mount);

protected:
    static void drive_connected_callback(GVolumeMonitor *monitor,
                                         GDrive *drive,
                                         VolumeManager *p_this);

    static void drive_disconnected_callback(GVolumeMonitor *monitor,
                                            GDrive *drive,
                                            VolumeManager *p_this);

    static void volume_added_callback(GVolumeMonitor *monitor,
                                      GVolume *volume,
                                      VolumeManager *p_this);

    static void volume_removed_callback(GVolumeMonitor *monitor,
                                        GVolume *volume,
                                        VolumeManager *p_this);

    static void mount_added_callback(GVolumeMonitor *monitor,
                                     GMount *mount,
                                     VolumeManager *p_this);

    static void mount_removed_callback(GVolumeMonitor *monitor,
                                       GMount *mount,
                                       VolumeManager *p_this);

public Q_SLOTS:
    static void unmount(const QString &uri);

protected:
    static void unmount_cb(GFile *file,
                           GAsyncResult *result,
                           GError **error);

private:
    explicit VolumeManager(QObject *parent = nullptr);
    ~VolumeManager();

    GVolumeMonitor *m_volume_monitor = nullptr;
    gulong m_drive_connected_handle = 0;
    gulong m_drive_disconnected_handle = 0;
    gulong m_volume_added_handle = 0;
    gulong m_volume_removed_handle = 0;
    gulong m_mount_added_handle = 0;
    gulong m_mount_removed_handle = 0;
};

}

Q_DECLARE_METATYPE(Peony::Drive)
Q_DECLARE_METATYPE(std::shared_ptr<Peony::Drive>)
Q_DECLARE_METATYPE(Peony::Volume)
Q_DECLARE_METATYPE(std::shared_ptr<Peony::Volume>)
Q_DECLARE_METATYPE(Peony::Mount)
Q_DECLARE_METATYPE(std::shared_ptr<Peony::Mount>)

#endif // VOLUMEMANAGER_H
