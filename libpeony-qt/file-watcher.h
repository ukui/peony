#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>

#include "peony-core_global.h"

#include <gio/gio.h>

namespace Peony {

/*!
 * \brief The FileWatcher class
 * <br>
 * FileWatcher class is a wrapper of a set of GFileMonitor handles.
 * The most obvious difference between it and the ordinary GFileMonitor is that
 * it can dynamically track the monitoring directory. For example,
 * if your directory move to another path, the watcher will aslo change
 * its monitors. If you delete the path (or trash), it will be deleted
 * automaticly later.
 * </br>
 */
class PEONYCORESHARED_EXPORT FileWatcher : public QObject
{
    Q_OBJECT
public:
    explicit FileWatcher(QString uri = nullptr, QObject *parent = nullptr);
    ~FileWatcher();
    /*!
     * \brief setMonitorChildrenChange
     * \param monitor_children_change
     * \details
     * For most case, we do not care wether a file internal changed event
     * in file manager (for example, the file's content changed).
     * Some special files, like files in computer:///, could use this signal
     * to monitor the changed event that not created or deleted
     * (for volume file handle in computer:///, it might be mount/unmount).
     */
    void setMonitorChildrenChange(bool monitor_children_change = true) {m_montor_children_change = monitor_children_change;}
    void startMonitor();
    void stopMonitor();

    /*!
     * \brief supportMonitor
     * \return
     * \details
     * Some file or directory doesn't support g_file_monitor* methods.
     * We can use this to ensure that if it was truely in monitoring.
     * If not, we might take over the handle of file change in our own
     * code.
     */
    bool supportMonitor() {return m_supprot_monitor;}

Q_SIGNALS:
    void locationChanged(const QString &oldUri, const QString &newUri);
    void directoryDeleted(const QString &uri);
    void fileCreated(const QString &uri);
    void fileDeleted(const QString &uri);
    void fileChanged(const QString &uri);

public Q_SLOTS:
    void cancel();

protected:
    void prepare();

    static void file_changed_callback(GFileMonitor *monitor,
                                      GFile *file,
                                      GFile *other_file,
                                      GFileMonitorEvent event_type,
                                      FileWatcher *p_this);

    static void dir_changed_callback(GFileMonitor *monitor,
                                     GFile *file,
                                     GFile *other_file,
                                     GFileMonitorEvent event_type,
                                     FileWatcher *p_this);

    void changeMonitorUri(QString uri);

private:
    QString m_uri = nullptr;
    QString m_target_uri = nullptr;
    GFile *m_file = nullptr;
    GFileMonitor *m_monitor = nullptr;
    GFileMonitor *m_dir_monitor = nullptr;

    bool m_montor_children_change = false;

    GCancellable *m_cancellable = nullptr;

    /*!
     * \brief m_monitor_err
     * \deprecated use GErrorWrapper
     */
    GError *m_monitor_err = nullptr;
    /*!
     * \brief m_dir_monitor_err
     * \deprecated use GErrorWrapper
     */
    GError *m_dir_monitor_err = nullptr;

    gulong m_file_handle = 0;
    gulong m_dir_handle = 0;

    bool m_supprot_monitor = true;
};

}

#endif // FILEWATCHER_H
