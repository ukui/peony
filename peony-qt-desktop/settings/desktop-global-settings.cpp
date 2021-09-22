//
// Created by hxf on 2021/8/17.
//

#include "desktop-global-settings.h"

#ifdef KYLIN_COMMON
#include <ukuisdk/kylin-com4cxx.h>
#endif

#include <QDebug>

using namespace Peony;

static DesktopGlobalSettings *g_desktopGlobalSettings = nullptr;

const QString DesktopGlobalSettings::V10SP1           = V10_SP1;
const QString DesktopGlobalSettings::V10SP1Edu        = V10_SP1_EDU;

DesktopGlobalSettings *DesktopGlobalSettings::globalInstance(QObject *parent)
{
    if (!g_desktopGlobalSettings) {
        g_desktopGlobalSettings = new DesktopGlobalSettings(parent);
    }
    return g_desktopGlobalSettings;
}

DesktopGlobalSettings::DesktopGlobalSettings(QObject *parent) : QObject(parent)
{

}

const QString &DesktopGlobalSettings::getCurrentProjectName()
{
#ifdef KYLIN_COMMON
    QString platFromName = QString::fromStdString(KDKGetPrjCodeName());
    qDebug() << "[DesktopGlobalSettings::getCurrentProjectName]" << platFromName;
    if (QString::compare(V10_SP1, platFromName, Qt::CaseInsensitive) == 0) {
        return V10SP1;
    }

    if (QString::compare(V10_SP1_EDU, platFromName, Qt::CaseInsensitive) == 0) {
        return V10SP1Edu;
    }

    return V10SP1;
#else
    return V10SP1;
#endif
}
