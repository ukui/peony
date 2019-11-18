#ifndef LINUXPWDHELPER_H
#define LINUXPWDHELPER_H

#include <QObject>

#include <pwd.h>
#include <sys/types.h>

class PWDItem
{
public:
    explicit PWDItem(passwd *user);
    ~PWDItem() {}

    const QString userName() {return m_user_name;}
    int userId() {return m_uid;}
    int groupId() {return m_gid;}

    const QString fullName() {return m_full_name;}
    const QString homeDir() {return m_home_dir;}
    const QString shellDir() {return m_shell_dir;}

private:
    QString m_user_name;
    QString m_full_name;
    QString m_home_dir;
    QString m_shell_dir;

    int m_uid = -1;
    int m_gid = -1;
};

class LinuxPWDHelper
{
public:
    static const QList<PWDItem> getAllUserInfos();

private:
    LinuxPWDHelper();
};

#endif // LINUXPWDHELPER_H
