INCLUDEPATH += $$PWD

include(location-bar/location-bar.pri)
include(path-bar/path-bar.pri)

HEADERS += \
    $$PWD/navigation-tool-bar.h \
    $$PWD/advanced-location-bar.h \
    $$PWD/navigation-bar.h

SOURCES += \
    $$PWD/navigation-tool-bar.cpp \
    $$PWD/advanced-location-bar.cpp \
    $$PWD/navigation-bar.cpp
