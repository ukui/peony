/****************************************************************************
** Meta object code from reading C++ file 'search-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/tool-bar/search-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'search-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__SearchBar_t {
    QByteArrayData data[14];
    char stringdata0[168];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__SearchBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__SearchBar_t qt_meta_stringdata_Peony__SearchBar = {
    {
QT_MOC_LITERAL(0, 0, 16), // "Peony::SearchBar"
QT_MOC_LITERAL(1, 17, 16), // "searchKeyChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 3), // "key"
QT_MOC_LITERAL(4, 39, 13), // "searchRequest"
QT_MOC_LITERAL(5, 53, 4), // "init"
QT_MOC_LITERAL(6, 58, 12), // "hasTopWindow"
QT_MOC_LITERAL(7, 71, 14), // "initTableModel"
QT_MOC_LITERAL(8, 86, 16), // "updateTableModel"
QT_MOC_LITERAL(9, 103, 14), // "onTableClicked"
QT_MOC_LITERAL(10, 118, 11), // "QModelIndex"
QT_MOC_LITERAL(11, 130, 5), // "index"
QT_MOC_LITERAL(12, 136, 17), // "clearSearchRecord"
QT_MOC_LITERAL(13, 154, 13) // "hideTableView"

    },
    "Peony::SearchBar\0searchKeyChanged\0\0"
    "key\0searchRequest\0init\0hasTopWindow\0"
    "initTableModel\0updateTableModel\0"
    "onTableClicked\0QModelIndex\0index\0"
    "clearSearchRecord\0hideTableView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__SearchBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   60,    2, 0x0a /* Public */,
       7,    0,   63,    2, 0x0a /* Public */,
       8,    0,   64,    2, 0x0a /* Public */,
       9,    1,   65,    2, 0x0a /* Public */,
      12,    0,   68,    2, 0x0a /* Public */,
      13,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Peony::SearchBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SearchBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->searchKeyChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->searchRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->init((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->initTableModel(); break;
        case 4: _t->updateTableModel(); break;
        case 5: _t->onTableClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: _t->clearSearchRecord(); break;
        case 7: _t->hideTableView(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SearchBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SearchBar::searchKeyChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SearchBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SearchBar::searchRequest)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::SearchBar::staticMetaObject = { {
    &QLineEdit::staticMetaObject,
    qt_meta_stringdata_Peony__SearchBar.data,
    qt_meta_data_Peony__SearchBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::SearchBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::SearchBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__SearchBar.stringdata0))
        return static_cast<void*>(this);
    return QLineEdit::qt_metacast(_clname);
}

int Peony::SearchBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Peony::SearchBar::searchKeyChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::SearchBar::searchRequest(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
