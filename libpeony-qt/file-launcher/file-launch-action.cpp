#include "file-launch-action.h"
#include <gio/gdesktopappinfo.h>

#include "file-info.h"
#include "file-info-job.h"

using namespace Peony;

FileLaunchAction::FileLaunchAction(const QString &uri, GAppInfo *app_info, QObject *parent) : QAction(parent)
{
    m_uri = uri;
    m_app_info = static_cast<GAppInfo*>(g_object_ref(app_info));
    GThemedIcon *icon = G_THEMED_ICON(g_app_info_get_icon(m_app_info));
    const char * const * icon_names = g_themed_icon_get_names(icon);

    if (icon_names)
        m_icon = QIcon::fromTheme(*icon_names);
    setIcon(m_icon);
    m_info_name = g_app_info_get_name(m_app_info);
    setText(m_info_name);
    m_info_display_name = g_app_info_get_display_name(m_app_info);

    connect(this, &QAction::triggered, this, &FileLaunchAction::lauchFileAsync);
}

FileLaunchAction::~FileLaunchAction()
{
    g_object_unref(m_app_info);
}

const QString FileLaunchAction::getUri()
{
    return m_uri;
}

bool FileLaunchAction::isDesktopFileAction()
{
    auto info = FileInfo::fromUri(m_uri);
    if (info->isEmptyInfo()) {
        FileInfoJob j(info);
        j.querySync();
    }
    return info->isDesktopFile();
}

const QString FileLaunchAction::getAppInfoName()
{
    return m_info_name;
}

const QString FileLaunchAction::getAppInfoDisplayName()
{
    return m_info_display_name;
}

void FileLaunchAction::lauchFileSync()
{
    if (isDesktopFileAction()) {
        g_app_info_launch(m_app_info, nullptr, nullptr, nullptr);
    } else {
        GList *l = nullptr;
        char *uri = g_strdup(m_uri.toUtf8().constData());
        l = g_list_prepend(l, uri);
        g_app_info_launch_uris(m_app_info,
                               l,
                               nullptr,
                               nullptr);
        g_list_free(l);
    }

    return;
    if (isDesktopFileAction()) {
        auto desktop_info = G_DESKTOP_APP_INFO(m_app_info);
        g_desktop_app_info_launch_uris_as_manager (desktop_info,
                                                   nullptr,
                                                   nullptr,
                                                   G_SPAWN_DEFAULT,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr);
    } else {
        g_app_info_launch_default_for_uri(m_uri.toUtf8().constData(),
                                          nullptr,
                                          nullptr);
    }
}

void FileLaunchAction::lauchFileAsync()
{
    if (isDesktopFileAction()) {
        g_app_info_launch_uris_async(m_app_info, nullptr,
                                     nullptr, nullptr,
                                     nullptr, nullptr);
    } else {
        GList *l = nullptr;
        char *uri = g_strdup(m_uri.toUtf8().constData());
        l = g_list_prepend(l, uri);
        g_app_info_launch_uris_async(m_app_info,
                                     l,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr);
        g_list_free(l);
    }

    return;
    if (isDesktopFileAction()) {
        auto desktop_info = G_DESKTOP_APP_INFO(m_app_info);
        g_desktop_app_info_launch_uris_as_manager (desktop_info,
                                                   nullptr,
                                                   nullptr,
                                                   G_SPAWN_DEFAULT,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr);
    } else {
        g_app_info_launch_default_for_uri_async(m_uri.toUtf8().constData(),
                                                nullptr,
                                                nullptr,
                                                nullptr,
                                                nullptr);
    }
}
