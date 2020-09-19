/****************************************************************************
** Meta object code from reading C++ file 'side-bar-separator-item.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "model/side-bar-separator-item.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'side-bar-separator-item.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__SideBarSeparatorItem_t {
    QByteArrayData data[9];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__SideBarSeparatorItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__SideBarSeparatorItem_t qt_meta_stringdata_Peony__SideBarSeparatorItem = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Peony::SideBarSeparatorItem"
QT_MOC_LITERAL(1, 28, 5), // "eject"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 7), // "unmount"
QT_MOC_LITERAL(4, 43, 6), // "format"
QT_MOC_LITERAL(5, 50, 9), // "onUpdated"
QT_MOC_LITERAL(6, 60, 12), // "findChildren"
QT_MOC_LITERAL(7, 73, 17), // "findChildrenAsync"
QT_MOC_LITERAL(8, 91, 13) // "clearChildren"

    },
    "Peony::SideBarSeparatorItem\0eject\0\0"
    "unmount\0format\0onUpdated\0findChildren\0"
    "findChildrenAsync\0clearChildren"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__SideBarSeparatorItem[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x0a /* Public */,
       5,    0,   52,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x0a /* Public */,
       7,    0,   54,    2, 0x0a /* Public */,
       8,    0,   55,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Peony::SideBarSeparatorItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SideBarSeparatorItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->eject(); break;
        case 1: _t->unmount(); break;
        case 2: _t->format(); break;
        case 3: _t->onUpdated(); break;
        case 4: _t->findChildren(); break;
        case 5: _t->findChildrenAsync(); break;
        case 6: _t->clearChildren(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::SideBarSeparatorItem::staticMetaObject = { {
    &SideBarAbstractItem::staticMetaObject,
    qt_meta_stringdata_Peony__SideBarSeparatorItem.data,
    qt_meta_data_Peony__SideBarSeparatorItem,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::SideBarSeparatorItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::SideBarSeparatorItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__SideBarSeparatorItem.stringdata0))
        return static_cast<void*>(this);
    return SideBarAbstractItem::qt_metacast(_clname);
}

int Peony::SideBarSeparatorItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SideBarAbstractItem::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
