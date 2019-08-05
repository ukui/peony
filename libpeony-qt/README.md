# peony-qt-core
Core part of peony-qt, providing a set of APIs for peony-qt and other projects for file manager development.

# build-depends
- glib-2.0 (development files)
- gio-2.0 (develotment files)
- qtbase5-dev
- qtbase5-dev-tools
- pkg-config

# description
peony-qt is the filemanager of ukui3.0，which refactoring peony based on qt and glib/gio.

In simple terms, this project can be seem as the qt/c++ style wrapper of gio's api. It should provide all kinds of file operation that gio has supported, such as querying, enumerating, monitoring copying/moving, mounting, etc... And it will translate g_signal to qt's signals for using signal/slot between QObject based classes.

## designing concept
- 1. File and its info is shared in global scale.
- 2. Try to use asynchronous methods.
- 3. Integrate exception handling.
- 4. Detach front and backend as detached as possible.
