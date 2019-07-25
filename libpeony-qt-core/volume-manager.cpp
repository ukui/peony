#include "volume-manager.h"

#include <QDebug>

using namespace Peony;

static VolumeManager *m_global_manager = nullptr;

VolumeManager::VolumeManager(QObject *parent) : QObject(parent)
{
    m_volume_monitor = g_volume_monitor_get();

    m_drive_connected_handle = g_signal_connect(m_volume_monitor,
                                                "drive-connected",
                                                G_CALLBACK(drive_connected_callback),
                                                this);

    m_drive_disconnected_handle = g_signal_connect(m_volume_monitor,
                                                   "drive-disconnected",
                                                   G_CALLBACK(drive_disconnected_callback),
                                                   this);

    m_volume_added_handle = g_signal_connect(m_volume_monitor,
                                             "volume-added",
                                             G_CALLBACK(volume_added_callback),
                                             this);

    m_volume_removed_handle = g_signal_connect(m_volume_monitor,
                                               "volume-removed",
                                               G_CALLBACK(volume_removed_callback),
                                               this);

    m_mount_added_handle = g_signal_connect(m_volume_monitor,
                                            "mount-added",
                                            G_CALLBACK(mount_added_callback),
                                            this);

    m_mount_removed_handle = g_signal_connect(m_volume_monitor,
                                              "mount-removed",
                                              G_CALLBACK(mount_removed_callback),
                                              this);
}

VolumeManager::~VolumeManager()
{
    disconnect();

    g_signal_handler_disconnect(m_volume_monitor, m_drive_connected_handle);
    g_signal_handler_disconnect(m_volume_monitor, m_drive_disconnected_handle);
    g_signal_handler_disconnect(m_volume_monitor, m_volume_added_handle);
    g_signal_handler_disconnect(m_volume_monitor, m_volume_removed_handle);
    g_signal_handler_disconnect(m_volume_monitor, m_mount_added_handle);
    g_signal_handler_disconnect(m_volume_monitor, m_mount_removed_handle);

    g_object_unref(m_volume_monitor);
}

VolumeManager *VolumeManager::getInstance()
{
    if (!m_global_manager) {
        m_global_manager = new VolumeManager;
    }
    return m_global_manager;
}

void VolumeManager::drive_connected_callback(GVolumeMonitor *monitor,
                                             GDrive *drive,
                                             VolumeManager *p_this)
{
    Q_UNUSED(monitor);
    auto data = std::make_shared<Drive>(drive);
    Q_EMIT p_this->driveConnected(data);
}

void VolumeManager::drive_disconnected_callback(GVolumeMonitor *monitor,
                                                GDrive *drive,
                                                VolumeManager *p_this)
{
    Q_UNUSED(monitor);
    Q_EMIT p_this->driveDisconnected(std::make_shared<Drive>(drive));
}

void VolumeManager::volume_added_callback(GVolumeMonitor *monitor,
                                          GVolume *volume,
                                          VolumeManager *p_this)
{
    Q_UNUSED(monitor);
    Q_EMIT p_this->volumeAdded(std::make_shared<Volume>(volume));
}

void VolumeManager::volume_removed_callback(GVolumeMonitor *monitor,
                                            GVolume *volume,
                                            VolumeManager *p_this)
{
    Q_UNUSED(monitor);
    Q_EMIT p_this->volumeRemoved(std::make_shared<Volume>(volume));
}

void VolumeManager::mount_added_callback(GVolumeMonitor *monitor,
                                         GMount *mount,
                                         VolumeManager *p_this)
{
    Q_UNUSED(monitor);
    Q_EMIT p_this->mountAdded(std::make_shared<Mount>(mount));
}

void VolumeManager::mount_removed_callback(GVolumeMonitor *monitor,
                                           GMount *mount,
                                           VolumeManager *p_this)
{
    Q_UNUSED(monitor);
    Q_EMIT p_this->mountRemoved(std::make_shared<Mount>(mount));
}
