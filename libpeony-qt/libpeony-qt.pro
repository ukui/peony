#-------------------------------------------------
#
# Project created by QtCreator 2019-07-20T10:23:11
#
#-------------------------------------------------

QT       += core widgets gui

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TARGET = peony-qt
TEMPLATE = lib

CONFIG += link_pkgconfig no_keywords c++11
PKGCONFIG += glib-2.0 gio-2.0 gio-unix-2.0

DEFINES += PEONYCORE_LIBRARY

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

unix {
    target.path = $$[QT_INSTALL_LIBS]
    INSTALLS += target

    header.path = /usr/include/peony-qt
    header.files += *.h model/*.h file-operation/*.h vfs/*.h controls/ ../plugin-iface/*.h
    header.files += development-files/header-files/*
    INSTALLS += header

    pcfile.path = $$[QT_INSTALL_LIBS]/pkgconfig
    pcfile.files = development-files/peony-qt.pc
    INSTALLS += pcfile
}
