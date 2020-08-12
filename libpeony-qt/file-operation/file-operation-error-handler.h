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

#define HANDLE_ERR_NEW 1

#define ErrorHandlerIID "org.ukui.peony-qt.FileOperationErrorHandler"

namespace Peony {

enum ExceptionType
{
    ET_GIO,
    ET_CUSTOM,
};

/*!
 * \brief Type of error handling
 * \li ED_CONFLICT: General conflict handling for file operations
 */
enum ExceptionDialogType {
    ED_CONFLICT,
};

/*!
 * \brief All possible results of error handling
 */
enum ExceptionResponse {
    Other,
    Retry,          // ok
    Cancel,
    Rename,
    Invalid,
    IgnoreOne,      // ok
    IgnoreAll,      // ok
    BackupOne,
    BackupAll,
    OverWriteOne,   // ok
    OverWriteAll,   // ok
};

/*!
 * \brief The format of the data that needs to be transferred for error handling operations
 * \li errorCode: Error code
 * \li isCritical: is critical
 * \li title: The title that appears in the error handling window, indicating what operation went wrong
 * \li srcUri: The file/folder being operated on
 * \li destDirUri: The target folder
 * \li errorType: Error types, gio errors and custom errors
 * \li respCode: The action selected by the user
 * \li respValue: Data entered by the user
 */
typedef struct _FileOperationError
{
    int                         errorCode;
    bool                        isCritical;
    QString                     title;
    QString                     srcUri;
    QString                     destDirUri;
    ExceptionType               errorType;
    ExceptionDialogType         dlgType;

    ExceptionResponse           respCode;
    QMap<QString, QVariant>     respValue;
} FileOperationError;

class PEONYCORESHARED_EXPORT FileOperationErrorHandler {
public:
    virtual ~FileOperationErrorHandler() = 0;

#if HANDLE_ERR_NEW
    virtual void handle (FileOperationError& error) = 0;
#else
    virtual int handleError(const QString &srcUri,
                                 const QString &destDirUri,
                                 const GErrorWrapperPtr &err,
                                 bool isCritical = false) = 0;
#endif
};
}

Q_DECLARE_INTERFACE(Peony::FileOperationErrorHandler, ErrorHandlerIID)

#endif // FILEOPERATIONERRORHANDLER_H
