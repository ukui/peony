#ifndef FILELAUNCHMANAGER_H
#define FILELAUNCHMANAGER_H

#include <QObject>
#include "peony-core_global.h"

namespace Peony {

class FileLaunchAction;

/*!
 * \brief The FileLaunchManager class
 * \todo
 * support open multi-files.
 */
class FileLaunchManager : public QObject
{
    Q_OBJECT
public:
    static FileLaunchAction *getDefaultAction(const QString &uri);
    static const QList<FileLaunchAction*> getRecommendActions(const QString &uri);
    static const QList<FileLaunchAction*> getFallbackActions(const QString &uri);
    static const QList<FileLaunchAction*> getAllActionsForType(const QString &uri);
    static const QList<FileLaunchAction*> getAllActions(const QString &uri);

    static void openSync(const QString &uri);
    static void openAsync(const QString &uri);

private:
    explicit FileLaunchManager(QObject *parent = nullptr);
};

}

#endif // FILELAUNCHMANAGER_H
