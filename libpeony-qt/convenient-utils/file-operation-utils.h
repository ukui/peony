#ifndef FILEOPERATIONUTILS_H
#define FILEOPERATIONUTILS_H

#include <QStringList>
#include <memory>

namespace Peony {

class FileInfo;

/*!
 * \brief The FileOperationUtils class
 * This is a class only provide static method for file operation.
 */
class FileOperationUtils
{
public:
    static void move(const QStringList &srcUris, const QString &destUri, bool addHistory);
    static void copy(const QStringList &srcUris, const QString &destUri, bool addHistory);
    static void trash(const QStringList &uris, bool addHistory);
    static void remove(const QStringList &uris);
    static void rename(const QString &uri, const QString &newName, bool addHistory);
    static void link(const QString &srcUri, const QString &destUri, bool addHistory);

    /*!
     * \brief queryFileInfo
     * \param uri
     * \return
     * \retval the file's latest info at current time.
     * \details
     * I provide a method for force querying a file's info.
     * \note
     * This info might be hold by other class instance yet,
     * that means other object share this info can recive the info's
     * updated signal when the file's info is updated.
     */
    static std::shared_ptr<FileInfo> queryFileInfo(const QString &uri);
private:
    FileOperationUtils();
};

}

#endif // FILEOPERATIONUTILS_H
