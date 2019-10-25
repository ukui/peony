#ifndef FILELAUNCHACTION_H
#define FILELAUNCHACTION_H

#include <QAction>
#include <gio/gio.h>

#include "peony-core_global.h"

namespace Peony {

/*!
 * \brief The FileLaunchAction class
 * \todo
 * add error reprot GUI.
 */
class FileLaunchAction : public QAction
{
    Q_OBJECT
public:
    explicit FileLaunchAction(const QString &uri, GAppInfo *app_info, QObject *parent = nullptr);
    ~FileLaunchAction() override;
    const QString getUri();
    bool isDesktopFileAction();
    const QString getAppInfoName();
    const QString getAppInfoDisplayName();

protected:
    bool isValid();

public Q_SLOTS:
    void lauchFileSync();
    void lauchFileAsync();

private:
    QString m_uri;
    bool m_is_desktop_file;
    GAppInfo *m_app_info;

    QIcon m_icon;
    QString m_info_name;
    QString m_info_display_name;
};

}

#endif // FILELAUNCHACTION_H
