#ifndef FILEDELETEOPERATION_H
#define FILEDELETEOPERATION_H

#include "file-operation.h"

#include "peony-core_global.h"

namespace Peony {

class FileNode;
class FileNodeReporter;

class PEONYCORESHARED_EXPORT FileDeleteOperation : public FileOperation
{
    Q_OBJECT
public:
    explicit FileDeleteOperation(QStringList sourceUris, QObject *parent = nullptr);
    ~FileDeleteOperation() override;

    void deleteRecursively(FileNode *node);
    void run() override;

    void cancel() override;

private:
    QStringList m_source_uris;

    int m_current_count = 0;
    int m_total_count = 0;
    QString m_current_src_uri = nullptr;

    goffset m_current_offset = 0;
    goffset m_total_szie = 0;

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

#endif // FILEDELETEOPERATION_H
