/****************************************************************************
** Meta object code from reading C++ file 'format_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "windows/format_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'format_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Format_Dialog_t {
    QByteArrayData data[5];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Format_Dialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Format_Dialog_t qt_meta_stringdata_Format_Dialog = {
    {
QT_MOC_LITERAL(0, 0, 13), // "Format_Dialog"
QT_MOC_LITERAL(1, 14, 12), // "acceptFormat"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 11), // "colseFormat"
QT_MOC_LITERAL(4, 40, 10) // "formatloop"

    },
    "Format_Dialog\0acceptFormat\0\0colseFormat\0"
    "formatloop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Format_Dialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       3,    1,   32,    2, 0x0a /* Public */,
       4,    0,   35,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

       0        // eod
};

void Format_Dialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Format_Dialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->acceptFormat((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->colseFormat((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->formatloop(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Format_Dialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_Format_Dialog.data,
    qt_meta_data_Format_Dialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Format_Dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Format_Dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Format_Dialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Format_Dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
