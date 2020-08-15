#-------------------------------------------------
#
# Project created by QtCreator 2019-11-05T10:40:52
#
#-------------------------------------------------

QT       += widgets

TARGET = peony-qt-menu-plugin-mate-terminal
TEMPLATE = lib

DEFINES += PEONYQTMENUPLUGINMATETERMINAL_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../../plugin-iface/plugin-iface.pri)
include(../../libpeony-qt/libpeony-qt-header.pri)
PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0
CONFIG += c++11 link_pkgconfig no_keywords lrelease
LIBS += -L$$PWD/../../libpeony-qt/ -lpeony-qt

SOURCES += \
        mate-terminal-menu-plugin.cpp

HEADERS += \
        mate-terminal-menu-plugin.h \
        peony-qt-menu-plugin-mate-terminal_global.h 

DESTDIR = ../../testdir

TRANSLATIONS += translations/peony-qt-mate-terminal-extension_zh_CN.ts

unix {
    target.path = $$[QT_INSTALL_LIBS]/peony-extensions
    INSTALLS += target
}

RESOURCES += \
    peony-qt-menu-plugin-mate-terminal.qrc
