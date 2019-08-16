#ifndef FILETRASHOPERATION_H
#define FILETRASHOPERATION_H

#include "peony-core_global.h"
#include "file-operation.h"

namespace Peony {

class PEONYCORESHARED_EXPORT FileTrashOperation : public FileOperation
{
    Q_OBJECT
public:
    explicit FileTrashOperation(QStringList srcUris, QObject *parent = nullptr);

    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return m_info;}
    void run() override;

private:
    QStringList m_src_uris;
    std::shared_ptr<FileOperationInfo> m_info = nullptr;
};

}

#endif // FILETRASHOPERATION_H
