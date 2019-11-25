#ifndef CREATETEMPLATEOPERATION_H
#define CREATETEMPLATEOPERATION_H

#include <QObject>
#include "peony-core_global.h"

#include "file-operation.h"

namespace Peony {

class CreateTemplateOperation : public FileOperation
{
    Q_OBJECT
public:
    enum Type {
        EmptyFile,
        EmptyFolder,
        Template
    };

    explicit CreateTemplateOperation(const QString &destDirUri, Type type = EmptyFile, const QString &templateName = nullptr, QObject *parent = nullptr);
    void run() override;
    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return m_info;}

    const QString target() {return m_target_uri;}

protected:
    void handleDuplicate(const QString &uri);

private:
    std::shared_ptr<FileOperationInfo> m_info;

    QString m_src_uri;
    QString m_dest_dir_uri;
    QString m_target_uri;
    Type m_type;
};

}

#endif // CREATETEMPLATEOPERATION_H
