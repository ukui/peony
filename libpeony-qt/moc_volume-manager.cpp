/****************************************************************************
** Meta object code from reading C++ file 'volume-manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "volume-manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'volume-manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__VolumeManager_t {
    QByteArrayData data[16];
    char stringdata0[206];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__VolumeManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__VolumeManager_t qt_meta_stringdata_Peony__VolumeManager = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Peony::VolumeManager"
QT_MOC_LITERAL(1, 21, 14), // "driveConnected"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 22), // "std::shared_ptr<Drive>"
QT_MOC_LITERAL(4, 60, 5), // "drive"
QT_MOC_LITERAL(5, 66, 17), // "driveDisconnected"
QT_MOC_LITERAL(6, 84, 11), // "volumeAdded"
QT_MOC_LITERAL(7, 96, 23), // "std::shared_ptr<Volume>"
QT_MOC_LITERAL(8, 120, 6), // "volume"
QT_MOC_LITERAL(9, 127, 13), // "volumeRemoved"
QT_MOC_LITERAL(10, 141, 10), // "mountAdded"
QT_MOC_LITERAL(11, 152, 22), // "std::shared_ptr<Mount>"
QT_MOC_LITERAL(12, 175, 5), // "mount"
QT_MOC_LITERAL(13, 181, 12), // "mountRemoved"
QT_MOC_LITERAL(14, 194, 7), // "unmount"
QT_MOC_LITERAL(15, 202, 3) // "uri"

    },
    "Peony::VolumeManager\0driveConnected\0"
    "\0std::shared_ptr<Drive>\0drive\0"
    "driveDisconnected\0volumeAdded\0"
    "std::shared_ptr<Volume>\0volume\0"
    "volumeRemoved\0mountAdded\0"
    "std::shared_ptr<Mount>\0mount\0mountRemoved\0"
    "unmount\0uri"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__VolumeManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    1,   52,    2, 0x06 /* Public */,
       6,    1,   55,    2, 0x06 /* Public */,
       9,    1,   58,    2, 0x06 /* Public */,
      10,    1,   61,    2, 0x06 /* Public */,
      13,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    1,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 11,   12,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

void Peony::VolumeManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VolumeManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->driveConnected((*reinterpret_cast< const std::shared_ptr<Drive>(*)>(_a[1]))); break;
        case 1: _t->driveDisconnected((*reinterpret_cast< const std::shared_ptr<Drive>(*)>(_a[1]))); break;
        case 2: _t->volumeAdded((*reinterpret_cast< const std::shared_ptr<Volume>(*)>(_a[1]))); break;
        case 3: _t->volumeRemoved((*reinterpret_cast< const std::shared_ptr<Volume>(*)>(_a[1]))); break;
        case 4: _t->mountAdded((*reinterpret_cast< const std::shared_ptr<Mount>(*)>(_a[1]))); break;
        case 5: _t->mountRemoved((*reinterpret_cast< const std::shared_ptr<Mount>(*)>(_a[1]))); break;
        case 6: _t->unmount((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VolumeManager::*)(const std::shared_ptr<Drive> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeManager::driveConnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VolumeManager::*)(const std::shared_ptr<Drive> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeManager::driveDisconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (VolumeManager::*)(const std::shared_ptr<Volume> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeManager::volumeAdded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (VolumeManager::*)(const std::shared_ptr<Volume> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeManager::volumeRemoved)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (VolumeManager::*)(const std::shared_ptr<Mount> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeManager::mountAdded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (VolumeManager::*)(const std::shared_ptr<Mount> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VolumeManager::mountRemoved)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::VolumeManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__VolumeManager.data,
    qt_meta_data_Peony__VolumeManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::VolumeManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::VolumeManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__VolumeManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::VolumeManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Peony::VolumeManager::driveConnected(const std::shared_ptr<Drive> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::VolumeManager::driveDisconnected(const std::shared_ptr<Drive> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peony::VolumeManager::volumeAdded(const std::shared_ptr<Volume> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Peony::VolumeManager::volumeRemoved(const std::shared_ptr<Volume> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Peony::VolumeManager::mountAdded(const std::shared_ptr<Mount> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Peony::VolumeManager::mountRemoved(const std::shared_ptr<Mount> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
