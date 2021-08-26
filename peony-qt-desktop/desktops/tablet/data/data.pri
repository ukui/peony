INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/tablet-app-manager.h

SOURCES += \
    $$PWD/tablet-app-manager.cpp

desktop-tablet-system-app.files += $$PWD/desktop-tablet-system-app.ini
desktop-tablet-system-app.path = /usr/bin/

DISTFILES += \
    $$PWD/desktop-tablet-system-app.ini
