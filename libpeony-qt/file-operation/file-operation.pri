INCLUDEPATH += $$PWD

#include(../peony-core.pri)

PKGCONFIG += gio-unix-2.0

HEADERS += \
    $$PWD/file-operation.h \
    $$PWD/file-move-operation.h \
    $$PWD/file-node.h \
    $$PWD/file-node-reporter.h \
    $$PWD/file-operation-progress-wizard.h \
    $$PWD/file-operation-error-handler.h \
    $$PWD/file-operation-error-dialog.h \
    $$PWD/file-copy-operation.h \
    $$PWD/file-operation-manager.h \
    $$PWD/file-delete-operation.h \
    $$PWD/file-link-operation.h \
    $$PWD/file-trash-operation.h \
    $$PWD/file-untrash-operation.h \
    $$PWD/file-rename-operation.h \
    $$PWD/file-count-operation.h

SOURCES += \
    $$PWD/file-operation.cpp \
    $$PWD/file-move-operation.cpp \
    $$PWD/file-node.cpp \
    $$PWD/file-node-reporter.cpp \
    $$PWD/file-operation-progress-wizard.cpp \
    $$PWD/file-operation-error-dialog.cpp \
    $$PWD/file-copy-operation.cpp \
    $$PWD/file-operation-manager.cpp \
    $$PWD/file-delete-operation.cpp \
    $$PWD/file-link-operation.cpp \
    $$PWD/file-trash-operation.cpp \
    $$PWD/file-untrash-operation.cpp \
    $$PWD/file-rename-operation.cpp \
    $$PWD/file-count-operation.cpp
