#ifndef USERDIRMANAGER_H
#define USERDIRMANAGER_H

#include <QObject>
#include <QStringList>
#include <QFileSystemWatcher>
#include <QMap>
#include <QDir>
#include <QSettings>

namespace Peony {
class UserdirManager : public QObject
{
    Q_OBJECT
public:
    explicit UserdirManager(QObject *parent = nullptr);
    void getUserdir();
    void moveFile();

Q_SIGNALS:
    void desktopDirChanged();
    void thumbnailSetingChange();

private:
    QString m_user_name;
    QString m_user_path;
    QMap<QString,QString> m_last_user_dir;
    QMap<QString,QString> m_current_user_dir;
    QFileSystemWatcher *m_user_dir_watcher;
    QDir *m_dir;
    QStringList m_file_list;
    QSettings *m_settings;
    bool m_do_not_thumbnail;
    bool m_allow_parallel;
    bool m_showTrashDialog;
    int m_times = 9;

};

}

#endif // USERDIRMANAGER_H
