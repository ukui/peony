/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#ifndef FILEOPERATIONERRORHANDLER_H
#define FILEOPERATIONERRORHANDLER_H

#include <QMetaType>
#include "file-operation.h"
#include "gerror-wrapper.h"
#include "peony-core_global.h"

#define ErrorHandlerIID "org.ukui.peony-qt.FileOperationErrorHandler"

namespace Peony {

enum ErrorType
{
    ET_GIO,
    ET_CUSTOM,
};

typedef struct _FileOperationError
{
    int                         errorCode;
    ErrorType                   errorType;
    QString                     title;
    QString                     srcUri;
    QString                     destDirUri;
    bool                        isCritical;

    QMap<QString, QVariant>     respStr;
} FileOperationError;

class PEONYCORESHARED_EXPORT FileOperationErrorHandler {
public:
    virtual ~FileOperationErrorHandler() = 0;

    virtual bool handle () = 0;

    virtual int handleError(const QString &srcUri,
                                 const QString &destDirUri,
                                 const GErrorWrapperPtr &err,
                                 bool isCritical = false) = 0;
};
}

Q_DECLARE_INTERFACE(Peony::FileOperationErrorHandler, ErrorHandlerIID)

#endif // FILEOPERATIONERRORHANDLER_H
