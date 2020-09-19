/****************************************************************************
** Meta object code from reading C++ file 'navigation-tool-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/navigation-bar/navigation-tool-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'navigation-tool-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__NavigationToolBar_t {
    QByteArrayData data[15];
    char stringdata0[208];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__NavigationToolBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__NavigationToolBar_t qt_meta_stringdata_Peony__NavigationToolBar = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Peony::NavigationToolBar"
QT_MOC_LITERAL(1, 25, 27), // "updateWindowLocationRequest"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 3), // "uri"
QT_MOC_LITERAL(4, 58, 10), // "addHistory"
QT_MOC_LITERAL(5, 69, 11), // "forceUpdate"
QT_MOC_LITERAL(6, 81, 14), // "refreshRequest"
QT_MOC_LITERAL(7, 96, 19), // "setCurrentContainer"
QT_MOC_LITERAL(8, 116, 23), // "DirectoryViewContainer*"
QT_MOC_LITERAL(9, 140, 9), // "container"
QT_MOC_LITERAL(10, 150, 13), // "updateActions"
QT_MOC_LITERAL(11, 164, 8), // "onGoBack"
QT_MOC_LITERAL(12, 173, 11), // "onGoForward"
QT_MOC_LITERAL(13, 185, 9), // "onGoToUri"
QT_MOC_LITERAL(14, 195, 12) // "clearHistory"

    },
    "Peony::NavigationToolBar\0"
    "updateWindowLocationRequest\0\0uri\0"
    "addHistory\0forceUpdate\0refreshRequest\0"
    "setCurrentContainer\0DirectoryViewContainer*\0"
    "container\0updateActions\0onGoBack\0"
    "onGoForward\0onGoToUri\0clearHistory"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__NavigationToolBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   64,    2, 0x06 /* Public */,
       1,    2,   71,    2, 0x26 /* Public | MethodCloned */,
       6,    0,   76,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   77,    2, 0x0a /* Public */,
      10,    0,   80,    2, 0x0a /* Public */,
      11,    0,   81,    2, 0x0a /* Public */,
      12,    0,   82,    2, 0x0a /* Public */,
      13,    3,   83,    2, 0x0a /* Public */,
      13,    2,   90,    2, 0x2a /* Public | MethodCloned */,
      14,    0,   95,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void,

       0        // eod
};

void Peony::NavigationToolBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NavigationToolBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->refreshRequest(); break;
        case 3: _t->setCurrentContainer((*reinterpret_cast< DirectoryViewContainer*(*)>(_a[1]))); break;
        case 4: _t->updateActions(); break;
        case 5: _t->onGoBack(); break;
        case 6: _t->onGoForward(); break;
        case 7: _t->onGoToUri((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 8: _t->onGoToUri((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->clearHistory(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NavigationToolBar::*)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationToolBar::updateWindowLocationRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NavigationToolBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationToolBar::refreshRequest)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::NavigationToolBar::staticMetaObject = { {
    &QToolBar::staticMetaObject,
    qt_meta_stringdata_Peony__NavigationToolBar.data,
    qt_meta_data_Peony__NavigationToolBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::NavigationToolBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::NavigationToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__NavigationToolBar.stringdata0))
        return static_cast<void*>(this);
    return QToolBar::qt_metacast(_clname);
}

int Peony::NavigationToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Peony::NavigationToolBar::updateWindowLocationRequest(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 2
void Peony::NavigationToolBar::refreshRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
