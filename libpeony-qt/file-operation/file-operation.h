#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <QObject>
#include <QRunnable>

#include "gerror-wrapper.h"
#include "gobject-template.h"

#include <QMetaType>
#include <QHash>

#include "peony-core_global.h"

namespace Peony {

class FileOperationInfo;
/*!
 * \brief The FileOperation class
 * <br>
 * This class is an interface for several kinds of file operatrion,
 * like move, copy or delete, etc.
 * </br>
 * \details
 * FileOperation specifies a set of signals as an interactive interface.
 * There a mainly two kinds of interface, operation wizard and error handler.
 * Operation Wizard is used to show the operation progess and cancel the operation
 * in ui. Error Handler is used to handle the error when the operation went error.
 * \note
 * You should not use this class and derived classes in main thread.
 * Insteadly, using QThreadPool::start() is the best choice.
 * \see
 * FileOperationProgressWizard, FileOperationErrorHandler, FileOperationErrorDialog.
 *
 * \todo
 * add disk i/o speed computing function, this should be implement by implement
 * gio's progress callback.
 */
class PEONYCORESHARED_EXPORT FileOperation : public QObject, public QRunnable
{
    Q_OBJECT

public:
    enum ResponseType {
        Invalid,
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
    virtual void run();

    void setHasError(bool hasError = true) {m_has_error = hasError;}
    bool hasError() {return m_has_error;}

    /*!
     * \brief getOperationInfo
     * \return
     * \details
     * This is a virtual function, some derived operation class should override
     * this function.
     * The FileOperation instance will destroy itself when it finished, but its info might not.
     * The FileOperationInfo is a part of peony-qt's undo/redo stack(s). FileOperationManager
     * will manage the stack(s) made up of these info.
     */
    virtual std::shared_ptr<FileOperationInfo> getOperationInfo() {return nullptr;}

    /*!
     * \brief setShouldReversible
     * \param reversible
     * \details
     * If operation is reversible, it should support to be undo and redo.
     * \note
     * Even though a operation has been set should reversible, it doesn't mean
     * that it really reversible. For example, if a file was deleted, it can not
     * be undo, so it should not add into operation's history of undo/redo.
     * If you don't hope your operation keep records, just do not set it.
     */
    void setShouldReversible(bool reversible = true) {m_reversible = reversible;}
    virtual bool reversible() {return m_reversible;}

    bool isCancelled() {return m_is_cancelled;}

Q_SIGNALS:
    /*!
     * \brief invalidOperation
     * \param message
     * \details
     * Before a file operation start, peony-qt will do some simple checks.
     * If there is an obvious error, the operation will not be performed.
     * For example, copying/moving a file to the same folder, renaming a file
     * with existed name, etc.
     * The operation will send the invalidOperation() signal and skip the fileoperation.
     */
    void invalidOperation(const QString &message);
    void invalidExited(const QString &message);
    /*!
     * \brief operationStarted
     * <br>
     * This signal should be sent when operation started.
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
     * \param isCritical, the error is critical and operation should be interupted now.
     * \return \retval response type for error handling.
     * \note Qt's signal/slot provide a blocking flag to ensure get return value of signal.
     * If you want to get response value rightly, you must connect this signal with
     * Qt::BlockingQueuedConnection flag set. That also limit you use fileoperation and its
     * derived class in main thread.
     */
    QVariant errored(const QString &srcUri, const QString &destUri, const Peony::GErrorWrapperPtr &err, bool isCritical = false);

    void FileProgressCallback(const QString &srcUri, const QString &destUri,
                              const qint64 &current_file_offset, const qint64 &current_file_size);

    /*!
     * \brief operationRequestShowWizard
     * \details
     * Instead of connect with operationStarted() signal, using operationRequestShowWizard() for showing
     * a wizard is better. Because you might not want to see a wizard in some operation.
     * \note
     * only fallback-move, copy and delete send this signal.
     */
    void operationRequestShowWizard();
    /*!
     * \brief operationFallbackRetried
     * \details
     * In peony-qt, we perfer user using native move function for fast moving.
     * Unfortunatly there are many limitration for native move.
     * If a native move is failed for those reasons, the move operation will
     * retry with copy/delete function as fallback, it might cost much more time.
     */
    void operationFallbackRetried();
    /*!
     * \brief operationPreparedOne
     * \param srcUri
     * \param destUri
     * \param size
     * \details
     * This signal should be sent when the operation found a file node.
     * The signal reciver will count the received signals count as the total source files count.
     * The total size should also be accumulated.
     */
    void operationPreparedOne(const QString &srcUri, const qint64 &size);
    /*!
     * \brief operationPrepared
     * <br>
     * This signal should be sent when the operation ready to handle the files.
     * Before we really handle the files, we might need to do something preparing.
     * For example, a recursively enumerating. We should send this signal when these
     * works have been done.
     * </br>
     */
    void operationPrepared();
    /*!
     * \brief operationProgressedOne
     * \param srcUri
     * \param destUri
     * \param size
     * \details
     * This signal should be sent when the operation progressed one files.
     * The receiver could use operationPreparedOne() and operationProgressedOne()
     * to compute the current progress for most of operations.
     */
    void operationProgressedOne(const QString& srcUri, const QString &destUri, const qint64 &size);
    /*!
     * \brief operationProgressed
     * <br>
     * This signal should be sent when the operation is half-finished.
     * Some operation, such as move, might be splitted into 2 parts.
     * Copy and delete both spend a while.
     * If the other object doesn't care the next process of unfinished
     * operation, they can connect this signal instead of operationFinished()
     * signal to ignore the next progress, even thought the operation will
     * continue until it really completed.
     * </br>
     */
    void operationProgressed();
    /*!
     * \brief operationRollbackedOne
     * \param destUri
     * \param srcUri
     * \details
     * This signal should be sent when the operation had been cancelled,
     * and one file was rollbacked to the state before operation executed.
     * \note
     * The rollback progress is difficult to count.
     * This would mean the progress inaccuracy
     */
    /*!
     * \brief operationAfterProgressedOne
     * \param srcUri
     * \details
     * This signal is not necerssary by all operations. It is used by some
     * operations that cannot be implemented in one step.
     * For example, a non-native move operation will use recursive copy and delete method.
     * In this case the operationProgressedOne() and operationProgressed() signal would
     * only show the copy states. Use operationAfterProgressedOne() and operationAfterProgressed()
     * for tell others the progress of the move operation's clearing progress.
     */
    void operationAfterProgressedOne(const QString &srcUri);
    /*!
     * \brief operationAfterProgressed
     * \details
     * This signal is not necerssary by all operations.
     * If a multi-step operation finished the last operation, it should be sent.
     */
    void operationAfterProgressed();
    /*!
     * \brief operationRollbackedOne
     * \param destUri
     * \param srcUri
     * \details
     * In peony-qt, if a file operation was cancelled, it not just simply cancel the operation.
     * In order to maintain the atomicity of the operation, the cancelled operation will try rolling
     * back to previous state. This signal should be sent when a file which had been handled rollbacked.
     */
    void operationRollbackedOne(const QString &destUri, const QString &srcUri);
    /*!
     * \brief operationStartRollbacked
     * <br>
     * This signal is used to tell other object that
     * the operation has cancelled and rollbacked, not all operations
     * send should this signal.
     * </br>
     */
    void operationStartRollbacked();
    /*!
     * \brief operationFinished
     * <br>
     * This signal is used to tell other object that the file operation has finished.
     * Usually, a progress dialog can connect this signal and close itself when the signal triggered.
     * </br>
     */
    void operationFinished();

public Q_SLOTS:
    virtual void cancel();

protected:
    GCancellableWrapperPtr getCancellable(){return m_cancellable_wrapper;}

private:
    GCancellableWrapperPtr m_cancellable_wrapper = nullptr;
    bool m_is_cancelled = false;
    bool m_reversible = false;
    bool m_has_error = false;
};

}

Q_DECLARE_METATYPE(Peony::FileOperation::ResponseType)

#endif // FILEOPERATION_H
