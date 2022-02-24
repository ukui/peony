include(peony-core.pri)
include(file-operation/file-operation.pri)
include(file-launcher/file-launcher.pri)
include(model/model.pri)
#search vfs extension based on peony-qt core.
include(vfs/vfs.pri)
#plugin interface
include(../plugin-iface/plugin-iface.pri)
include(effects/effects.pri)

include(convenient-utils/convenient-utils.pri)

RESOURCES += $$PWD/libpeony-qt.qrc

#ui-controls
include(controls/controls.pri)

#windows
include(windows/windows.pri)

# preview
include(thumbnail/thumbnail.pri)

HEADERS += \
    $$PWD/file-copy.h               \
    $$PWD/peony-log.h               \
    $$PWD/plugin-manager.h          \
    $$PWD/global-settings.h         \
    $$PWD/complementary-style.h     \
    $$PWD/global-fstabdata.h        \


SOURCES += \
    $$PWD/file-copy.cpp             \
    $$PWD/plugin-manager.cpp        \
    $$PWD/global-settings.cpp       \
    $$PWD/complementary-style.cpp   \
    $$PWD/global-fstabdata.cpp      \


FORMS += \
    $$PWD/connect-server-dialog.ui


