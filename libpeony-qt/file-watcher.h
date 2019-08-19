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
    void startMonitor();
    void stopMonitor();

Q_SIGNALS:
    void locationChanged(const QString &oldUri, const QString &newUri);
    void directoryDeleted(const QString &uri);
    void fileCreated(const QString &uri);
    void fileDeleted(const QString &uri);

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
};

}

#endif // FILEWATCHER_H
