#ifndef FILERENAMEOPERATION_H
#define FILERENAMEOPERATION_H

#include "peony-core_global.h"
#include "file-operation.h"

namespace Peony {

class PEONYCORESHARED_EXPORT FileRenameOperation : public FileOperation
{
    Q_OBJECT
public:
    /*!
     * \brief FileRenameOperation
     * \param uri
     * \param newName
     * \details
     * In most filemanagers, files always show their display name at directory view,
     * but there were a special kind of files not, the .desktop files might show
     * their names based on their contents.
     * Rename Operation have to both effect at the normal files and .desktop files(executable).
     * \note
     * Rename a .desktop file is very complex. Because it would change the contents of the file.
     * In GLib/GIO's api, it will lost some attribute if change the file contents.
     */
    explicit FileRenameOperation(QString uri, QString newName);

    void run() override;
    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return m_info;}

private:
    GFileCopyFlags m_default_copy_flag = GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS|
                                                        G_FILE_COPY_ALL_METADATA);

    QString m_uri = nullptr;
    QString m_new_name = nullptr;

    std::shared_ptr<FileOperationInfo> m_info = nullptr;
};

}

#endif // FILERENAMEOPERATION_H
