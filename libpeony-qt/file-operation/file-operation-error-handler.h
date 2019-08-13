#ifndef FILEOPERATIONERRORHANDLER_H
#define FILEOPERATIONERRORHANDLER_H

#include "file-operation.h"
#include "gerror-wrapper.h"
#include <QMetaType>

#include "peony-core_global.h"

#define ErrorHandlerIID "org.ukui.peony-qt.FileOperationErrorHandler"

namespace Peony {

class PEONYCORESHARED_EXPORT FileOperationErrorHandler {
public:
    virtual ~FileOperationErrorHandler();
    virtual QVariant handleError(const QString &srcUri,
                                 const QString &destDirUri,
                                 const GErrorWrapperPtr &err) = 0;
};

}

Q_DECLARE_INTERFACE(Peony::FileOperationErrorHandler, ErrorHandlerIID)

#endif // FILEOPERATIONERRORHANDLER_H
