TEMPLATE = subdirs
SUBDIRS = src libpeony-qt \ # plugin #libpeony-qt/test \ #plugin-iface
    #libpeony-qt/model/model-test \
    #libpeony-qt/file-operation/file-operation-test \
    peony-qt-plugin-test \
    peony-qt-desktop

src.depends = libpeony-qt
peony-qt-plugin-test.depends = libpeony-qt
peony-qt-desktop.depends = libpeony-qt
