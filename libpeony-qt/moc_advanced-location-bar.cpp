/****************************************************************************
** Meta object code from reading C++ file 'advanced-location-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/navigation-bar/advanced-location-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'advanced-location-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__AdvancedLocationBar_t {
    QByteArrayData data[18];
    char stringdata0[226];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__AdvancedLocationBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__AdvancedLocationBar_t qt_meta_stringdata_Peony__AdvancedLocationBar = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Peony::AdvancedLocationBar"
QT_MOC_LITERAL(1, 27, 27), // "updateWindowLocationRequest"
QT_MOC_LITERAL(2, 55, 0), // ""
QT_MOC_LITERAL(3, 56, 3), // "uri"
QT_MOC_LITERAL(4, 60, 10), // "addHistory"
QT_MOC_LITERAL(5, 71, 11), // "forceUpdate"
QT_MOC_LITERAL(6, 83, 14), // "refreshRequest"
QT_MOC_LITERAL(7, 98, 13), // "searchRequest"
QT_MOC_LITERAL(8, 112, 4), // "path"
QT_MOC_LITERAL(9, 117, 3), // "key"
QT_MOC_LITERAL(10, 121, 20), // "updateFileTypeFilter"
QT_MOC_LITERAL(11, 142, 5), // "index"
QT_MOC_LITERAL(12, 148, 14), // "updateLocation"
QT_MOC_LITERAL(13, 163, 9), // "startEdit"
QT_MOC_LITERAL(14, 173, 10), // "finishEdit"
QT_MOC_LITERAL(15, 184, 14), // "switchEditMode"
QT_MOC_LITERAL(16, 199, 11), // "bSearchMode"
QT_MOC_LITERAL(17, 211, 14) // "clearSearchBox"

    },
    "Peony::AdvancedLocationBar\0"
    "updateWindowLocationRequest\0\0uri\0"
    "addHistory\0forceUpdate\0refreshRequest\0"
    "searchRequest\0path\0key\0updateFileTypeFilter\0"
    "index\0updateLocation\0startEdit\0"
    "finishEdit\0switchEditMode\0bSearchMode\0"
    "clearSearchBox"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__AdvancedLocationBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   69,    2, 0x06 /* Public */,
       1,    2,   76,    2, 0x26 /* Public | MethodCloned */,
       1,    1,   81,    2, 0x26 /* Public | MethodCloned */,
       6,    0,   84,    2, 0x06 /* Public */,
       7,    2,   85,    2, 0x06 /* Public */,
      10,    1,   90,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   93,    2, 0x0a /* Public */,
      13,    0,   96,    2, 0x0a /* Public */,
      14,    0,   97,    2, 0x0a /* Public */,
      15,    1,   98,    2, 0x0a /* Public */,
      17,    0,  101,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::Int,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void,

       0        // eod
};

void Peony::AdvancedLocationBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AdvancedLocationBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->refreshRequest(); break;
        case 4: _t->searchRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->updateFileTypeFilter((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 6: _t->updateLocation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->startEdit(); break;
        case 8: _t->finishEdit(); break;
        case 9: _t->switchEditMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->clearSearchBox(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AdvancedLocationBar::*)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AdvancedLocationBar::updateWindowLocationRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AdvancedLocationBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AdvancedLocationBar::refreshRequest)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (AdvancedLocationBar::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AdvancedLocationBar::searchRequest)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (AdvancedLocationBar::*)(const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AdvancedLocationBar::updateFileTypeFilter)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::AdvancedLocationBar::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_Peony__AdvancedLocationBar.data,
    qt_meta_data_Peony__AdvancedLocationBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::AdvancedLocationBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::AdvancedLocationBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__AdvancedLocationBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Peony::AdvancedLocationBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void Peony::AdvancedLocationBar::updateWindowLocationRequest(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 3
void Peony::AdvancedLocationBar::refreshRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Peony::AdvancedLocationBar::searchRequest(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Peony::AdvancedLocationBar::updateFileTypeFilter(const int & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
