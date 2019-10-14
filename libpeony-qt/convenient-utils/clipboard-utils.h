#ifndef CLIPBOARDUTILS_H
#define CLIPBOARDUTILS_H

#include <QObject>
#include "peony-core_global.h"

namespace Peony {

/*!
 * \brief The ClipboardUtils class, a convinet class to access with QClipboard instance.
 * \details
 * This class provide some nessersary method which other libpeony-qt's class need.
 * For example, isClipboardFilesBeCut is used in Peony::DirectoryView::IconView.
 * IconViewDelegate paint the cut files with different opacity. The paint event
 * is triggered by clipboardChanged() signal.
 */
class ClipboardUtils : public QObject
{
    Q_OBJECT
public:
    ClipboardUtils *getInstance();
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
    static QStringList getClipboardFilesUris();
    static void pasteClipboardFiles(const QString &targetDirUri);
    static void clearClipboard();
    static const QString getClipedFilesParentUri();

Q_SIGNALS:
    void clipboardChanged();

private:
    explicit ClipboardUtils(QObject *parent = nullptr);
    ~ClipboardUtils();
};

}

#endif // CLIPBOARDUTILS_H
