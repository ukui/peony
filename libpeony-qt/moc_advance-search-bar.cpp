/****************************************************************************
** Meta object code from reading C++ file 'advance-search-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/tool-bar/advance-search-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'advance-search-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__AdvanceSearchBar_t {
    QByteArrayData data[11];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__AdvanceSearchBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__AdvanceSearchBar_t qt_meta_stringdata_Peony__AdvanceSearchBar = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Peony::AdvanceSearchBar"
QT_MOC_LITERAL(1, 24, 9), // "clearData"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 10), // "browsePath"
QT_MOC_LITERAL(4, 46, 12), // "searchFilter"
QT_MOC_LITERAL(5, 59, 12), // "filterUpdate"
QT_MOC_LITERAL(6, 72, 13), // "setShowHidden"
QT_MOC_LITERAL(7, 86, 14), // "updateLocation"
QT_MOC_LITERAL(8, 101, 11), // "pathChanged"
QT_MOC_LITERAL(9, 113, 14), // "setdefaultpath"
QT_MOC_LITERAL(10, 128, 4) // "path"

    },
    "Peony::AdvanceSearchBar\0clearData\0\0"
    "browsePath\0searchFilter\0filterUpdate\0"
    "setShowHidden\0updateLocation\0pathChanged\0"
    "setdefaultpath\0path"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__AdvanceSearchBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x0a /* Public */,
       3,    0,   55,    2, 0x0a /* Public */,
       4,    0,   56,    2, 0x0a /* Public */,
       5,    0,   57,    2, 0x0a /* Public */,
       6,    0,   58,    2, 0x0a /* Public */,
       7,    0,   59,    2, 0x0a /* Public */,
       8,    0,   60,    2, 0x0a /* Public */,
       9,    1,   61,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void Peony::AdvanceSearchBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AdvanceSearchBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clearData(); break;
        case 1: _t->browsePath(); break;
        case 2: _t->searchFilter(); break;
        case 3: _t->filterUpdate(); break;
        case 4: _t->setShowHidden(); break;
        case 5: _t->updateLocation(); break;
        case 6: _t->pathChanged(); break;
        case 7: _t->setdefaultpath((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::AdvanceSearchBar::staticMetaObject = { {
    &QScrollArea::staticMetaObject,
    qt_meta_stringdata_Peony__AdvanceSearchBar.data,
    qt_meta_data_Peony__AdvanceSearchBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::AdvanceSearchBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::AdvanceSearchBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__AdvanceSearchBar.stringdata0))
        return static_cast<void*>(this);
    return QScrollArea::qt_metacast(_clname);
}

int Peony::AdvanceSearchBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
