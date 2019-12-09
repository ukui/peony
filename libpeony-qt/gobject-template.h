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

#ifndef GOBJECTTEMPLATE_H
#define GOBJECTTEMPLATE_H

#include "peony-core_global.h"
#include <glib-object.h>
#include <gio/gio.h>
#include <memory>

#include <QDebug>

namespace Peony {

template<class T>
/*!
 * \brief The GObjectTemplate class
 * <br>
 * I have to say that managing the GObject data memory manually is harder than i thought.
 * This template class provide a way to wrap the GObject handle to a smart pointer.
 * and manage them automaticly.
 * for example, if you have a GFile handle and you really don't know when the handle will
 * be deleted. you can use 'auto filewrapper = wrapGFile(file)', then you should hold the
 * left value, and use 'filewrapper.get()' to get the GFile handle if you need.
 * </br>
 */
class PEONYCORESHARED_EXPORT GObjectTemplate
{
public:
    //do not use this constructor.
    GObjectTemplate();
    GObjectTemplate(T *obj, bool ref = false) {
        m_obj = obj;
        if (ref) {
            g_object_ref(obj);
        }
    }

    ~GObjectTemplate() {
        //qDebug()<<"~GObjectTemplate";
        if (m_obj)
            g_object_unref(m_obj);
    }

    T *get() {return m_obj;}

private:
    mutable T *m_obj = nullptr;
};

//typedef
typedef std::shared_ptr<GObjectTemplate<GFile>> GFileWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GFileInfo>> GFileInfoWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GFileEnumerator>> GFileEnumeratorWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GFileMonitor>> GFileMonitorWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GVolumeMonitor>> GVolumeMonitorWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GDrive>> GDriveWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GVolume>> GVolumeWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GMount>> GMountWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GIcon>> GIconWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GThemedIcon>> GThemedIconWrapperPtr;
typedef std::shared_ptr<GObjectTemplate<GCancellable>> GCancellableWrapperPtr;

std::shared_ptr<GObjectTemplate<GFile>> wrapGFile(GFile *file);
std::shared_ptr<GObjectTemplate<GFileInfo>> wrapGFileInfo(GFileInfo *info);
std::shared_ptr<GObjectTemplate<GFileEnumerator>> wrapGFileEnumerator(GFileEnumerator *enumerator);
std::shared_ptr<GObjectTemplate<GFileMonitor>> wrapGFileMonitor(GFileMonitor *monitor);
std::shared_ptr<GObjectTemplate<GVolumeMonitor>> wrapGVolumeMonitor(GVolumeMonitor *monitor);
std::shared_ptr<GObjectTemplate<GDrive>> wrapGDrive(GDrive *drive);
std::shared_ptr<GObjectTemplate<GVolume>> wrapGVolume(GVolume *volume);
std::shared_ptr<GObjectTemplate<GMount>> wrapGMount(GMount *mount);
std::shared_ptr<GObjectTemplate<GIcon>> wrapGIcon(GIcon *icon);
std::shared_ptr<GObjectTemplate<GThemedIcon>> wrapGThemedIcon(GThemedIcon *icon);
std::shared_ptr<GObjectTemplate<GCancellable>> wrapGCancellable(GCancellable *cancellable);

/*
std::shared_ptr<GObjectTemplate<>> wrap() {
    return std::make_shared<GObjectTemplate<>>();
}
*/

}

#endif // GOBJECTTEMPLATE_H
