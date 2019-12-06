#ifndef FILELINKOPERATION_H
#define FILELINKOPERATION_H

#include "peony-core_global.h"
#include "file-operation.h"

namespace Peony {

class PEONYCORESHARED_EXPORT FileLinkOperation : public FileOperation
{
    Q_OBJECT
public:
    FileLinkOperation(QString srcUri, QString destDirUri, QObject *parent = nullptr);
    ~FileLinkOperation() override;

    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return m_info;}
    void run() override;

private:
    QString m_src_uri = nullptr;
    QString m_dest_uri = nullptr;

    std::shared_ptr<FileOperationInfo> m_info = nullptr;
};

}

#endif // FILELINKOPERATION_H
