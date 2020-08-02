# peony

[![build](https://github.com/ukui/peony/workflows/Check%20build/badge.svg?branch=master)](https://github.com/ukui/peony)

peony is the filemanager application of UKUI3.0.

# About this project

- [Wiki on GitHub](https://github.com/ukui/peony/wiki)

- [zh_CN](GUIDE_ZH_CN.md)

- [project preview](https://github.com/orgs/explorer-cs/projects/3?fullscreen=true)

# Description
Peony is intent to be the default FileManager application in UKUI3.0.

It uses the same underlying layer as Peony(glib/gvfs/gio), but uses Qt to refactor or improve anything else. Peony can be roughly divided into the following parts:

> 1. peony-qt-core: the abstraction of glib/gvfs/gio object or method from glib/c to qt/c++.

> 2. file-operation: file operation set based on core.

> 3. model: mapping core and operation to Qt's model/view framework.

> 4. extensions framework: migration of the Peony's extension framework.

> 5. ui: reconstructed based on the above parts and qt's ui framework.

# How to understand
Although Peony's documentation is limited and might be outdated, I still recommend you read them through. The documents in this project can be come together with doxygen, see [how to generate the document files](doxygen/README.md).

# Build and test

## Preparation

The dependency of Peony is very "clean". It is not difficult to build pre-depends.

### **In Debian/Ustable and Ubuntu 20.04**

Peony has been uploaded to debian/unstable, and downstreams can get the source in archive by:

> apt source peony

To build peony in local, we should

> sudo apt build-dep peony

~~I recommend you use latest UbuntuKylin(19.10 for now), and you can build pre-depends with such command:~~

> ~~sudo apt install libglib2.0-dev qt5-default libqt5x11extras5-dev libgsettings-qt-dev  libpoppler-qt5-dev  qttools5-dev-tools~~

~~NOTE: build-depends references the paragraph in debian/control.~~

~~and this is a recommend plugin:~~

> ~~sudo apt install qt5-gtk2-platformtheme~~

NOTE: actually Peony is designed to adapt to all different qt styles, but for now there are lots of things to be done. You can also experience it with other styles, such as breeze, oxygen, not only gtk.

This qpa plugin will let qt applications use system style in UbuntuKylin.

### **Porting to other distros**

It is a little chalange for porting peony to other distros. If you want to try, please make sure the distro's qt's version is higher than 5.6, and glib's version is higher than 2.48.

Note that although most of installtions of peony is implemented by qmake and make, I used some featrues provided by debhelper, which is the package tool in debian. That means it is better to know how to do those things without debhelper in other distros, too.

If there are some problems during porting peony to other distros, commit an issue here. I'm willing to help you.

## build from source and test

> git clone https://github.com/ukui/peony.git

> cd peony && mkdir build && cd build

> qmake .. && make

> sudo make install

> /usr/bin/peony

NOTE: Peony & peony-qt-desktop is based on libpeony3, so you should put the libpeony3.so to the directory which is included in ld's config file.

# Examples
Another way to get familiar with the project is through [some examples which I provided](https://github.com/Yue-Lan/libpeony-qt-development-examples).

These examples are based on libpeony3 and display some basic usage of the Peony's api. It is simpler than the project but can help us understand how to use the libpeony3 for development.

# Contribution
I really welcome you to participate in this project. Before that, you'd better read the [contribution manual](CONTRIBUTING.md).

# About third-party open source codes
I used some third-party code and modified it to fit into my project.

The 3rd parties codes would been placed in project's ${top-src-dir}/3rd-parties directory.

Note that Peony/libpeony-qt might use different license (GPL/LGPL) with a 3rd parties' license. I will keep those 3rd parties codes' Copyrights and Licenses.
