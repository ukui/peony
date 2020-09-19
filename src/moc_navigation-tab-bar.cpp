/****************************************************************************
** Meta object code from reading C++ file 'navigation-tab-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/navigation-tab-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'navigation-tab-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NavigationTabBar_t {
    QByteArrayData data[13];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NavigationTabBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NavigationTabBar_t qt_meta_stringdata_NavigationTabBar = {
    {
QT_MOC_LITERAL(0, 0, 16), // "NavigationTabBar"
QT_MOC_LITERAL(1, 17, 9), // "pageAdded"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 3), // "uri"
QT_MOC_LITERAL(4, 32, 18), // "closeWindowRequest"
QT_MOC_LITERAL(5, 51, 14), // "addPageRequest"
QT_MOC_LITERAL(6, 66, 6), // "jumpTo"
QT_MOC_LITERAL(7, 73, 15), // "locationUpdated"
QT_MOC_LITERAL(8, 89, 7), // "addPage"
QT_MOC_LITERAL(9, 97, 12), // "jumpToNewTab"
QT_MOC_LITERAL(10, 110, 8), // "addPages"
QT_MOC_LITERAL(11, 119, 14), // "updateLocation"
QT_MOC_LITERAL(12, 134, 5) // "index"

    },
    "NavigationTabBar\0pageAdded\0\0uri\0"
    "closeWindowRequest\0addPageRequest\0"
    "jumpTo\0locationUpdated\0addPage\0"
    "jumpToNewTab\0addPages\0updateLocation\0"
    "index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NavigationTabBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    0,   62,    2, 0x06 /* Public */,
       5,    2,   63,    2, 0x06 /* Public */,
       7,    1,   68,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,   71,    2, 0x0a /* Public */,
       8,    1,   76,    2, 0x2a /* Public | MethodCloned */,
       8,    0,   79,    2, 0x2a /* Public | MethodCloned */,
      10,    1,   80,    2, 0x0a /* Public */,
      11,    2,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    6,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    9,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   12,    3,

       0        // eod
};

void NavigationTabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NavigationTabBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pageAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->closeWindowRequest(); break;
        case 2: _t->addPageRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->locationUpdated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->addPage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->addPage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->addPage(); break;
        case 7: _t->addPages((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 8: _t->updateLocation((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NavigationTabBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationTabBar::pageAdded)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NavigationTabBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationTabBar::closeWindowRequest)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NavigationTabBar::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationTabBar::addPageRequest)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NavigationTabBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationTabBar::locationUpdated)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NavigationTabBar::staticMetaObject = { {
    &QTabBar::staticMetaObject,
    qt_meta_stringdata_NavigationTabBar.data,
    qt_meta_data_NavigationTabBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NavigationTabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NavigationTabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NavigationTabBar.stringdata0))
        return static_cast<void*>(this);
    return QTabBar::qt_metacast(_clname);
}

int NavigationTabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void NavigationTabBar::pageAdded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NavigationTabBar::closeWindowRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NavigationTabBar::addPageRequest(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NavigationTabBar::locationUpdated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
