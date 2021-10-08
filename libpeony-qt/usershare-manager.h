
#ifndef USERSHARE_MANAGER_H
#define USERSHARE_MANAGER_H

#include <QMap>
#include <QMutex>
#include <QObject>
//#include <QProcess>
#include "peony-core_global.h"
namespace Peony {

class PEONYCORESHARED_EXPORT ShareInfo
{
public:
    ShareInfo& operator= (const ShareInfo*);

    QString name;
    QString comment;
    QString originalPath;

    bool readOnly   = true;
    bool allowGuest = false;
    bool isShared   = false;
};


class PEONYCORESHARED_EXPORT UserShareInfoManager : public QObject
{
    Q_OBJECT
public:
    static UserShareInfoManager* getInstance ();
    QString exectueCommand (QStringList& args, QString sharePath, bool* ret /* out */);

    bool hasSharedInfo (QString& name);
    void removeShareInfo (QString& name);
    bool addShareInfo (ShareInfo* shareInfo);
    bool updateShareInfo (ShareInfo& shareInfo);
    const ShareInfo* getShareInfo (QString& name);

private:
    explicit UserShareInfoManager (QObject* parent = nullptr) : QObject(parent) {};

Q_SIGNALS:
    void signal_addSharedFolder(const ShareInfo& shareInfo, bool successed);
    void signal_deleteSharedFolder(const QString& originalPath, bool successed);

private:
    bool                            m_bInit = false;
    QMutex                          m_mutex;
    QMap <QString, ShareInfo*>      m_sharedInfoMap;
    static UserShareInfoManager*    g_shareInfo;
};
}
#endif // USERSHARE_MANAGER_H
