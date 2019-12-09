/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

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
        mimeType = info->mimeType();
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
        actions<<new FileLaunchAction(uri, app_info, true);
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
        actions<<new FileLaunchAction(uri, app_info, true);
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
        actions<<new FileLaunchAction(uri, app_info, true);
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
        actions<<new FileLaunchAction(uri, app_info, true);
        g_object_unref(app_info);
        l = l->next;
    }
    return actions;
}

void FileLaunchManager::openSync(const QString &uri)
{
    QString tmp = uri;
    auto targetUri = FileUtils::getTargetUri(uri);
    if (targetUri.isNull()) {
        tmp = targetUri;
    }
    auto action = getDefaultAction(tmp);
    action->lauchFileSync();
    action->deleteLater();
}

void FileLaunchManager::openAsync(const QString &uri)
{
    QString tmp = uri;
    auto targetUri = FileUtils::getTargetUri(uri);
    if (!targetUri.isNull()) {
        tmp = targetUri;
        qDebug()<<"open async"<<targetUri;
    }
    auto action = getDefaultAction(tmp);
    action->lauchFileAsync();
    action->deleteLater();
}

void FileLaunchManager::setDefaultLauchAction(const QString &uri, FileLaunchAction *action)
{
    auto info = FileInfo::fromUri(uri, false);
    if (info->mimeType().isEmpty()) {
        FileInfoJob job(info);
        job.querySync();
    }
    g_app_info_set_as_default_for_type(action->gAppInfo(),
                                       info->mimeType().toUtf8(),
                                       nullptr);
}
