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

## content
### FileInfo and FileInfoManager
In peony-qt, a file can be abstracted to a FileInfo class. It contains some information about the file, such as uri, icon, size, etc. When the FileInfo was created, it should be managed by FileInfoManager. So that the file system will be mapped to a hash table.
The FileInfoManager is desgined to a static single global instance, and the FileInfo instances are wrapped by a std::shared_ptr.
The FileInfo instance life is not same as a regular smart pointer's instance life.
Actually, FileInfo in FileInfoManager will destoryed by a third classed instance which hold this smart pointer. It will be destoryed when the last third classed instance holder deconstrunction.
For example, class A's instance required a FileInfo instance from global manager.
After a while. the instance of A was deleted. The instance of FileInfo is 'unused' now, so it must be removed from global manager, too.

In order to achieve such a mechanism, the third class also also needs to pay some effort. When the third class in deconstruction, it must judge if there is only global manager and itself hold the reference cout of the FileInfo instance.
If it is, the recources of FileInfo instance should be released.

The FileInfo instance could be 'incomplete' and 'virtual'. It means the real file might not exsited. When requiring a FileInfo instance, the instance aslo have not gotten all details of the file. You should use FileInfoJob for a querying at first.

### FileEnumerator
FileEnumerator is a 'holder' class of FileInfo instances. It provieds the file enumeration function of both local and virtual filesystem.
The enumeration is divided into two types, the synchronous one and the asynchronous one. asynchronous enueration need to connect the signal to monitor the enumeration states. Once enumeration finished, you can get the children of the target directoy, which is aslo abstract to the FileInfo instances. This instances are aslo incomplete and need be handled.

### FileWatcher
FileWatcher is desgined to monitor the directory changing. When there some changes ocurred, it will send the signal by the file change type.

### FileItem and models of FileItem
FileItem is a more advanced layer abstraction of FileInfo. This class is the interface of Qt's Model/View programming.
Through the FileItem, the file's info could be shown into a model-based view.

By this way, It could aslo used the Qt's ProxySortFilterModel for the sorting and filtering.

- see [Model](model/README.md)

### FileOperation
FileOperation is relatively independent from previous classes. All file operations should not run in ui thread. It dervied from QRunnable and should be executed by QThreadPool global instance.

- see [FileOperation](file-operation/README.md)

# License
- LGPLv3
