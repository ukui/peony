/****************************************************************************
** Meta object code from reading C++ file 'view-type-menu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/view-type-menu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'view-type-menu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ViewTypeMenu_t {
    QByteArrayData data[12];
    char stringdata0[157];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewTypeMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewTypeMenu_t qt_meta_stringdata_ViewTypeMenu = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ViewTypeMenu"
QT_MOC_LITERAL(1, 13, 17), // "switchViewRequest"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 6), // "viewId"
QT_MOC_LITERAL(4, 39, 4), // "icon"
QT_MOC_LITERAL(5, 44, 20), // "resetToZoomLevelHint"
QT_MOC_LITERAL(6, 65, 26), // "updateZoomLevelHintRequest"
QT_MOC_LITERAL(7, 92, 13), // "zoomLevelHint"
QT_MOC_LITERAL(8, 106, 14), // "setCurrentView"
QT_MOC_LITERAL(9, 121, 11), // "blockSignal"
QT_MOC_LITERAL(10, 133, 19), // "setCurrentDirectory"
QT_MOC_LITERAL(11, 153, 3) // "uri"

    },
    "ViewTypeMenu\0switchViewRequest\0\0viewId\0"
    "icon\0resetToZoomLevelHint\0"
    "updateZoomLevelHintRequest\0zoomLevelHint\0"
    "setCurrentView\0blockSignal\0"
    "setCurrentDirectory\0uri"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewTypeMenu[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   44,    2, 0x06 /* Public */,
       1,    2,   51,    2, 0x26 /* Public | MethodCloned */,
       6,    1,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,   59,    2, 0x0a /* Public */,
       8,    1,   64,    2, 0x2a /* Public | MethodCloned */,
      10,    1,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QIcon, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QIcon,    3,    4,
    QMetaType::Void, QMetaType::Int,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    9,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,   11,

       0        // eod
};

void ViewTypeMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ViewTypeMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->switchViewRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QIcon(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->switchViewRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QIcon(*)>(_a[2]))); break;
        case 2: _t->updateZoomLevelHintRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setCurrentView((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->setCurrentView((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->setCurrentDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ViewTypeMenu::*)(const QString & , const QIcon & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewTypeMenu::switchViewRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ViewTypeMenu::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ViewTypeMenu::updateZoomLevelHintRequest)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ViewTypeMenu::staticMetaObject = { {
    &QMenu::staticMetaObject,
    qt_meta_stringdata_ViewTypeMenu.data,
    qt_meta_data_ViewTypeMenu,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ViewTypeMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewTypeMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ViewTypeMenu.stringdata0))
        return static_cast<void*>(this);
    return QMenu::qt_metacast(_clname);
}

int ViewTypeMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ViewTypeMenu::switchViewRequest(const QString & _t1, const QIcon & _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 2
void ViewTypeMenu::updateZoomLevelHintRequest(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
