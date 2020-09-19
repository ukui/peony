/****************************************************************************
** Meta object code from reading C++ file 'file-item.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "model/file-item.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-item.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileItem_t {
    QByteArrayData data[17];
    char stringdata0[189];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileItem_t qt_meta_stringdata_Peony__FileItem = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Peony::FileItem"
QT_MOC_LITERAL(1, 16, 18), // "cancelFindChildren"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 10), // "childAdded"
QT_MOC_LITERAL(4, 47, 3), // "uri"
QT_MOC_LITERAL(5, 51, 12), // "childRemoved"
QT_MOC_LITERAL(6, 64, 7), // "deleted"
QT_MOC_LITERAL(7, 72, 7), // "thisUri"
QT_MOC_LITERAL(8, 80, 7), // "renamed"
QT_MOC_LITERAL(9, 88, 6), // "oldUri"
QT_MOC_LITERAL(10, 95, 6), // "newUri"
QT_MOC_LITERAL(11, 102, 12), // "onChildAdded"
QT_MOC_LITERAL(12, 115, 14), // "onChildRemoved"
QT_MOC_LITERAL(13, 130, 9), // "onDeleted"
QT_MOC_LITERAL(14, 140, 9), // "onRenamed"
QT_MOC_LITERAL(15, 150, 24), // "onUpdateDirectoryRequest"
QT_MOC_LITERAL(16, 175, 13) // "clearChildren"

    },
    "Peony::FileItem\0cancelFindChildren\0\0"
    "childAdded\0uri\0childRemoved\0deleted\0"
    "thisUri\0renamed\0oldUri\0newUri\0"
    "onChildAdded\0onChildRemoved\0onDeleted\0"
    "onRenamed\0onUpdateDirectoryRequest\0"
    "clearChildren"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileItem[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    1,   70,    2, 0x06 /* Public */,
       5,    1,   73,    2, 0x06 /* Public */,
       6,    1,   76,    2, 0x06 /* Public */,
       8,    2,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   84,    2, 0x0a /* Public */,
      12,    1,   87,    2, 0x0a /* Public */,
      13,    1,   90,    2, 0x0a /* Public */,
      14,    2,   93,    2, 0x0a /* Public */,
      15,    0,   98,    2, 0x0a /* Public */,
      16,    0,   99,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Peony::FileItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelFindChildren(); break;
        case 1: _t->childAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->childRemoved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->deleted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->renamed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->onChildAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onChildRemoved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onDeleted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onRenamed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 9: _t->onUpdateDirectoryRequest(); break;
        case 10: _t->clearChildren(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItem::cancelFindChildren)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileItem::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItem::childAdded)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileItem::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItem::childRemoved)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FileItem::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItem::deleted)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (FileItem::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItem::renamed)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileItem::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileItem.data,
    qt_meta_data_Peony__FileItem,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileItem.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Peony::FileItem::cancelFindChildren()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Peony::FileItem::childAdded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peony::FileItem::childRemoved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Peony::FileItem::deleted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Peony::FileItem::renamed(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
