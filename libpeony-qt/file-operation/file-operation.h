#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <QObject>
#include <QRunnable>

#include "gerror-wrapper.h"
#include "gobject-template.h"

#include "file-enumerator.h"
#include "file-info.h"

#include <QMetaType>
#include <QHash>

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
        Cancel,
        Other
    };

    explicit FileOperation(QObject *parent = nullptr);
    ~FileOperation();
    virtual void run() = 0;

    bool isCancelled() {return m_is_cancelled;}

Q_SIGNALS:
    /*!
     * \brief operationStarted
     * <br>
     * This signal should send when operation started.
     * when a derived class implement the run() method, it aslo need send this signal
     * to tell other object that the operation has started. it might use block-queue connect
     * for other object prepared.
     * </br>
     */
    void operationStarted();
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
    QVariant errored(const QString &srcUri, const QString &destUri, const Peony::GErrorWrapperPtr &err);

    /*!
     * \brief operationFinished
     * <br>
     * This signal is used to tell other object that the file operation has finished.
     * A progress dialog can connect this signal and close itself when the signal triggered.
     * </br>
     */
    void operationFinished();

public Q_SLOTS:
    void cancel();

protected:
    GCancellableWrapperPtr getCancellable(){return m_cancellable_wrapper;}

private:
    GCancellableWrapperPtr m_cancellable_wrapper = nullptr;
    bool m_is_cancelled = false;
};

}

Q_DECLARE_METATYPE(Peony::FileOperation::ResponseType)

#endif // FILEOPERATION_H
