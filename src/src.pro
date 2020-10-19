#-------------------------------------------------
#
# Project created by QtCreator 2019-06-20T09:45:20
#
#-------------------------------------------------

QT       += core gui gui-private x11extras dbus KWindowSystem concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets widgets-private

VERSION = 3.0.0

TARGET = peony

TEMPLATE = app

include(../libpeony-qt/libpeony-qt-header.pri)
include(../3rd-parties/SingleApplication/singleapplication.pri)
include(windows/windows.pri)
include(control/control.pri)
#include(view/view.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0 gsettings-qt libcanberra
LIBS +=-lgio-2.0 -lglib-2.0 -lX11
CONFIG += c++11 link_pkgconfig no_keywords lrelease

LIBS += -L$$PWD/../libpeony-qt/ -lpeony

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TRANSLATIONS += ../translations/peony-qt/peony-qt_zh_CN.ts \
                ../translations/peony-qt/peony-qt_tr.ts

SOURCES += \
    peony-application.cpp \
    peony-main-window-style.cpp \
    peony-main.cpp \
    peony-screen.cpp

HEADERS += \
    peony-application.h \
    peony-main-window-style.h \
    peony-screen.h

INCLUDEPATH    += ../plugin-iface

target.path = /usr/bin
INSTALLS += target

#QM_FILES_RESOURCE_PREFIX = ../translations/peony-qt
QM_FILES_INSTALL_PATH = /usr/share/peony-qt

data.path = /usr/share/applications
data.files += ../data/peony.desktop \
              ../data/peony-computer.desktop \
              ../data/peony-home.desktop \
              ../data/peony-trash.desktop \
              ../data/peony-desktop.desktop
INSTALLS += data

RESOURCES += \
    custome.qrc
