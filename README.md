# peony-qt
peony-qt is the filemanager application of UKUI3.0.

# About this project
- [zh_CN](GUIDE_ZH_CN.md)

- [project preview](https://github.com/orgs/explorer-cs/projects/3?fullscreen=true)

# Description
Peony-Qt is intent to replace Peony as default FileManager application in UKUI3.0.

It uses the same underlying layer as Peony(glib/gvfs/gio), but uses Qt to refactor or improve anything else. Peony-Qt can be roughly divided into the following parts:

> 1. peony-qt-core: the abstraction of glib/gvfs/gio object or method from glib/c to qt/c++.

> 2. file-operation: file operation set based on core.

> 3. model: mapping core and operation to Qt's model/view framework.

> 4. extensions framework: migration of the Peony's extension framework.

> 5. ui: reconstructed based on the above parts and qt's ui framework.

In addition, Peony-Qt also improves some designs in Peony.

# How to understand
Although Peony-Qt's documentation is limited and might be outdated, I still recommend you read them through. The documents in this project can be come together with doxygen, see [how to generate the document files](doxygen/README.md).

# Build and test

## Preparation
The dependency of Peony-Qt is very "clean". It is not difficult to build pre-depends.

I recommend you use latest UbuntuKylin(19.10 for now), and you can build pre-depends with such command:

> sudo apt install libglib2.0-dev qt5-default libqt5x11extras5-dev libgsettings-qt-dev

NOTE: build-depends references the paragraph in debian/control.

and this is a recommend plugin:

> sudo apt install qt5-gtk2-platformtheme

NOTE: actually peony-qt is designed to adapt to all different qt styles, but for now there are lots of things to be done. You can also experience it with other styles, such as breeze, oxygen, not only gtk.

This qpa plugin will let qt applications use system style in UbuntuKylin.

## build from source and test

> git clone https://github.com/explorer-cs/peony-qt.git

> cd peony-qt && mkdir build && cd build

> qmake .. && make

> sudo make install

> /usr/bin/peony-qt

NOTE: peony-qt & peony-qt-desktop is based on libpeony-qt, so you should put the libpeony-qt.so to the directory which is included in ld's config file.

# Examples
Another way to get familiar with the project is through [some examples which I provided](https://github.com/Yue-Lan/libpeony-qt-development-examples).

These examples are based on libpeony-qt and display some basic usage of the peony-qt's api. It is simpler than the project but can help us understand how to use the libpeony-qt for development.

# Contribution
I really welcome you to participate in this project. Before that, you'd better read the [contribution manual](CONTRIBUTING.md).

# About third-party open source codes
I used some third-party code and modified it to fit into my project.

The 3rd parties codes would been placed in project's ${top-src-dir}/3rd-parties directory.

Note that peony-qt/libpeony-qt might use different license (GPL/LGPL) with a 3rd parties' license. I will keep those 3rd parties codes' Copyrights and Licenses.
