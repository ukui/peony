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

#ifndef CLIPBOARDUTILS_H
#define CLIPBOARDUTILS_H

#include <QObject>
#include "peony-core_global.h"

namespace Peony {

class FileOperation;

/*!
 * \brief The ClipboardUtils class, a convinet class to access with QClipboard instance.
 * \details
 * This class provide some nessersary method which other libpeony-qt's class need.
 * For example, isClipboardFilesBeCut is used in Peony::DirectoryView::IconView.
 * IconViewDelegate paint the cut files with different opacity. The paint event
 * is triggered by clipboardChanged() signal.
 * \todo
 * Automatically detect the duplicated copy/paste and handle in backend.
 */
class PEONYCORESHARED_EXPORT ClipboardUtils : public QObject
{
    Q_OBJECT
public:
    static ClipboardUtils *getInstance();
    void release();

    /*!
     * \brief ClipboardUtils::setClipboardFiles
     * \param uris
     * \param isCut
     * \details
     * QClipboard doesn't have a concept of 'cut'. This concept would be used
     * in peony-qt for judge how we deal with the files in clipboard when we do
     * a 'paste' (copy/move).
     * \see ClipboardUtils::isClipboardFilesBeCut
     */
    static void setClipboardFiles(const QStringList &uris, bool isCut);
    static bool isClipboardHasFiles();
    /*!
     * \brief isClipboardFilesBeCut
     * \return
     * \retval true if you use setClipboardFiles and pass the isCut argument as 'true',
     * otherwise false.
     */
    static bool isClipboardFilesBeCut();
    static bool isDesktopFilesBeCut();
    static bool isPeonyFilesBeCut();
    static QStringList getClipboardFilesUris();
    static FileOperation *pasteClipboardFiles(const QString &targetDirUri);
    static void clearClipboard();
    static void popLastTargetDirectoryUri(QString& uri);
    static const QString getClipedFilesParentUri();

    const QString getLastTargetDirectoryUri();

Q_SIGNALS:
    void clipboardChanged();

private:
    explicit ClipboardUtils(QObject *parent = nullptr);
    ~ClipboardUtils();
};

}

#endif // CLIPBOARDUTILS_H
