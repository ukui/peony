#-------------------------------------------------
#
# Project created by QtCreator 2019-12-05T19:44:53
#
#-------------------------------------------------

QT       += widgets

TARGET = peony-qt-create-link
TEMPLATE = lib

DEFINES += PEONYQTCREATELINK_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0 peony-qt
CONFIG += c++11 link_pkgconfig no_keywords plugin

TRANSLATIONS = translations/peony-qt-create-link-extension_zh_CN.ts
DESTDIR += ../../testdir

SOURCES += \
        create-link-menu-plugin.cpp

HEADERS += \
        create-link-menu-plugin.h \
        peony-qt-create-link_global.h 

unix {
    target.path = /usr/lib/peony-qt-extensions
    INSTALLS += target
}

RESOURCES += \
    peony-qt-create-link.qrc
