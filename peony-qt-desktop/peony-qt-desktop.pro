#-------------------------------------------------
#
# Project created by QtCreator 2019-10-28T14:12:53
#
#-------------------------------------------------

QT       += core gui x11extras dbus concurrent KWindowSystem svg network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 3.0.0

TARGET = peony-qt-desktop
TEMPLATE = app
QMAKE_CXXFLAGS += -Werror=return-type -Werror=return-local-addr -Werror=uninitialized -Werror=unused-label
PLUGIN_INSTALL_DIRS = $$[QT_INSTALL_LIBS]/peony-extensions
DEFINES += PLUGIN_INSTALL_DIRS='\\"$${PLUGIN_INSTALL_DIRS}\\"'

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../libpeony-qt/libpeony-qt-header.pri)
include(../3rd-parties/SingleApplication/singleapplication.pri)

#导入模块
include($$PWD/main/main.pri)
include($$PWD/window/window.pri)
include($$PWD/settings/settings.pri)
include($$PWD/interface/interface.pri)
include($$PWD/desktops/desktops.pri)

DEFINES += QAPPLICATION_CLASS=QApplication

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0 gsettings-qt libcanberra libbamf3 x11 xrandr xtst
CONFIG += c++11 link_pkgconfig no_keywords lrelease

LIBS += -L$$PWD/../libpeony-qt/ -lpeony -lX11

TRANSLATIONS += ../translations/peony-qt-desktop/peony-qt-desktop_zh_CN.ts \
                ../translations/peony-qt-desktop/peony-qt-desktop_tr.ts \
                ../translations/peony-qt-desktop/peony-qt-desktop_cs.ts

SOURCES += \
    main.cpp

HEADERS +=

target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

#QM_FILES_RESOURCE_PREFIX = ../translations/peony-qt-desktop
QM_FILES_INSTALL_PATH = /usr/share/peony-qt-desktop

data.path = /etc/xdg/autostart
data.files += ../data/peony-desktop.desktop

DISTFILES +=

RESOURCES += \
    $$PWD/resources/peony-qt-desktop-style.qrc
