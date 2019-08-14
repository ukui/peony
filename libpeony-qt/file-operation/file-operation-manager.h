#ifndef FILEOPERATIONMANAGER_H
#define FILEOPERATIONMANAGER_H

#include <QObject>

#include "peony-core_global.h"
#include "gobject-template.h"
#include "file-utils.h"
#include <QMutex>

namespace Peony {

class FileOperation;

class FileOperationInfo;

/*!
 * \brief The FileOperationManager class
 * \details
 * In peony, the undo/redo stack manager is bind with a directory view.
 * And in peony-qt, it is similar to peony. But there are higher level
 * api to manage these 'managers' in peony-qt.
 * Not only the undo/redo stacks' management. FileOperationManager
 */
class PEONYCORESHARED_EXPORT FileOperationManager : public QObject
{
    Q_OBJECT
public:
    FileOperationManager *getInstance();
    void close();

Q_SIGNALS:
    void closed();

public Q_SLOTS:
    /*
    void startOperation(FileOperation *operation);
    void undo();
    void redo();
    void goToState(FileOperationInfo *operationInfo);
    void clearHistory();
    void onFilesDeleted(const QStringList &uris);
    */

private:
    explicit FileOperationManager(QObject *parent = nullptr);
    ~FileOperationManager();
};

class FileOperationInfo : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Invalid,
        Move,//move back if no error in original moving
        Copy,//delete if no error in original copying
        Link,//delete...
        Rename,//rename
        Trash,//untrash
        Untrash,//trash
        Delete,//nothing to do
        CreateTxt,//delete
        CreateFolder,//delete
        CreateTemplate,//delete
        Other//nothing to do
    };

    explicit FileOperationInfo(QStringList srcUris, QString destDirUri, Type type, QObject *parent = nullptr): QObject(parent) {
        m_src_uris = srcUris;
        m_dest_dir_uri = destDirUri;
        m_type = type;

        //compute opposite.
        if (type != Rename) {
            for (auto srcUri : srcUris) {
                auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
                if (m_src_dir_uri.isNull()) {
                    auto srcParent = FileUtils::getFileParent(srcFile);
                    m_src_dir_uri = FileUtils::getFileUri(srcParent);
                }
                QString relativePath = FileUtils::getFileBaseName(srcFile);
                auto destDirFile = wrapGFile(g_file_new_for_uri(destDirUri.toUtf8().constData()));
                auto destFile = FileUtils::resolveRelativePath(destDirFile, relativePath);
                QString destUri = FileUtils::getFileUri(destFile);
                m_dest_uris<<destUri;
            }
        } else {
            //Rename also use the common args format.
            QString src = srcUris.at(0);
            QString dest = destDirUri;
            m_dest_uris<<src;
            m_src_dir_uri = dest;
        }

        switch (type) {
        case Trash: {
            m_opposite_type = Untrash;
            break;
        }
        case Untrash: {
            m_opposite_type = Trash;
            break;
        }
        case Delete: {
            m_opposite_type = Other;
            break;
        }
        case Copy: {
            m_opposite_type = Delete;
            break;
        }
        case Link: {
            m_opposite_type = Delete;
            break;
        }
        default: {
            m_opposite_type = type;
        }
        }
    }

    FileOperationInfo *getOppositeInfo(FileOperationInfo *info) {
        return new FileOperationInfo(info->m_dest_uris, info->m_src_dir_uri, m_opposite_type);
    }

    /*!
     * \brief disable
     * \details
     * If a file was deleted, the history about this file should be disabled.
     * The disabled operation info will be removed when the new file operation comming.
     */
    void disable() {m_enable = false;}
    bool getEnable() {return m_enable;}

private:
    QStringList m_src_uris;
    QString m_dest_dir_uri;

    //FIXME: if files in different src dir, how to deal with it?
    QStringList m_dest_uris;
    QString m_src_dir_uri;
    QMutex m_mutex;

    Type m_type;
    Type m_opposite_type;

    bool m_enable = true;
};

}

#endif // FILEOPERATIONMANAGER_H
