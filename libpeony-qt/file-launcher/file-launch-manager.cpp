#include "file-launch-manager.h"
#include "file-utils.h"
#include "file-info.h"
#include "file-info-job.h"

#include "file-launch-action.h"

#include <gio/gdesktopappinfo.h>
#include <QUrl>

#include <QTimer>

using namespace Peony;

FileLaunchManager::FileLaunchManager(QObject *parent) : QObject(parent)
{

}

FileLaunchAction *FileLaunchManager::getDefaultAction(const QString &uri)
{
    auto info = FileInfo::fromUri(uri);
    QString mimeType = info->mimeType();
    if (mimeType.isEmpty()) {
        FileInfoJob job(info);
        job.querySync();
    }
    if (info->canExecute() && info->uri().endsWith(".desktop")) {
        QUrl url = uri;
        auto path = url.path();
        GDesktopAppInfo *info = g_desktop_app_info_new_from_filename(path.toUtf8().constData());
        FileLaunchAction *action = new FileLaunchAction(uri, G_APP_INFO(info));
        g_object_unref(info);
        return action;
    } else {
        GAppInfo *info = g_app_info_get_default_for_type(mimeType.toUtf8().constData(), false);
        FileLaunchAction *action = new FileLaunchAction(uri, info);
        g_object_unref(info);
        return action;
    }
}

const QList<FileLaunchAction*> FileLaunchManager::getRecommendActions(const QString &uri)
{
    auto info = FileInfo::fromUri(uri);
    QString mimeType = info->mimeType();
    if (mimeType.isEmpty()) {
        FileInfoJob job(info);
        job.querySync();
        mimeType = info->mimeType();
    }
    GList *app_infos = g_app_info_get_recommended_for_type(mimeType.toUtf8().constData());
    GList *l = app_infos;
    QList<FileLaunchAction *> actions;
    while (l) {
        auto app_info = static_cast<GAppInfo*>(l->data);
        actions<<new FileLaunchAction(uri, app_info);
        g_object_unref(app_info);
        l = l->next;
    }
    return actions;
}

const QList<FileLaunchAction*> FileLaunchManager::getFallbackActions(const QString &uri)
{
    auto info = FileInfo::fromUri(uri);
    QString mimeType = info->mimeType();
    if (mimeType.isEmpty()) {
        FileInfoJob job(info);
        job.querySync();
        mimeType = info->mimeType();
    }
    GList *app_infos = g_app_info_get_fallback_for_type(mimeType.toUtf8().constData());
    GList *l = app_infos;
    QList<FileLaunchAction *> actions;
    while (l) {
        auto app_info = static_cast<GAppInfo*>(l->data);
        actions<<new FileLaunchAction(uri, app_info);
        g_object_unref(app_info);
        l = l->next;
    }
    return actions;
}

const QList<FileLaunchAction*> FileLaunchManager::getAllActionsForType(const QString &uri)
{
    auto info = FileInfo::fromUri(uri);
    QString mimeType = info->mimeType();
    if (mimeType.isEmpty()) {
        FileInfoJob job(info);
        job.querySync();
        mimeType = info->mimeType();
    }
    GList *app_infos = g_app_info_get_all_for_type(mimeType.toUtf8().constData());
    GList *l = app_infos;
    QList<FileLaunchAction *> actions;
    while (l) {
        auto app_info = static_cast<GAppInfo*>(l->data);
        actions<<new FileLaunchAction(uri, app_info);
        g_object_unref(app_info);
        l = l->next;
    }
    return actions;
}

const QList<FileLaunchAction*> FileLaunchManager::getAllActions(const QString &uri)
{
    GList *app_infos = g_app_info_get_all();
    GList *l = app_infos;
    QList<FileLaunchAction *> actions;
    while (l) {
        auto app_info = static_cast<GAppInfo*>(l->data);
        actions<<new FileLaunchAction(uri, app_info);
        g_object_unref(app_info);
        l = l->next;
    }
    return actions;
}

void FileLaunchManager::openSync(const QString &uri)
{
    auto action = getDefaultAction(uri);
    action->lauchFileSync();
    action->deleteLater();
}

void FileLaunchManager::openAsync(const QString &uri)
{
    auto action = getDefaultAction(uri);
    action->lauchFileAsync();
    action->deleteLater();
}
