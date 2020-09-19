/****************************************************************************
** Meta object code from reading C++ file 'side-bar-abstract-item.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "model/side-bar-abstract-item.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'side-bar-abstract-item.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__SideBarAbstractItem_t {
    QByteArrayData data[12];
    char stringdata0[148];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__SideBarAbstractItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__SideBarAbstractItem_t qt_meta_stringdata_Peony__SideBarAbstractItem = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Peony::SideBarAbstractItem"
QT_MOC_LITERAL(1, 27, 20), // "findChildrenFinished"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 7), // "updated"
QT_MOC_LITERAL(4, 57, 9), // "onUpdated"
QT_MOC_LITERAL(5, 67, 5), // "eject"
QT_MOC_LITERAL(6, 73, 7), // "unmount"
QT_MOC_LITERAL(7, 81, 6), // "format"
QT_MOC_LITERAL(8, 88, 14), // "ejectOrUnmount"
QT_MOC_LITERAL(9, 103, 12), // "findChildren"
QT_MOC_LITERAL(10, 116, 17), // "findChildrenAsync"
QT_MOC_LITERAL(11, 134, 13) // "clearChildren"

    },
    "Peony::SideBarAbstractItem\0"
    "findChildrenFinished\0\0updated\0onUpdated\0"
    "eject\0unmount\0format\0ejectOrUnmount\0"
    "findChildren\0findChildrenAsync\0"
    "clearChildren"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__SideBarAbstractItem[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   66,    2, 0x0a /* Public */,
       5,    0,   67,    2, 0x0a /* Public */,
       6,    0,   68,    2, 0x0a /* Public */,
       7,    0,   69,    2, 0x0a /* Public */,
       8,    0,   70,    2, 0x0a /* Public */,
       9,    0,   71,    2, 0x0a /* Public */,
      10,    0,   72,    2, 0x0a /* Public */,
      11,    0,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Peony::SideBarAbstractItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SideBarAbstractItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->findChildrenFinished(); break;
        case 1: _t->updated(); break;
        case 2: _t->onUpdated(); break;
        case 3: _t->eject(); break;
        case 4: _t->unmount(); break;
        case 5: _t->format(); break;
        case 6: _t->ejectOrUnmount(); break;
        case 7: _t->findChildren(); break;
        case 8: _t->findChildrenAsync(); break;
        case 9: _t->clearChildren(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SideBarAbstractItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SideBarAbstractItem::findChildrenFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SideBarAbstractItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SideBarAbstractItem::updated)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::SideBarAbstractItem::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__SideBarAbstractItem.data,
    qt_meta_data_Peony__SideBarAbstractItem,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::SideBarAbstractItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::SideBarAbstractItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__SideBarAbstractItem.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::SideBarAbstractItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Peony::SideBarAbstractItem::findChildrenFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Peony::SideBarAbstractItem::updated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
