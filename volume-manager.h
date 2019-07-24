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
    Drive(GDrive *drive) {m_drive = drive;}
    ~Drive() {g_object_unref(m_drive);}

    QString name()
    {
        char *name = g_drive_get_name(m_drive);
        QString value = name;
        g_free(name);
        return value;
    }
    QString iconName()
    {
        GThemedIcon *g_icon = G_THEMED_ICON(g_drive_get_icon(m_drive));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }
    QString symbolicIconName()
    {
        GThemedIcon *g_icon = G_THEMED_ICON(g_drive_get_symbolic_icon(m_drive));
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        g_object_unref(g_icon);
        return *icon_names;
    }

    bool removable()
    {
        return g_drive_is_removable(m_drive);
    }

private:
    GDrive *m_drive = nullptr;
};

class PEONYCORESHARED_EXPORT VolumeManager : public QObject
{
    Q_OBJECT
public:
    static VolumeManager *getInstance();

Q_SIGNALS:
    //void driveConnected(GDrive *drive);
    //void driveDisconnected(GDrive *drive);
    void volumeAdded(GVolume *volume);
    void volumeRemoved(GVolume *volume);
    void mountAdded(GMount *mount);
    void mountRemoved(GMount *mount);

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

#endif // VOLUMEMANAGER_H
