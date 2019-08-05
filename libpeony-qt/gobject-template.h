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
        qDebug()<<"~GObjectTemplate";
        if (m_obj)
            g_object_unref(m_obj);
    }

    T *get() {return m_obj;}

private:
    mutable T *m_obj = nullptr;
};

//typedef
typedef std::shared_ptr<GObjectTemplate<GFile>> GFileWrapper;
typedef std::shared_ptr<GObjectTemplate<GFileInfo>> GFileInfoWrapper;
typedef std::shared_ptr<GObjectTemplate<GFileEnumerator>> GFileEnumeratorWrapper;
typedef std::shared_ptr<GObjectTemplate<GFileMonitor>> GFileMonitorWrapper;
typedef std::shared_ptr<GObjectTemplate<GVolumeMonitor>> GVolumeMonitorWrapper;
typedef std::shared_ptr<GObjectTemplate<GDrive>> GDriveWrapper;
typedef std::shared_ptr<GObjectTemplate<GVolume>> GVolumeWrapper;
typedef std::shared_ptr<GObjectTemplate<GMount>> GMountWrapper;
typedef std::shared_ptr<GObjectTemplate<GIcon>> GIconWrapper;
typedef std::shared_ptr<GObjectTemplate<GThemedIcon>> GThemedIconWrapper;
typedef std::shared_ptr<GObjectTemplate<GCancellable>> GCancellableWrapper;

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
