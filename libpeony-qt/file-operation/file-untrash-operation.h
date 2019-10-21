#ifndef FILEUNTRASHOPERATION_H
#define FILEUNTRASHOPERATION_H

#include "peony-core_global.h"
#include "file-operation.h"

namespace Peony {

/*!
 * \brief The FileUntrashOperation class
 * \bug
 * can not restore the files which's parents has chinese.
 */
class PEONYCORESHARED_EXPORT FileUntrashOperation : public FileOperation
{
    Q_OBJECT
public:
    explicit FileUntrashOperation(QStringList uris, QObject *parent = nullptr);

    void run() override;
    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return m_info;}

protected:
    void cacheOriginalUri();

private:
    GFileCopyFlags m_default_copy_flag = GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS|
                                                        G_FILE_COPY_ALL_METADATA);

    QStringList m_uris;
    QHash<QString, QString> m_restore_hash;
    std::shared_ptr<FileOperationInfo> m_info = nullptr;
};

}

#endif // FILEUNTRASHOPERATION_H
