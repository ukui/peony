/****************************************************************************
** Meta object code from reading C++ file 'file-operation-manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-operation/file-operation-manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-operation-manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileOperationManager_t {
    QByteArrayData data[28];
    char stringdata0[379];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationManager_t qt_meta_stringdata_Peony__FileOperationManager = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Peony::FileOperationManager"
QT_MOC_LITERAL(1, 28, 6), // "closed"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 14), // "startOperation"
QT_MOC_LITERAL(4, 51, 14), // "FileOperation*"
QT_MOC_LITERAL(5, 66, 9), // "operation"
QT_MOC_LITERAL(6, 76, 12), // "addToHistory"
QT_MOC_LITERAL(7, 89, 15), // "startUndoOrRedo"
QT_MOC_LITERAL(8, 105, 34), // "std::shared_ptr<FileOperation..."
QT_MOC_LITERAL(9, 140, 4), // "info"
QT_MOC_LITERAL(10, 145, 7), // "canUndo"
QT_MOC_LITERAL(11, 153, 11), // "getUndoInfo"
QT_MOC_LITERAL(12, 165, 4), // "undo"
QT_MOC_LITERAL(13, 170, 7), // "canRedo"
QT_MOC_LITERAL(14, 178, 11), // "getRedoInfo"
QT_MOC_LITERAL(15, 190, 4), // "redo"
QT_MOC_LITERAL(16, 195, 12), // "clearHistory"
QT_MOC_LITERAL(17, 208, 14), // "onFilesDeleted"
QT_MOC_LITERAL(18, 223, 4), // "uris"
QT_MOC_LITERAL(19, 228, 11), // "handleError"
QT_MOC_LITERAL(20, 240, 19), // "FileOperationError&"
QT_MOC_LITERAL(21, 260, 5), // "error"
QT_MOC_LITERAL(22, 266, 19), // "registerFileWatcher"
QT_MOC_LITERAL(23, 286, 12), // "FileWatcher*"
QT_MOC_LITERAL(24, 299, 7), // "watcher"
QT_MOC_LITERAL(25, 307, 21), // "unregisterFileWatcher"
QT_MOC_LITERAL(26, 329, 30), // "manuallyNotifyDirectoryChanged"
QT_MOC_LITERAL(27, 360, 18) // "FileOperationInfo*"

    },
    "Peony::FileOperationManager\0closed\0\0"
    "startOperation\0FileOperation*\0operation\0"
    "addToHistory\0startUndoOrRedo\0"
    "std::shared_ptr<FileOperationInfo>\0"
    "info\0canUndo\0getUndoInfo\0undo\0canRedo\0"
    "getRedoInfo\0redo\0clearHistory\0"
    "onFilesDeleted\0uris\0handleError\0"
    "FileOperationError&\0error\0registerFileWatcher\0"
    "FileWatcher*\0watcher\0unregisterFileWatcher\0"
    "manuallyNotifyDirectoryChanged\0"
    "FileOperationInfo*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    2,   95,    2, 0x0a /* Public */,
       3,    1,  100,    2, 0x2a /* Public | MethodCloned */,
       7,    1,  103,    2, 0x0a /* Public */,
      10,    0,  106,    2, 0x0a /* Public */,
      11,    0,  107,    2, 0x0a /* Public */,
      12,    0,  108,    2, 0x0a /* Public */,
      13,    0,  109,    2, 0x0a /* Public */,
      14,    0,  110,    2, 0x0a /* Public */,
      15,    0,  111,    2, 0x0a /* Public */,
      16,    0,  112,    2, 0x0a /* Public */,
      17,    1,  113,    2, 0x0a /* Public */,
      19,    1,  116,    2, 0x0a /* Public */,
      22,    1,  119,    2, 0x0a /* Public */,
      25,    1,  122,    2, 0x0a /* Public */,
      26,    1,  125,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4, QMetaType::Bool,    5,    6,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Bool,
    0x80000000 | 8,
    QMetaType::Void,
    QMetaType::Bool,
    0x80000000 | 8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 27,    9,

       0        // eod
};

void Peony::FileOperationManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperationManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->closed(); break;
        case 1: _t->startOperation((*reinterpret_cast< FileOperation*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->startOperation((*reinterpret_cast< FileOperation*(*)>(_a[1]))); break;
        case 3: _t->startUndoOrRedo((*reinterpret_cast< std::shared_ptr<FileOperationInfo>(*)>(_a[1]))); break;
        case 4: { bool _r = _t->canUndo();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: { std::shared_ptr<FileOperationInfo> _r = _t->getUndoInfo();
            if (_a[0]) *reinterpret_cast< std::shared_ptr<FileOperationInfo>*>(_a[0]) = std::move(_r); }  break;
        case 6: _t->undo(); break;
        case 7: { bool _r = _t->canRedo();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { std::shared_ptr<FileOperationInfo> _r = _t->getRedoInfo();
            if (_a[0]) *reinterpret_cast< std::shared_ptr<FileOperationInfo>*>(_a[0]) = std::move(_r); }  break;
        case 9: _t->redo(); break;
        case 10: _t->clearHistory(); break;
        case 11: _t->onFilesDeleted((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 12: _t->handleError((*reinterpret_cast< FileOperationError(*)>(_a[1]))); break;
        case 13: _t->registerFileWatcher((*reinterpret_cast< FileWatcher*(*)>(_a[1]))); break;
        case 14: _t->unregisterFileWatcher((*reinterpret_cast< FileWatcher*(*)>(_a[1]))); break;
        case 15: _t->manuallyNotifyDirectoryChanged((*reinterpret_cast< FileOperationInfo*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FileOperation* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FileOperation* >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FileOperationInfo* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileOperationManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperationManager::closed)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationManager.data,
    qt_meta_data_Peony__FileOperationManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileOperationManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void Peony::FileOperationManager::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_Peony__FileOperationInfo_t {
    QByteArrayData data[1];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationInfo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationInfo_t qt_meta_stringdata_Peony__FileOperationInfo = {
    {
QT_MOC_LITERAL(0, 0, 24) // "Peony::FileOperationInfo"

    },
    "Peony::FileOperationInfo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationInfo[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Peony::FileOperationInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationInfo::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationInfo.data,
    qt_meta_data_Peony__FileOperationInfo,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationInfo.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileOperationInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
