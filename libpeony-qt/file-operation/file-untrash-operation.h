#ifndef FILEUNTRASHOPERATION_H
#define FILEUNTRASHOPERATION_H

#include "peony-core_global.h"
#include "file-operation.h"

namespace Peony {

class FileUntrashOperation : public FileOperation
{
public:
    explicit FileUntrashOperation(QStringList uris, QObject *parent = nullptr);

    void run() override;
    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return m_info;}

private:
    GFileCopyFlags m_default_copy_flag = GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS|
                                                        G_FILE_COPY_ALL_METADATA);

    QStringList m_uris;
    QHash<QString, QString> m_restore_hash;
    std::shared_ptr<FileOperationInfo> m_info = nullptr;
};

}

#endif // FILEUNTRASHOPERATION_H
