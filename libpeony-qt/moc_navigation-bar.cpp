/****************************************************************************
** Meta object code from reading C++ file 'navigation-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/navigation-bar/navigation-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'navigation-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__NavigationBar_t {
    QByteArrayData data[18];
    char stringdata0[233];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__NavigationBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__NavigationBar_t qt_meta_stringdata_Peony__NavigationBar = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Peony::NavigationBar"
QT_MOC_LITERAL(1, 21, 27), // "updateWindowLocationRequest"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 3), // "uri"
QT_MOC_LITERAL(4, 54, 10), // "addHistory"
QT_MOC_LITERAL(5, 65, 11), // "forceUpdate"
QT_MOC_LITERAL(6, 77, 14), // "refreshRequest"
QT_MOC_LITERAL(7, 92, 24), // "switchPreviewPageRequest"
QT_MOC_LITERAL(8, 117, 2), // "id"
QT_MOC_LITERAL(9, 120, 13), // "bindContainer"
QT_MOC_LITERAL(10, 134, 23), // "DirectoryViewContainer*"
QT_MOC_LITERAL(11, 158, 9), // "container"
QT_MOC_LITERAL(12, 168, 14), // "updateLocation"
QT_MOC_LITERAL(13, 183, 8), // "setBlock"
QT_MOC_LITERAL(14, 192, 5), // "block"
QT_MOC_LITERAL(15, 198, 9), // "startEdit"
QT_MOC_LITERAL(16, 208, 10), // "finishEdit"
QT_MOC_LITERAL(17, 219, 13) // "triggerAction"

    },
    "Peony::NavigationBar\0updateWindowLocationRequest\0"
    "\0uri\0addHistory\0forceUpdate\0refreshRequest\0"
    "switchPreviewPageRequest\0id\0bindContainer\0"
    "DirectoryViewContainer*\0container\0"
    "updateLocation\0setBlock\0block\0startEdit\0"
    "finishEdit\0triggerAction"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__NavigationBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   69,    2, 0x06 /* Public */,
       1,    2,   76,    2, 0x26 /* Public | MethodCloned */,
       6,    0,   81,    2, 0x06 /* Public */,
       7,    1,   82,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   85,    2, 0x0a /* Public */,
      12,    1,   88,    2, 0x0a /* Public */,
      13,    1,   91,    2, 0x0a /* Public */,
      13,    0,   94,    2, 0x2a /* Public | MethodCloned */,
      15,    0,   95,    2, 0x0a /* Public */,
      16,    0,   96,    2, 0x0a /* Public */,
      17,    1,   97,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,

       0        // eod
};

void Peony::NavigationBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NavigationBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->refreshRequest(); break;
        case 3: _t->switchPreviewPageRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->bindContainer((*reinterpret_cast< DirectoryViewContainer*(*)>(_a[1]))); break;
        case 5: _t->updateLocation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->setBlock((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->setBlock(); break;
        case 8: _t->startEdit(); break;
        case 9: _t->finishEdit(); break;
        case 10: _t->triggerAction((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NavigationBar::*)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationBar::updateWindowLocationRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NavigationBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationBar::refreshRequest)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NavigationBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationBar::switchPreviewPageRequest)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::NavigationBar::staticMetaObject = { {
    &QToolBar::staticMetaObject,
    qt_meta_stringdata_Peony__NavigationBar.data,
    qt_meta_data_Peony__NavigationBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::NavigationBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::NavigationBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__NavigationBar.stringdata0))
        return static_cast<void*>(this);
    return QToolBar::qt_metacast(_clname);
}

int Peony::NavigationBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
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
void Peony::NavigationBar::updateWindowLocationRequest(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 2
void Peony::NavigationBar::refreshRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Peony::NavigationBar::switchPreviewPageRequest(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
