/****************************************************************************
** Meta object code from reading C++ file 'view-factory-sort-filter-model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/tool-bar/view-factory-sort-filter-model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'view-factory-sort-filter-model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__ViewFactorySortFilterModel2_t {
    QByteArrayData data[4];
    char stringdata0[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__ViewFactorySortFilterModel2_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__ViewFactorySortFilterModel2_t qt_meta_stringdata_Peony__ViewFactorySortFilterModel2 = {
    {
QT_MOC_LITERAL(0, 0, 34), // "Peony::ViewFactorySortFilterM..."
QT_MOC_LITERAL(1, 35, 15), // "setDirectoryUri"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 3) // "uri"

    },
    "Peony::ViewFactorySortFilterModel2\0"
    "setDirectoryUri\0\0uri"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__ViewFactorySortFilterModel2[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Peony::ViewFactorySortFilterModel2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ViewFactorySortFilterModel2 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setDirectoryUri((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::ViewFactorySortFilterModel2::staticMetaObject = { {
    &QSortFilterProxyModel::staticMetaObject,
    qt_meta_stringdata_Peony__ViewFactorySortFilterModel2.data,
    qt_meta_data_Peony__ViewFactorySortFilterModel2,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::ViewFactorySortFilterModel2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::ViewFactorySortFilterModel2::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__ViewFactorySortFilterModel2.stringdata0))
        return static_cast<void*>(this);
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int Peony::ViewFactorySortFilterModel2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
