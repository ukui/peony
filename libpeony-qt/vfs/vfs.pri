INCLUDEPATH += $$PWD \
               $$PWD/..

HEADERS += $$PWD/peony-search-vfs-file.h \
           $$PWD/peony-search-vfs-file-enumerator.h \
           $$PWD/search-vfs-register.h \
    $$PWD/search-vfs-manager.h \
    $$PWD/search-vfs-uri-parser.h \
    $$PWD/vfs-plugin-manager.h

SOURCES += $$PWD/peony-search-vfs-file.cpp \
           $$PWD/peony-search-vfs-file-enumerator.cpp \
           $$PWD/search-vfs-register.cpp \
    $$PWD/search-vfs-manager.cpp \
    $$PWD/search-vfs-uri-parser.cpp \
    $$PWD/vfs-plugin-manager.cpp
