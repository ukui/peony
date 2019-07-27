#ifndef FileInfoJob_H
#define FileInfoJob_H

#include "peony-core_global.h"

#include <QObject>

#include <memory>
#include <gio/gio.h>

namespace Peony {

class FileInfo;

/*!
 * \brief The FileInfoJob class
 * <br>
 * FileInfoJob provide both sync and async method querying of an FileInfo
 * instance. Both of them will send the info's FileInfo::updated() signal.
 * For query async, it aslo send FileInfoJob::queryAsyncFinished() signal
 * when job finished.
 * </br>
 * \note
 * Operations that querying a file's information might be very frequent,
 * so I recommend querying in asynchronous and cancellable ways.
 * Using queryAsync() will cancel the all existing querying job about this shared info.
 * Alternatively, the share info data will be refresh when last async job finished.
 * This desgin is similar to peony(caja).
 */
class PEONYCORESHARED_EXPORT FileInfoJob : public QObject
{
    friend class FileInfo;

    Q_OBJECT
public:
    explicit FileInfoJob(std::shared_ptr<FileInfo> info, QObject *parent = nullptr);
    bool querySync();

    void setAutoDelete(bool deleteWhenJobFinished = true) {m_auto_delete = deleteWhenJobFinished;}

Q_SIGNALS:
    /*!
     * \brief queryAsyncFinished
     * \param successed
     * \retval true if no error happend in queryAsync() and callback
     * \retval false if error happend, it might be cancelled, or others.
     * \note If you just want to get the info states, I recommend you connect to
     * Peony::FileInfo::updated signal, every query job of a info will send this signal
     * if query successfully.
     * \see Peony::FileInfo::updated(), Peony::FileInfoJob::refreshInfoContents().
     */
    void queryAsyncFinished(bool successed);

public Q_SLOTS:
    void queryAsync();
    /*!
     * \brief cancel
     * <br>
     * cancel is an useful method to control the content refresh frequency.
     * there may be many query job for one same info at In a very short period of time
     * from different info holders.
     * </br>
     */
    void cancel();

protected:
    static GAsyncReadyCallback query_info_async_callback(GFile *file,
                                                         GAsyncResult *res,
                                                         FileInfoJob *thisJob);

private:
    ~FileInfoJob();
    void refreshInfoContents(GFileInfo *new_info);
    std::shared_ptr<FileInfo> m_info;
    GCancellable *m_cancellble = nullptr;
    bool m_auto_delete = false;
};

}

#endif // FileInfoJob_H
