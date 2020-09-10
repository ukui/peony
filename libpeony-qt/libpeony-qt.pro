#-------------------------------------------------
#
# Project created by QtCreator 2019-07-20T10:23:11
#
#-------------------------------------------------

QT       += core widgets gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

VERSION = 3.0.0

TARGET = peony
TEMPLATE = lib

CONFIG += link_pkgconfig no_keywords c++11 lrelease hide_symbols
PKGCONFIG += glib-2.0 gio-2.0 gio-unix-2.0 poppler-qt5 gsettings-qt udisks2 libnotify

DEFINES += PEONYCORE_LIBRARY

PLUGIN_INSTALL_DIRS = $$[QT_INSTALL_LIBS]/peony-extensions
DEFINES += PLUGIN_INSTALL_DIRS='\\"$${PLUGIN_INSTALL_DIRS}\\"'

QMAKE_CXXFLAGS += -execution-charset:utf-8

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(libpeony-qt.pri)

TRANSLATIONS += ../translations/libpeony-qt/libpeony-qt_zh_CN.ts \
                ../translations/libpeony-qt/libpeony-qt_tr.ts

DESTDIR += $$PWD

unix {
    target.path = $$[QT_INSTALL_LIBS]
    INSTALLS += target

    header.path = /usr/include/peony-qt
    header.files += *.h model/*.h file-operation/*.h vfs/*.h controls/ ../plugin-iface/*.h convenient-utils/*.h
#    header.depends = header2
    header.files += development-files/header-files/*
    INSTALLS += header

#    header2.commands = sh $$PWD/copy-headers.sh
#    INSTALLS += header2

#    QMAKE_EXTRA_TARGETS += header header2

    pcfile.path = $$[QT_INSTALL_LIBS]/pkgconfig
    pcfile.files = development-files/peony.pc
    INSTALLS += pcfile

    #QM_FILES_RESOURCE_PREFIX = ../translations/libpeony-qt
    QM_FILES_INSTALL_PATH = /usr/share/libpeony-qt
}
