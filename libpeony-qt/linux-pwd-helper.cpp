#include "linux-pwd-helper.h"

LinuxPWDHelper::LinuxPWDHelper()
{

}

const QList<PWDItem> LinuxPWDHelper::getAllUserInfos()
{
    setpwent();
    QList<PWDItem> l;
    struct passwd *user;
    while((user = getpwent())!=nullptr){
        l<<PWDItem(user);
    }
    endpwent();

    return l;
}

PWDItem::PWDItem(passwd *user)
{
    m_user_name = user->pw_name;
    m_full_name = user->pw_gecos;
    m_home_dir = user->pw_dir;
    m_shell_dir = user->pw_shell;
    m_uid = user->pw_uid;
    m_gid = user->pw_gid;
}
