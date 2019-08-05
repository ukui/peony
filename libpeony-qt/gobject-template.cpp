#include "gobject-template.h"

namespace Peony {

std::shared_ptr<GObjectTemplate<GFile>> wrapGFile(GFile *file) {
    return std::make_shared<GObjectTemplate<GFile>>(file);
}

std::shared_ptr<GObjectTemplate<GFileInfo>> wrapGFileInfo(GFileInfo *info){
    return std::make_shared<GObjectTemplate<GFileInfo>>(info);
}

std::shared_ptr<GObjectTemplate<GFileEnumerator>> wrapGFileEnumerator(GFileEnumerator *enumerator) {
    return std::make_shared<GObjectTemplate<GFileEnumerator>>(enumerator);
}

std::shared_ptr<GObjectTemplate<GFileMonitor>> wrapGFileMonitor(GFileMonitor *monitor){
    return std::make_shared<GObjectTemplate<GFileMonitor>>(monitor);
}

std::shared_ptr<GObjectTemplate<GVolumeMonitor>> wrapGVolumeMonitor(GVolumeMonitor *monitor){
    return std::make_shared<GObjectTemplate<GVolumeMonitor>>(monitor);
}

std::shared_ptr<GObjectTemplate<GDrive>> wrapGDrive(GDrive *drive){
    return std::make_shared<GObjectTemplate<GDrive>>(drive);
}

std::shared_ptr<GObjectTemplate<GVolume>> wrapGVolume(GVolume *volume) {
    return std::make_shared<GObjectTemplate<GVolume>>(volume);
}

std::shared_ptr<GObjectTemplate<GMount>> wrapGMount(GMount *mount) {
    return std::make_shared<GObjectTemplate<GMount>>(mount);
}

std::shared_ptr<GObjectTemplate<GIcon>> wrapGIcon(GIcon *icon) {
    return std::make_shared<GObjectTemplate<GIcon>>(icon);
}

std::shared_ptr<GObjectTemplate<GThemedIcon>> wrapGThemedIcon(GThemedIcon *icon) {
    return std::make_shared<GObjectTemplate<GThemedIcon>>(icon);
}

std::shared_ptr<GObjectTemplate<GCancellable>> wrapGCancellable(GCancellable *cancellable) {
    return std::make_shared<GObjectTemplate<GCancellable>>(cancellable);
}

/*
std::shared_ptr<GObjectTemplate<>> wrap() {
    return std::make_shared<GObjectTemplate<>>();
}
*/

}
