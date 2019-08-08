#ifndef FILEMOVEOPERATION_H
#define FILEMOVEOPERATION_H

#include "file-operation.h"
#include "file-info.h"

namespace Peony {

class FileNodeReporter;
class FileNode;

class FileMoveOperation : public FileOperation
{
    Q_OBJECT
public:
    /*!
     * \brief FileMoveOperation
     * \param sourceUris, list of source files' uris.
     * \param destUri, dest folder uri.
     * \param parent
     * <br>
     * This class is derived from FileOperation class, implement the virtual
     * run() function for moving files to a target directory.
     * </br>
     * \note
     * Unlike the g_file_move function, the target file of this operation must
     * be a directory.
     */
    explicit FileMoveOperation(QStringList sourceUris, QString destDirUri, QObject *parent = nullptr);

    ~FileMoveOperation();

    /*!
     * \brief setForceUseFallback
     * \param useFallback
     * \value true, force using copy and delete replace native move function provided by file system.
     * \value false, try using native move function first.
     * <br>
     * The reason why we have a optional of force-using fallback is that we might want to get
     * more details of a move, which is the native one doesn't support.
     * If we force using fallback function, we will trying to get infos of files that waiting for
     * moving. So that we can count the total size, and dynamicly get the progress of operation.
     * </br>
     */
    void setForceUseFallback(bool useFallback = true) {m_force_use_fallback = useFallback;}

    void run() override;

Q_SIGNALS:
    void addOne(const QString &srcUri, const qint64 &size);
    void fileMoved(const QString &srcUri, const qint64 &size);
    void srcFileDeleted(const QString &uri);
    void nativeMoveProgressCallbacked(const QString &srcUri, const QString &destDirUri,
                                      const int &current, const int &total);
    void fallbackMoveProgressCallbacked(const QString &srcUri, const QString &destDirUri,
                                        const qint64 &current_bytes, const qint64 &total_bytes);

public Q_SLOTS:
    void cancel() override;

protected:
    static void progress_callback(goffset current_num_bytes,
                                  goffset total_num_bytes,
                                  FileMoveOperation *p_this);

    void copyRecursively(FileNode *node);
    void deleteRecursively(FileNode *node);

    void move();
    void moveForceUseFallback();

    /*!
     * \brief prehandle
     * \param err
     * \return
     * \retval true if error handled.
     * \retval false if error not handled.
     * \details
     * There's something complicate handling the error in file operation.
     * User might handle the same kind of errors unified.
     * But different errors actually need different way to solve.
     * this function will help us do some thing prehandling if user
     * choose IgnoreAll, BackupAll or OverWriteAll option when a
     * specific type error occured.
     */
    ResponseType prehandle(GError *err);

private:
    QStringList m_source_uris;
    QString m_dest_dir_uri = nullptr;

    /*!
     * \brief m_current_count, used in progress_callback
     */
    int m_current_count = 0;
    /*!
     * \brief m_total_count, used in progress_callback
     */
    int m_total_count = 0;
    /*!
     * \brief m_current_src_uri, used in progress_callback.
     */
    QString m_current_src_uri = nullptr;
    /*!
     * \brief m_current_dest_dir_uri, used in progress_callback.
     */
    QString m_current_dest_dir_uri = nullptr;

    goffset m_current_offset = 0;
    goffset m_total_szie = 0;

    /*!
     * \brief m_force_use_callback
     * \value true, the move operation will use copy + delete fallback anyway.
     * \value false, the move operation will priority using native move operations
     * if supported. The native implementation may support moving directories
     * (for instance on moves inside the same filesystem).
     */
    bool m_force_use_fallback = false;

    /*!
     * \brief m_ignore_all_errors
     * \deprecated
     */
    bool m_ignore_all_errors = false;
    /*!
     * \brief m_overwrite_all_duplicated
     * \deprecated
     */
    bool m_overwrite_all_duplicated = false;
    /*!
     * \brief m_backup_all_duplicated
     * \deprecated
     */
    bool m_backup_all_duplicated = false;

    GFileCopyFlags m_default_copy_flag = GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS|
                                                        G_FILE_COPY_ALL_METADATA);

    FileNodeReporter *m_reporter = nullptr;

    /*!
     * \brief m_prehandle_hash
     * \details
     * Once a move operation get into error, this class might cache the specific response
     * for next prehandleing.
     */
    QHash<int, ResponseType> m_prehandle_hash;
};

}

#endif // FILEMOVEOPERATION_H
