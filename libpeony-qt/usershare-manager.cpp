
#include "usershare-manager.h"

#include <QDebug>
#include <QProcess>

#include <glib.h>

using namespace Peony;

UserShareInfoManager* UserShareInfoManager::g_shareInfo = nullptr;

static void         parseShareInfo (ShareInfo& shareInfo, QString& content);
static QString      exectueCommand (QStringList& args, bool* ret /* out */);

ShareInfo& ShareInfo::operator =(const ShareInfo* oth)
{
    if (oth == this) {
        return *this;
    }

    this->name = oth->name;
    this->comment = oth->comment;
    this->isShared = oth->isShared;
    this->readOnly = oth->readOnly;
    this->allowGuest = oth->allowGuest;
    this->originalPath = oth->originalPath;

    return *this;
}

UserShareInfoManager* UserShareInfoManager::getInstance()
{
    if (!g_shareInfo) {
        g_shareInfo = new UserShareInfoManager;
        //gShareInfo->run();
    }

    return g_shareInfo;
}


static void parseShareInfo (ShareInfo& shareInfo, QString& content)
{
    auto lines = content.split('\n');

    for (auto line : lines) {
        if (line.startsWith("path")) {
            shareInfo.originalPath = line.split('=').last();
        } else if (line.startsWith("comment")) {
            shareInfo.comment = line.split('=').last();
        } else if (line.startsWith("usershare_acl")) {
            shareInfo.readOnly = line.contains("Everyone:R");
        } else if (line.startsWith("guest_ok")) {
            shareInfo.allowGuest = line.split('=').last() == "y";
        }
    }
}

QString UserShareInfoManager::exectueCommand (QStringList& args, bool* retb /* out */, QString sharedPath)
{
    QProcess proc;
    proc.open();

    // Check whether sambashare exists and contains the current user
    QProcess::execute ("bash pkexec /usr/bin/peony-share.sh", QStringList() << g_get_user_name () << sharedPath);

    // Shared folder
    args.prepend ("net");
    proc.start("bash");
//    args.prepend("pkexec");
    proc.waitForStarted();
    QString cmd = args.join(" ");
    proc.write(cmd.toUtf8() + "\n");
    proc.waitForFinished(500);
    if (retb) {
        if (proc.readAllStandardError().isEmpty()) {
            *retb = true;
        } else {
            *retb = false;
        }
    }

    QString all = proc.readAllStandardOutput();
    proc.close();

    return all;
}

bool UserShareInfoManager::updateShareInfo(ShareInfo &shareInfo)
{
    if ("" == shareInfo.name
            || shareInfo.name.isEmpty()
            || shareInfo.originalPath.isEmpty()) {
        return false;
    }

    bool ret = false;
    QStringList args;
    ShareInfo* sharedInfo = new ShareInfo;
    sharedInfo->name = shareInfo.name;
    sharedInfo->comment = shareInfo.comment;
    sharedInfo->isShared = shareInfo.isShared;
    sharedInfo->readOnly = shareInfo.readOnly;
    sharedInfo->allowGuest = shareInfo.allowGuest;
    sharedInfo->originalPath = shareInfo.originalPath;

    m_mutex.lock();
    bool isShare = true;
    if (m_sharedInfoMap.contains(sharedInfo->name)
            && nullptr != m_sharedInfoMap[sharedInfo->name]) {
        if(sharedInfo->isShared == m_sharedInfoMap[sharedInfo->name]->isShared){
            isShare = false;
        }
        delete m_sharedInfoMap[sharedInfo->name];
    }
    m_sharedInfoMap[sharedInfo->name] = sharedInfo;
    m_mutex.unlock();

    args << "usershare" << "add";
    args << sharedInfo->name;
    args << sharedInfo->originalPath;
    args << (sharedInfo->comment.isNull() ? "Peony-Qt-Share-Extension" : sharedInfo->comment);
    args << (sharedInfo->readOnly ? "Everyone:R" : "Everyone:F");
    args << (sharedInfo->allowGuest ? "guest_ok=y" : "guest_ok=n");

    exectueCommand (args, &ret);
    if(isShare)
        Q_EMIT signal_addSharedFolder(*sharedInfo, ret);
    return ret;
}

const ShareInfo* UserShareInfoManager::getShareInfo(QString &name)
{
    if (nullptr == name || name.isEmpty()) {
        qDebug() << "invalid param";
        return nullptr;
    }

    if (!m_bInit) {
        bool            ret;
        QStringList     args;
        args << "usershare" << "info" << name;
        QString result = exectueCommand (args, &ret);
        if (!ret && result.isEmpty()) {
            return nullptr;
        }

        // parse UserShared
        ShareInfo* shareInfo = new ShareInfo;
        shareInfo->name = name;
        parseShareInfo(*shareInfo, result);
        if (!addShareInfo(shareInfo)) {
            delete shareInfo;
        }
    }

    m_mutex.lock();
    if (!m_sharedInfoMap.contains(name)) {
        m_mutex.unlock();
        return nullptr;
    }

    m_mutex.unlock();

    return m_sharedInfoMap[name];
}

bool UserShareInfoManager::addShareInfo(ShareInfo* shareInfo)
{
    if (nullptr == shareInfo
            || shareInfo->name.isEmpty()
            || shareInfo->originalPath.isEmpty()) {
        return false;
    }

    m_mutex.lock();
    if (m_sharedInfoMap.contains(shareInfo->name)) {
        m_mutex.unlock();
        return false;
    }

    shareInfo->isShared = true;
    m_sharedInfoMap[shareInfo->name] = shareInfo;
    m_mutex.unlock();

    return true;
}

void UserShareInfoManager::removeShareInfo(QString &name)
{
    m_mutex.lock();
    QString originalPath;
    if (m_sharedInfoMap.contains(name)) {
        if (nullptr != m_sharedInfoMap[name])
        {
            originalPath=m_sharedInfoMap[name]->originalPath;
            delete m_sharedInfoMap[name];
        }
        m_sharedInfoMap.remove(name);
    }
    m_mutex.unlock();

    QStringList args;
    args << "usershare" << "delete" << name;

    bool ret = false;
    exectueCommand (args, &ret);
    Q_EMIT signal_deleteSharedFolder(originalPath, ret);
}
