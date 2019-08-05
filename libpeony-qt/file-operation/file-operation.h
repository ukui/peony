#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <QObject>
#include <QRunnable>

#include "gerror-wrapper.h"
#include "gobject-template.h"

namespace Peony {

/*!
 * \brief The FileOperation class
 * <br>
 * This class is an interface for several kinds of file operatrion,
 * like move, copy or delete, etc.
 * </br>
 * \note You should not use this class and derived classes in main thread.
 * Insteadly, using QThreadPool::start() is the best choice.
 */
class FileOperation : public QObject, public QRunnable
{
    Q_OBJECT

public:
    enum ResponseType {
        IgnoreOne,
        IgnoreAll,
        OverWriteOne,
        OverWriteAll,
        BackupOne,
        BackupAll,
        Retry,
        Cancel
    };

    explicit FileOperation(QObject *parent = nullptr);
    ~FileOperation();
    virtual void run() = 0;

    bool isCancelled() {return m_is_cancelled;}

Q_SIGNALS:
    /*!
     * \brief errored
     * <br>
     * This signal should be sent when a derived class instance went to an gio error.
     * The return value is needed by the instance for the error handling.
     * </br>
     * \param err, the shared_ptr wrapper of GError.
     * \return \retval response type for error handling.
     * \note Qt's signal/slot provide a blocking flag to ensure get return value of signal.
     * If you want to get response value rightly, you must connect this signal with
     * Qt::BlockingQueuedConnection flag set. That also limit you use fileoperation and its
     * derived class in main thread.
     */
    ResponseType errored(const GErrorWrapperPtr &err);

public Q_SLOTS:
    void cancel();

protected:
    GCancellableWrapperPtr getCancellable(){return m_cancellable_wrapper;}

private:
    GCancellableWrapperPtr m_cancellable_wrapper = nullptr;
    bool m_is_cancelled = false;
};

}

#endif // FILEOPERATION_H
