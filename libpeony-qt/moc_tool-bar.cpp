/****************************************************************************
** Meta object code from reading C++ file 'tool-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/tool-bar/tool-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tool-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__ToolBar_t {
    QByteArrayData data[22];
    char stringdata0[209];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__ToolBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__ToolBar_t qt_meta_stringdata_Peony__ToolBar = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Peony::ToolBar"
QT_MOC_LITERAL(1, 15, 13), // "optionRequest"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 11), // "RequestType"
QT_MOC_LITERAL(4, 42, 4), // "type"
QT_MOC_LITERAL(5, 47, 21), // "updateLocationRequest"
QT_MOC_LITERAL(6, 69, 3), // "uri"
QT_MOC_LITERAL(7, 73, 14), // "updateLocation"
QT_MOC_LITERAL(8, 88, 12), // "updateStates"
QT_MOC_LITERAL(9, 101, 6), // "Ivalid"
QT_MOC_LITERAL(10, 108, 15), // "OpenInNewWindow"
QT_MOC_LITERAL(11, 124, 12), // "OpenInNewTab"
QT_MOC_LITERAL(12, 137, 14), // "OpenInTerminal"
QT_MOC_LITERAL(13, 152, 10), // "SwitchView"
QT_MOC_LITERAL(14, 163, 4), // "Copy"
QT_MOC_LITERAL(15, 168, 5), // "Paste"
QT_MOC_LITERAL(16, 174, 3), // "Cut"
QT_MOC_LITERAL(17, 178, 5), // "Trash"
QT_MOC_LITERAL(18, 184, 5), // "Share"
QT_MOC_LITERAL(19, 190, 4), // "Burn"
QT_MOC_LITERAL(20, 195, 7), // "Archive"
QT_MOC_LITERAL(21, 203, 5) // "Other"

    },
    "Peony::ToolBar\0optionRequest\0\0RequestType\0"
    "type\0updateLocationRequest\0uri\0"
    "updateLocation\0updateStates\0Ivalid\0"
    "OpenInNewWindow\0OpenInNewTab\0"
    "OpenInTerminal\0SwitchView\0Copy\0Paste\0"
    "Cut\0Trash\0Share\0Burn\0Archive\0Other"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__ToolBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       1,   44, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   40,    2, 0x0a /* Public */,
       8,    0,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,

 // enums: name, alias, flags, count, data
       3,    3, 0x0,   13,   49,

 // enum data: key, value
       9, uint(Peony::ToolBar::Ivalid),
      10, uint(Peony::ToolBar::OpenInNewWindow),
      11, uint(Peony::ToolBar::OpenInNewTab),
      12, uint(Peony::ToolBar::OpenInTerminal),
      13, uint(Peony::ToolBar::SwitchView),
      14, uint(Peony::ToolBar::Copy),
      15, uint(Peony::ToolBar::Paste),
      16, uint(Peony::ToolBar::Cut),
      17, uint(Peony::ToolBar::Trash),
      18, uint(Peony::ToolBar::Share),
      19, uint(Peony::ToolBar::Burn),
      20, uint(Peony::ToolBar::Archive),
      21, uint(Peony::ToolBar::Other),

       0        // eod
};

void Peony::ToolBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ToolBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->optionRequest((*reinterpret_cast< const RequestType(*)>(_a[1]))); break;
        case 1: _t->updateLocationRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->updateLocation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->updateStates(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ToolBar::*)(const RequestType & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolBar::optionRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ToolBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolBar::updateLocationRequest)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::ToolBar::staticMetaObject = { {
    &QToolBar::staticMetaObject,
    qt_meta_stringdata_Peony__ToolBar.data,
    qt_meta_data_Peony__ToolBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::ToolBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::ToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__ToolBar.stringdata0))
        return static_cast<void*>(this);
    return QToolBar::qt_metacast(_clname);
}

int Peony::ToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
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
void Peony::ToolBar::optionRequest(const RequestType & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::ToolBar::updateLocationRequest(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
