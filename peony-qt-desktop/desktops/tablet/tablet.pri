INCLUDEPATH += $$PWD

include($$PWD/data/data.pri)
include($$PWD/config/config.pri)

HEADERS += \
    $$PWD/tablet-mode.h \
    $$PWD/tablet-mode-factory.h \
    $$PWD/KySmallPluginInterface.h

SOURCES += \
    $$PWD/tablet-mode.cpp \
    $$PWD/tablet-mode-factory.cpp

SOURCES += \
        $$PWD/src/Interface/currenttimeinterface.cpp \
        $$PWD/src/Interface/ukuichineseletter.cpp \
        $$PWD/src/Interface/ukuimenuinterface.cpp \
        $$PWD/src/UtilityFunction/fullcommonusewidget.cpp \
        $$PWD/src/UtilityFunction/fullitemdelegate.cpp \
        $$PWD/src/UtilityFunction/fulllistview.cpp \
        $$PWD/src/MainViewWidget/directorychangedthread.cpp \
        $$PWD/src/Style/style.cpp \
        $$PWD/src/UtilityFunction/thumbnail.cpp \
        $$PWD/src/UtilityFunction/utility.cpp \
        $$PWD/src/UtilityFunction/toolbox.cpp \
        $$PWD/src/UtilityFunction/tablet-plugin-widget.cpp


HEADERS += \
        $$PWD/src/Interface/currenttimeinterface.h \
        $$PWD/src/Interface/ukuichineseletter.h \
        $$PWD/src/Interface/ukuimenuinterface.h \
        $$PWD/src/UtilityFunction/abstractInterface.h \
        $$PWD/src/UtilityFunction/fullcommonusewidget.h \
        $$PWD/src/UtilityFunction/fullitemdelegate.h \
        $$PWD/src/UtilityFunction/fulllistview.h \
        $$PWD/src/Style/style.h \
        $$PWD/src/MainViewWidget/directorychangedthread.h \
        $$PWD/src/UtilityFunction/AbstractInterface.h \
        $$PWD/src/UtilityFunction/thumbnail.h \
        $$PWD/src/UtilityFunction/utility.h \
        $$PWD/src/UtilityFunction/toolbox.h \
        $$PWD/src/UtilityFunction/tablet-plugin-widget.h

RESOURCES += \
    $$PWD/img.qrc
