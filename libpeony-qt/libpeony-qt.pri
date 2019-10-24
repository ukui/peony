include(peony-core.pri)
include(file-operation/file-operation.pri)
include(file-launcher/file-launcher.pri)
include(model/model.pri)
#search vfs extension based on peony-qt core.
include(vfs/vfs.pri)
#plugin interface
include(../plugin-iface/plugin-iface.pri)

include(convenient-utils/convenient-utils.pri)

RESOURCES += $$PWD/libpeony-qt.qrc

#ui-controls
include(controls/controls.pri)

#windows
include(windows/windows.pri)

DISTFILES +=

HEADERS += \
    $$PWD/plugin-manager.h

SOURCES += \
    $$PWD/plugin-manager.cpp
