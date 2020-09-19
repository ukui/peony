/****************************************************************************
** Meta object code from reading C++ file 'fm-dbus-service.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "fm-dbus-service.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fm-dbus-service.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FMDBusService_t {
    QByteArrayData data[12];
    char stringdata0[187];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FMDBusService_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FMDBusService_t qt_meta_stringdata_Peony__FMDBusService = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Peony::FMDBusService"
QT_MOC_LITERAL(1, 21, 15), // "D-Bus Interface"
QT_MOC_LITERAL(2, 37, 28), // "org.freedesktop.FileManager1"
QT_MOC_LITERAL(3, 66, 17), // "showFolderRequest"
QT_MOC_LITERAL(4, 84, 0), // ""
QT_MOC_LITERAL(5, 85, 7), // "uriList"
QT_MOC_LITERAL(6, 93, 9), // "startUpId"
QT_MOC_LITERAL(7, 103, 16), // "showItemsRequest"
QT_MOC_LITERAL(8, 120, 25), // "showItemPropertiesRequest"
QT_MOC_LITERAL(9, 146, 11), // "ShowFolders"
QT_MOC_LITERAL(10, 158, 9), // "ShowItems"
QT_MOC_LITERAL(11, 168, 18) // "ShowItemProperties"

    },
    "Peony::FMDBusService\0D-Bus Interface\0"
    "org.freedesktop.FileManager1\0"
    "showFolderRequest\0\0uriList\0startUpId\0"
    "showItemsRequest\0showItemPropertiesRequest\0"
    "ShowFolders\0ShowItems\0ShowItemProperties"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FMDBusService[] = {

 // content:
       8,       // revision
       0,       // classname
       1,   14, // classinfo
       6,   16, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // classinfo: key, value
       1,    2,

 // signals: name, argc, parameters, tag, flags
       3,    2,   46,    4, 0x06 /* Public */,
       7,    2,   51,    4, 0x06 /* Public */,
       8,    2,   56,    4, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       9,    2,   61,    4, 0x42 /* Public | isScriptable */,
      10,    2,   66,    4, 0x42 /* Public | isScriptable */,
      11,    2,   71,    4, 0x42 /* Public | isScriptable */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    5,    6,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    5,    6,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    5,    6,

 // methods: parameters
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    5,    6,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    5,    6,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    5,    6,

       0        // eod
};

void Peony::FMDBusService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FMDBusService *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showFolderRequest((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->showItemsRequest((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->showItemPropertiesRequest((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->ShowFolders((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->ShowItems((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->ShowItemProperties((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FMDBusService::*)(const QStringList & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FMDBusService::showFolderRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FMDBusService::*)(const QStringList & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FMDBusService::showItemsRequest)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FMDBusService::*)(const QStringList & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FMDBusService::showItemPropertiesRequest)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FMDBusService::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FMDBusService.data,
    qt_meta_data_Peony__FMDBusService,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FMDBusService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FMDBusService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FMDBusService.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FMDBusService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Peony::FMDBusService::showFolderRequest(const QStringList & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::FMDBusService::showItemsRequest(const QStringList & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peony::FMDBusService::showItemPropertiesRequest(const QStringList & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
