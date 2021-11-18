#-------------------------------------------------
#
# Project created by QtCreator 2019-10-28T14:12:53
#
#-------------------------------------------------

QT       += core gui x11extras dbus concurrent KWindowSystem KWaylandClient
# for tablet-desktop
QT       += svg network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../common.pri)

TARGET = peony-qt-desktop
TEMPLATE = app
QMAKE_CXXFLAGS += -Werror=return-type -Werror=return-local-addr -Werror=uninitialized -Werror=unused-label
PLUGIN_INSTALL_DIRS = $$[QT_INSTALL_LIBS]/peony-extensions
DEFINES += PLUGIN_INSTALL_DIRS='\\"$${PLUGIN_INSTALL_DIRS}\\"'

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../libpeony-qt/libpeony-qt-header.pri)
include(../3rd-parties/SingleApplication/singleapplication.pri)
include(../3rd-parties/qtsingleapplication/qtsingleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0 gsettings-qt libcanberra wayland-client
# for tablet-desktop
PKGCONFIG += libbamf3 x11 xrandr xtst

#导入模块
include($$PWD/main/main.pri)
include($$PWD/window/window.pri)
include($$PWD/settings/settings.pri)
include($$PWD/interface/interface.pri)
include($$PWD/desktops/desktops.pri)

CONFIG += c++11 link_pkgconfig no_keywords lrelease

LIBS += -L$$PWD/../libpeony-qt/ -lpeony -lX11 -lukui-log4qt

TRANSLATIONS += ../translations/peony-qt-desktop/peony-qt-desktop_zh_CN.ts \
                ../translations/peony-qt-desktop/peony-qt-desktop_tr.ts \
                ../translations/peony-qt-desktop/peony-qt-desktop_cs.ts \
                ../translations/peony-qt-desktop/peony-qt-desktop_bo_CN.ts

SOURCES += \
    main.cpp \
    plasma-shell-manager.cpp \
    primary-manager.cpp \
    waylandoutputmanager.cpp \
    ukui-output-core.c

HEADERS += \
    plasma-shell-manager.h \
    primary-manager.h \
    waylandoutputmanager.h \
    ukui-output-client.h

target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

#QM_FILES_RESOURCE_PREFIX = ../translations/peony-qt-desktop
QM_FILES_INSTALL_PATH = /usr/share/peony-qt-desktop

desktop_data.path = /etc/xdg/autostart
desktop_data.files += ../data/peony-desktop.desktop
INSTALLS += desktop_data

DISTFILES +=

RESOURCES += \
    $$PWD/resources/peony-qt-desktop-style.qrc
