#ifndef FILECOPYOPERATION_H
#define FILECOPYOPERATION_H

#include "peony-core_global.h"

#include "file-operation.h"

namespace Peony {

class FileNodeReporter;
class FileNode;

/*!
 * \brief The FileCopyOperation class
 * \todo
 * implment duplicated copy. this should be consumed as the backup handler.
 */
class PEONYCORESHARED_EXPORT FileCopyOperation : public FileOperation
{
    Q_OBJECT
public:
    explicit FileCopyOperation(QStringList sourceUris, QString destDirUri, QObject *parent = nullptr);
    ~FileCopyOperation() override;

    void run() override;
    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return m_info;}

public Q_SLOTS:
    void cancel() override;

protected:
    ResponseType prehandle(GError *err);
    static void progress_callback(goffset current_num_bytes,
                                  goffset total_num_bytes,
                                  FileCopyOperation *p_this);
    /*!
     * \brief copyRecursively
     * \param node
     * \see FileMoveOperation::copyRecursively()
     */
    void copyRecursively(FileNode *node);
    /*!
     * \brief rollbackNodeRecursively
     * \param node
     * \details
     * This function is similar to FileMoveOperation::rollbackNodeRecursively(),
     * but it is more simple. The copy operation's rollbacking logic is easier to
     * understand.
     */
    void rollbackNodeRecursively(FileNode *node);

private:
    /*!
     * \brief m_is_duplicated_copy
     * \details
     * In peony-qt, file copy operation has 2 types.
     * 1. The file is duplicated in same folder.
     * 2. Other.
     * If case 1, the peony-qt will trying to copy a file with special suffix automaticly.
     * For example, a file abc.xyz will be copied and renamed to abc(1).xyz, abc(2).xyz...
     * If case 2, there will be nothing special action in operation except it went into error.
     * \deprecated
     * use FileDuplicateOperation instead.
     */
    bool m_is_duplicated_copy = false;

    QStringList m_source_uris;
    QString m_dest_dir_uri = nullptr;

    int m_current_count = 0;
    int m_total_count = 0;
    QString m_current_src_uri = nullptr;
    QString m_current_dest_dir_uri = nullptr;

    goffset m_current_offset = 0;
    goffset m_total_szie = 0;

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

    std::shared_ptr<FileOperationInfo> m_info = nullptr;
};

}

#endif // FILECOPYOPERATION_H
