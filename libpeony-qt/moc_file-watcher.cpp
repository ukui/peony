/****************************************************************************
** Meta object code from reading C++ file 'file-watcher.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-watcher.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-watcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileWatcher_t {
    QByteArrayData data[14];
    char stringdata0[173];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileWatcher_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileWatcher_t qt_meta_stringdata_Peony__FileWatcher = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Peony::FileWatcher"
QT_MOC_LITERAL(1, 19, 15), // "locationChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 6), // "oldUri"
QT_MOC_LITERAL(4, 43, 6), // "newUri"
QT_MOC_LITERAL(5, 50, 16), // "directoryDeleted"
QT_MOC_LITERAL(6, 67, 3), // "uri"
QT_MOC_LITERAL(7, 71, 18), // "directoryUnmounted"
QT_MOC_LITERAL(8, 90, 11), // "fileCreated"
QT_MOC_LITERAL(9, 102, 11), // "fileDeleted"
QT_MOC_LITERAL(10, 114, 11), // "fileChanged"
QT_MOC_LITERAL(11, 126, 22), // "requestUpdateDirectory"
QT_MOC_LITERAL(12, 149, 16), // "thumbnailUpdated"
QT_MOC_LITERAL(13, 166, 6) // "cancel"

    },
    "Peony::FileWatcher\0locationChanged\0\0"
    "oldUri\0newUri\0directoryDeleted\0uri\0"
    "directoryUnmounted\0fileCreated\0"
    "fileDeleted\0fileChanged\0requestUpdateDirectory\0"
    "thumbnailUpdated\0cancel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileWatcher[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       5,    1,   64,    2, 0x06 /* Public */,
       7,    1,   67,    2, 0x06 /* Public */,
       8,    1,   70,    2, 0x06 /* Public */,
       9,    1,   73,    2, 0x06 /* Public */,
      10,    1,   76,    2, 0x06 /* Public */,
      11,    0,   79,    2, 0x06 /* Public */,
      12,    1,   80,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Peony::FileWatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileWatcher *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->locationChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->directoryDeleted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->directoryUnmounted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->fileCreated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->fileDeleted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->fileChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->requestUpdateDirectory(); break;
        case 7: _t->thumbnailUpdated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->cancel(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileWatcher::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::locationChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileWatcher::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::directoryDeleted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileWatcher::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::directoryUnmounted)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FileWatcher::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::fileCreated)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (FileWatcher::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::fileDeleted)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (FileWatcher::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::fileChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (FileWatcher::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::requestUpdateDirectory)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (FileWatcher::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileWatcher::thumbnailUpdated)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileWatcher::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileWatcher.data,
    qt_meta_data_Peony__FileWatcher,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileWatcher.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void Peony::FileWatcher::locationChanged(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::FileWatcher::directoryDeleted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peony::FileWatcher::directoryUnmounted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Peony::FileWatcher::fileCreated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Peony::FileWatcher::fileDeleted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Peony::FileWatcher::fileChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Peony::FileWatcher::requestUpdateDirectory()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void Peony::FileWatcher::thumbnailUpdated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
