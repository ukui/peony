#ifndef FILEOPERATIONERRORDIALOGREGULAR_H
#define FILEOPERATIONERRORDIALOGREGULAR_H

#include <QDialog>
#include "file-operation-error-handler.h"

namespace Peony {
class PEONYCORESHARED_EXPORT FileOperationErrorDialogRegular : public QDialog, public FileOperationErrorHandler
{
    Q_OBJECT
    Q_INTERFACES(Peony::FileOperationErrorHandler)
public:
    FileOperationErrorDialogRegular(QString& title, QString &srcUri, QString &destDirUri, QString& content, QWidget *parent = nullptr);
    ~FileOperationErrorDialogRegular() override;

    virtual bool handle (FileOperationError&) override;
    virtual int handleError(const QString &srcUri,
                                 const QString &destDirUri,
                                 const GErrorWrapperPtr &err,
                                 bool isCritical = false) override;

protected:
    QString m_title;
    QString m_src_uri;
    QString m_dest_uri;
    QString m_content;
};
};

#endif // FILEOPERATIONERRORDIALOGREGULAR_H
