#-------------------------------------------------
#
# Project created by QtCreator 2019-10-28T14:12:53
#
#-------------------------------------------------

QT       += core gui x11extras dbus concurrent KWindowSystem

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
DEFINES += QAPPLICATION_CLASS=QApplication

PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0 gsettings-qt libcanberra
CONFIG += c++11 link_pkgconfig no_keywords lrelease

LIBS += -L$$PWD/../libpeony-qt/ -lpeony -lX11

TRANSLATIONS += ../translations/peony-qt-desktop/peony-qt-desktop_zh_CN.ts \
                ../translations/peony-qt-desktop/peony-qt-desktop_tr.ts \
                ../translations/peony-qt-desktop/peony-qt-desktop_cs.ts

SOURCES += \
    main.cpp \
    desktop-window.cpp \
    peony-desktop-application.cpp \
    fm-dbus-service.cpp \
    desktop-item-model.cpp \
    desktop-icon-view.cpp \
    desktop-icon-view-delegate.cpp \
    desktop-index-widget.cpp \
    desktop-menu.cpp \
    desktop-menu-plugin-manager.cpp \
    desktop-item-proxy-model.cpp \
    peony-json-operation.cpp \
    bw-list-info.cpp \
    peony-dbus-service.cpp \
    user-dir-manager.cpp

HEADERS += \
    desktop-window.h \
    peony-desktop-application.h \
    fm-dbus-service.h \
    desktop-item-model.h \
    desktop-icon-view.h \
    desktop-icon-view-delegate.h \
    desktop-index-widget.h \
    desktop-menu.h \
    desktop-menu-plugin-manager.h \
    desktop-item-proxy-model.h \
    peony-json-operation.h \
    bw-list-info.h \
    peony-dbus-service.h \
    user-dir-manager.h

target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

#QM_FILES_RESOURCE_PREFIX = ../translations/peony-qt-desktop
QM_FILES_INSTALL_PATH = /usr/share/peony-qt-desktop

data.path = /etc/xdg/autostart
data.files += ../data/peony-desktop.desktop

DISTFILES +=

RESOURCES += \
    peony-qt-desktop-style.qrc
