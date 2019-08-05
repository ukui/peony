#ifndef FILEMOVEOPERATION_H
#define FILEMOVEOPERATION_H

#include "file-operation.h"
#include "file-info.h"

namespace Peony {

class FileMoveOperation : public FileOperation
{
public:
    explicit FileMoveOperation(QStringList sourceUris, QString destUri, QObject *parent = nullptr);
    void setForceUseFallback(bool useFallback = true) {m_force_use_fallback = useFallback;}

    void run() override;

Q_SIGNALS:
    void addOne(const QString &srcUri, const QString &destUri);

protected:
    static void progress_callback(goffset current_num_bytes,
                                  goffset total_num_bytes,
                                  FileMoveOperation *p_this);

    void move();
    void moveForceUseFallback();

private:
    QStringList m_source_uris;
    QString m_dest_uri = nullptr;

    QString m_current_src_uri = nullptr;
    QString m_current_dest_uri = nullptr;

    goffset m_current_offset = 0;
    goffset m_total_szie = 0;

    /*!
     * \brief m_force_use_callback
     * \value true, the move operation will use copy + delete fallback anyway.
     * \value false, the move operation will priority using native move operations
     * if supported. The native implementation may support moving directories
     * (for instance on moves inside the same filesystem).
     */
    bool m_force_use_fallback = false;

    bool m_ignore_all_errors = false;
    bool m_overwrite_all_duplicated = false;
    bool m_backup_all_duplicated = false;
};

}

#endif // FILEMOVEOPERATION_H
