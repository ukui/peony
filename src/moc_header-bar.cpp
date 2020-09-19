/****************************************************************************
** Meta object code from reading C++ file 'header-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/header-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'header-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HeaderBarContainer_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HeaderBarContainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HeaderBarContainer_t qt_meta_stringdata_HeaderBarContainer = {
    {
QT_MOC_LITERAL(0, 0, 18) // "HeaderBarContainer"

    },
    "HeaderBarContainer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HeaderBarContainer[] = {

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

void HeaderBarContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject HeaderBarContainer::staticMetaObject = { {
    &QToolBar::staticMetaObject,
    qt_meta_stringdata_HeaderBarContainer.data,
    qt_meta_data_HeaderBarContainer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HeaderBarContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HeaderBarContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HeaderBarContainer.stringdata0))
        return static_cast<void*>(this);
    return QToolBar::qt_metacast(_clname);
}

int HeaderBarContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_HeaderBar_t {
    QByteArrayData data[27];
    char stringdata0[364];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HeaderBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HeaderBar_t qt_meta_stringdata_HeaderBar = {
    {
QT_MOC_LITERAL(0, 0, 9), // "HeaderBar"
QT_MOC_LITERAL(1, 10, 21), // "updateLocationRequest"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 3), // "uri"
QT_MOC_LITERAL(4, 37, 10), // "addHistory"
QT_MOC_LITERAL(5, 48, 5), // "force"
QT_MOC_LITERAL(6, 54, 21), // "viewTypeChangeRequest"
QT_MOC_LITERAL(7, 76, 6), // "viewId"
QT_MOC_LITERAL(8, 83, 26), // "updateZoomLevelHintRequest"
QT_MOC_LITERAL(9, 110, 13), // "zoomLevelHint"
QT_MOC_LITERAL(10, 124, 19), // "updateSearchRequest"
QT_MOC_LITERAL(11, 144, 10), // "showSearch"
QT_MOC_LITERAL(12, 155, 11), // "setLocation"
QT_MOC_LITERAL(13, 167, 11), // "updateIcons"
QT_MOC_LITERAL(14, 179, 21), // "updateSearchRecursive"
QT_MOC_LITERAL(15, 201, 9), // "recursive"
QT_MOC_LITERAL(16, 211, 19), // "updateMaximizeState"
QT_MOC_LITERAL(17, 231, 9), // "startEdit"
QT_MOC_LITERAL(18, 241, 7), // "bSearch"
QT_MOC_LITERAL(19, 249, 10), // "finishEdit"
QT_MOC_LITERAL(20, 260, 19), // "searchButtonClicked"
QT_MOC_LITERAL(21, 280, 19), // "openDefaultTerminal"
QT_MOC_LITERAL(22, 300, 19), // "findDefaultTerminal"
QT_MOC_LITERAL(23, 320, 12), // "tryOpenAgain"
QT_MOC_LITERAL(24, 333, 13), // "setSearchMode"
QT_MOC_LITERAL(25, 347, 4), // "mode"
QT_MOC_LITERAL(26, 352, 11) // "closeSearch"

    },
    "HeaderBar\0updateLocationRequest\0\0uri\0"
    "addHistory\0force\0viewTypeChangeRequest\0"
    "viewId\0updateZoomLevelHintRequest\0"
    "zoomLevelHint\0updateSearchRequest\0"
    "showSearch\0setLocation\0updateIcons\0"
    "updateSearchRecursive\0recursive\0"
    "updateMaximizeState\0startEdit\0bSearch\0"
    "finishEdit\0searchButtonClicked\0"
    "openDefaultTerminal\0findDefaultTerminal\0"
    "tryOpenAgain\0setSearchMode\0mode\0"
    "closeSearch"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HeaderBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,  109,    2, 0x06 /* Public */,
       1,    2,  116,    2, 0x26 /* Public | MethodCloned */,
       1,    1,  121,    2, 0x26 /* Public | MethodCloned */,
       6,    1,  124,    2, 0x06 /* Public */,
       8,    1,  127,    2, 0x06 /* Public */,
      10,    1,  130,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,  133,    2, 0x08 /* Private */,
      13,    0,  136,    2, 0x08 /* Private */,
      14,    1,  137,    2, 0x08 /* Private */,
      16,    0,  140,    2, 0x08 /* Private */,
      17,    1,  141,    2, 0x08 /* Private */,
      17,    0,  144,    2, 0x28 /* Private | MethodCloned */,
      19,    0,  145,    2, 0x08 /* Private */,
      20,    0,  146,    2, 0x08 /* Private */,
      21,    0,  147,    2, 0x08 /* Private */,
      22,    0,  148,    2, 0x08 /* Private */,
      23,    0,  149,    2, 0x08 /* Private */,
      24,    1,  150,    2, 0x08 /* Private */,
      26,    0,  153,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Bool,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   25,
    QMetaType::Void,

       0        // eod
};

void HeaderBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HeaderBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->updateLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->updateLocationRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->viewTypeChangeRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->updateZoomLevelHintRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->updateSearchRequest((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->setLocation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->updateIcons(); break;
        case 8: _t->updateSearchRecursive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->updateMaximizeState(); break;
        case 10: _t->startEdit((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->startEdit(); break;
        case 12: _t->finishEdit(); break;
        case 13: _t->searchButtonClicked(); break;
        case 14: _t->openDefaultTerminal(); break;
        case 15: _t->findDefaultTerminal(); break;
        case 16: _t->tryOpenAgain(); break;
        case 17: _t->setSearchMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->closeSearch(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HeaderBar::*)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderBar::updateLocationRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HeaderBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderBar::viewTypeChangeRequest)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (HeaderBar::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderBar::updateZoomLevelHintRequest)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (HeaderBar::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderBar::updateSearchRequest)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject HeaderBar::staticMetaObject = { {
    &QToolBar::staticMetaObject,
    qt_meta_stringdata_HeaderBar.data,
    qt_meta_data_HeaderBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HeaderBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HeaderBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HeaderBar.stringdata0))
        return static_cast<void*>(this);
    return QToolBar::qt_metacast(_clname);
}

int HeaderBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void HeaderBar::updateLocationRequest(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 3
void HeaderBar::viewTypeChangeRequest(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void HeaderBar::updateZoomLevelHintRequest(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void HeaderBar::updateSearchRequest(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
struct qt_meta_stringdata_HeaderBarToolButton_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HeaderBarToolButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HeaderBarToolButton_t qt_meta_stringdata_HeaderBarToolButton = {
    {
QT_MOC_LITERAL(0, 0, 19) // "HeaderBarToolButton"

    },
    "HeaderBarToolButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HeaderBarToolButton[] = {

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

void HeaderBarToolButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject HeaderBarToolButton::staticMetaObject = { {
    &QToolButton::staticMetaObject,
    qt_meta_stringdata_HeaderBarToolButton.data,
    qt_meta_data_HeaderBarToolButton,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HeaderBarToolButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HeaderBarToolButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HeaderBarToolButton.stringdata0))
        return static_cast<void*>(this);
    return QToolButton::qt_metacast(_clname);
}

int HeaderBarToolButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_HeadBarPushButton_t {
    QByteArrayData data[1];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HeadBarPushButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HeadBarPushButton_t qt_meta_stringdata_HeadBarPushButton = {
    {
QT_MOC_LITERAL(0, 0, 17) // "HeadBarPushButton"

    },
    "HeadBarPushButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HeadBarPushButton[] = {

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

void HeadBarPushButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject HeadBarPushButton::staticMetaObject = { {
    &QPushButton::staticMetaObject,
    qt_meta_stringdata_HeadBarPushButton.data,
    qt_meta_data_HeadBarPushButton,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HeadBarPushButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HeadBarPushButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HeadBarPushButton.stringdata0))
        return static_cast<void*>(this);
    return QPushButton::qt_metacast(_clname);
}

int HeadBarPushButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_TopMenuBar_t {
    QByteArrayData data[1];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TopMenuBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TopMenuBar_t qt_meta_stringdata_TopMenuBar = {
    {
QT_MOC_LITERAL(0, 0, 10) // "TopMenuBar"

    },
    "TopMenuBar"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TopMenuBar[] = {

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

void TopMenuBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject TopMenuBar::staticMetaObject = { {
    &QMenuBar::staticMetaObject,
    qt_meta_stringdata_TopMenuBar.data,
    qt_meta_data_TopMenuBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TopMenuBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TopMenuBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TopMenuBar.stringdata0))
        return static_cast<void*>(this);
    return QMenuBar::qt_metacast(_clname);
}

int TopMenuBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenuBar::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
