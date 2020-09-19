/****************************************************************************
** Meta object code from reading C++ file 'tab-status-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/tab-status-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tab-status-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TabStatusBar_t {
    QByteArrayData data[9];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TabStatusBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TabStatusBar_t qt_meta_stringdata_TabStatusBar = {
    {
QT_MOC_LITERAL(0, 0, 12), // "TabStatusBar"
QT_MOC_LITERAL(1, 13, 23), // "zoomLevelChangedRequest"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 9), // "zoomLevel"
QT_MOC_LITERAL(4, 48, 6), // "update"
QT_MOC_LITERAL(5, 55, 7), // "message"
QT_MOC_LITERAL(6, 63, 20), // "updateZoomLevelState"
QT_MOC_LITERAL(7, 84, 13), // "onZoomRequest"
QT_MOC_LITERAL(8, 98, 6) // "zoomIn"

    },
    "TabStatusBar\0zoomLevelChangedRequest\0"
    "\0zoomLevel\0update\0message\0"
    "updateZoomLevelState\0onZoomRequest\0"
    "zoomIn"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TabStatusBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   42,    2, 0x0a /* Public */,
       4,    1,   43,    2, 0x0a /* Public */,
       6,    1,   46,    2, 0x0a /* Public */,
       7,    1,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool,    8,

       0        // eod
};

void TabStatusBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TabStatusBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->zoomLevelChangedRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->update(); break;
        case 2: _t->update((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->updateZoomLevelState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onZoomRequest((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TabStatusBar::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabStatusBar::zoomLevelChangedRequest)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TabStatusBar::staticMetaObject = { {
    &QStatusBar::staticMetaObject,
    qt_meta_stringdata_TabStatusBar.data,
    qt_meta_data_TabStatusBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TabStatusBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabStatusBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabStatusBar.stringdata0))
        return static_cast<void*>(this);
    return QStatusBar::qt_metacast(_clname);
}

int TabStatusBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStatusBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void TabStatusBar::zoomLevelChangedRequest(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ElidedLabel_t {
    QByteArrayData data[1];
    char stringdata0[12];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ElidedLabel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ElidedLabel_t qt_meta_stringdata_ElidedLabel = {
    {
QT_MOC_LITERAL(0, 0, 11) // "ElidedLabel"

    },
    "ElidedLabel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ElidedLabel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ElidedLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ElidedLabel::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ElidedLabel.data,
    qt_meta_data_ElidedLabel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ElidedLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElidedLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ElidedLabel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ElidedLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
