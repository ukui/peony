#ifndef FILENODE_H
#define FILENODE_H

#include <QString>
#include <QList>
#include <gio/gio.h>
#include <memory>

namespace Peony {

class FileNodeReporter;

/*!
 * \brief The FileNode class
 * <br>
 * This class contains 3 parts:
 * 1. the self info, such as uri, basename, etc,
 * 2. the parent node and list of children,
 * 3. the reporter handle.
 * </br>
 * <br>
 * FileNode is smilar to FileItem, but it is more simple. It is used with many limitation.
 * Cause it method is Synchronized, you should never try use this class in the ui-thread.
 * This class is desgined for file operation classes which running in other threads.
 * For example, a copy operation might need enumerate all children for get the total size
 * of the source files.
 * </br>
 * <br>
 * The FileNodeReporter handle is used for send signal to tell other object the current state
 * of file node enumeration. Actually, a FileNode instance always be with a FileNodeReproter
 * instance at its initialization.
 * </br>
 * \see FileNodeReporter.
 */
class FileNode
{
    friend class FileNodeReporter;
public:
    enum State {
        Unhandled,
        Handled,
        HandledButDoNotDeleteDestFile,
        Cleared,
        Invalid
    };
    FileNode(QString uri, FileNode* parent, FileNodeReporter *reporter = nullptr);
    ~FileNode();

    //FIXME: do i need add cancel function?
    void findChildrenRecursively();
    void computeTotalSize(goffset *offset);

    QString uri() {return m_uri;}
    QString baseName() {return m_basename;}
    FileNode *parent() {return m_parent;}
    QList<FileNode*> *children() {return m_children;}
    qint64 size() {return m_size;}
    bool isFolder() {return m_is_folder;}

    QString getRelativePath();

    /*!
     * \brief setDestUri
     * \param uri, the raw source uri of this file.
     * <br>
     * In peony-qt, operantion is cancellable. That means we need add rollback function
     * for handling cancelled operation, to make sure that the operation is completed.
     * We should create a list of node trees for the files which have been copied or moved
     * for 'recover' them to the previous uri.
     * Dest uri is set when the file has been copied or moved to dest location.
     * This will aslo changed the node states. The rollback function will determine how to roll back
     * based on the status of dest uri and current states.
     * </br>
     * \see setState().
     */
    void setDestUri(QString uri) {m_dest_uri = uri;}
    /*!
     * \brief setState
     * \param state
     * <br>
     * State represent the current state of file.
     * If a node state is Unhandled, rollback function will ignore this node.
     * If a node state is Handled, rollback function will try deleteing dest uri's file.
     * If a node state is HandledButDoNotDeleteDestFile, rollback function will ignore this node.
     * If a node state is Cleared, rollback function will try moving the dest uri's file
     * to raw position.
     * </br>
     * \note Cancel and rollback is flawed. For example, if you move a file to a exsited
     * file, and you didn't overwrite the old exsited file. The rollback of operation might
     * take the exsited file to orignal position. You can hide the cancel entry in clean progress
     * to avoid this problem.
     * \note HandledButDoNotDeleteDestFile is set when error handle set the Flags to Ignore or Backup.
     * \note Rollback function in operation, I call it internal rollback. It means a canceling,
     * not a rolling back. Peony-qt should provied a operation manager class to make the operation
     * really rollbackable.
     */
    void setState(State state) {m_state = state;}
private:
    QString m_uri = nullptr;
    QString m_basename = nullptr;
    goffset m_size = 0;
    bool m_is_folder = false;
    FileNode *m_parent = nullptr;
    QList<FileNode*> *m_children = nullptr;

    QString m_dest_uri = nullptr;
    State m_state = Unhandled;

    FileNodeReporter *m_reporter = nullptr;
};

}

#endif // FILENODE_H
