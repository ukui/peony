/****************************************************************************
** Meta object code from reading C++ file 'sort-type-menu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/sort-type-menu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sort-type-menu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SortTypeMenu_t {
    QByteArrayData data[9];
    char stringdata0[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SortTypeMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SortTypeMenu_t qt_meta_stringdata_SortTypeMenu = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SortTypeMenu"
QT_MOC_LITERAL(1, 13, 21), // "switchSortTypeRequest"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 4), // "type"
QT_MOC_LITERAL(4, 41, 22), // "switchSortOrderRequest"
QT_MOC_LITERAL(5, 64, 13), // "Qt::SortOrder"
QT_MOC_LITERAL(6, 78, 5), // "order"
QT_MOC_LITERAL(7, 84, 11), // "setSortType"
QT_MOC_LITERAL(8, 96, 12) // "setSortOrder"

    },
    "SortTypeMenu\0switchSortTypeRequest\0\0"
    "type\0switchSortOrderRequest\0Qt::SortOrder\0"
    "order\0setSortType\0setSortOrder"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SortTypeMenu[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   40,    2, 0x0a /* Public */,
       8,    1,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 5,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 5,    6,

       0        // eod
};

void SortTypeMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SortTypeMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->switchSortTypeRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->switchSortOrderRequest((*reinterpret_cast< Qt::SortOrder(*)>(_a[1]))); break;
        case 2: _t->setSortType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setSortOrder((*reinterpret_cast< Qt::SortOrder(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SortTypeMenu::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SortTypeMenu::switchSortTypeRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SortTypeMenu::*)(Qt::SortOrder );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SortTypeMenu::switchSortOrderRequest)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SortTypeMenu::staticMetaObject = { {
    &QMenu::staticMetaObject,
    qt_meta_stringdata_SortTypeMenu.data,
    qt_meta_data_SortTypeMenu,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SortTypeMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SortTypeMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SortTypeMenu.stringdata0))
        return static_cast<void*>(this);
    return QMenu::qt_metacast(_clname);
}

int SortTypeMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SortTypeMenu::switchSortTypeRequest(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SortTypeMenu::switchSortOrderRequest(Qt::SortOrder _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
