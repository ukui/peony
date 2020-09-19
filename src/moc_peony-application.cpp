/****************************************************************************
** Meta object code from reading C++ file 'peony-application.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "peony-application.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'peony-application.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PeonyApplication_t {
    QByteArrayData data[5];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PeonyApplication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PeonyApplication_t qt_meta_stringdata_PeonyApplication = {
    {
QT_MOC_LITERAL(0, 0, 16), // "PeonyApplication"
QT_MOC_LITERAL(1, 17, 8), // "parseCmd"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 2), // "id"
QT_MOC_LITERAL(4, 30, 3) // "msg"

    },
    "PeonyApplication\0parseCmd\0\0id\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PeonyApplication[] = {

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
       1,    2,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QByteArray,    3,    4,

       0        // eod
};

void PeonyApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PeonyApplication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->parseCmd((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PeonyApplication::staticMetaObject = { {
    &SingleApplication::staticMetaObject,
    qt_meta_stringdata_PeonyApplication.data,
    qt_meta_data_PeonyApplication,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PeonyApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PeonyApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PeonyApplication.stringdata0))
        return static_cast<void*>(this);
    return SingleApplication::qt_metacast(_clname);
}

int PeonyApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SingleApplication::qt_metacall(_c, _id, _a);
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
