INCLUDEPATH += $$PWD

include($$PWD/menu/menu.pri)
include($$PWD/view/view.pri)
include($$PWD/model/model.pri)

HEADERS += \
    $$PWD/desktop-mode.h \
    $$PWD/desktop-mode-factory.h

SOURCES += \
    $$PWD/desktop-mode.cpp \
    $$PWD/desktop-mode-factory.cpp
