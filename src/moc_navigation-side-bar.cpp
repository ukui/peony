/****************************************************************************
** Meta object code from reading C++ file 'navigation-side-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/navigation-side-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'navigation-side-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NavigationSideBar_t {
    QByteArrayData data[8];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NavigationSideBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NavigationSideBar_t qt_meta_stringdata_NavigationSideBar = {
    {
QT_MOC_LITERAL(0, 0, 17), // "NavigationSideBar"
QT_MOC_LITERAL(1, 18, 27), // "updateWindowLocationRequest"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 3), // "uri"
QT_MOC_LITERAL(4, 51, 10), // "addHistory"
QT_MOC_LITERAL(5, 62, 5), // "force"
QT_MOC_LITERAL(6, 68, 18), // "labelButtonClicked"
QT_MOC_LITERAL(7, 87, 7) // "checked"

    },
    "NavigationSideBar\0updateWindowLocationRequest\0"
    "\0uri\0addHistory\0force\0labelButtonClicked\0"
    "checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NavigationSideBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x06 /* Public */,
       1,    2,   41,    2, 0x26 /* Public | MethodCloned */,
       1,    1,   46,    2, 0x26 /* Public | MethodCloned */,
       6,    1,   49,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    7,

       0        // eod
};

void NavigationSideBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NavigationSideBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->labelButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NavigationSideBar::*)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationSideBar::updateWindowLocationRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NavigationSideBar::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NavigationSideBar::labelButtonClicked)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NavigationSideBar::staticMetaObject = { {
    &QTreeView::staticMetaObject,
    qt_meta_stringdata_NavigationSideBar.data,
    qt_meta_data_NavigationSideBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NavigationSideBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NavigationSideBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NavigationSideBar.stringdata0))
        return static_cast<void*>(this);
    return QTreeView::qt_metacast(_clname);
}

int NavigationSideBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
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
void NavigationSideBar::updateWindowLocationRequest(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 3
void NavigationSideBar::labelButtonClicked(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_NavigationSideBarContainer_t {
    QByteArrayData data[1];
    char stringdata0[27];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NavigationSideBarContainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NavigationSideBarContainer_t qt_meta_stringdata_NavigationSideBarContainer = {
    {
QT_MOC_LITERAL(0, 0, 26) // "NavigationSideBarContainer"

    },
    "NavigationSideBarContainer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NavigationSideBarContainer[] = {

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

void NavigationSideBarContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject NavigationSideBarContainer::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_NavigationSideBarContainer.data,
    qt_meta_data_NavigationSideBarContainer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NavigationSideBarContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NavigationSideBarContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NavigationSideBarContainer.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int NavigationSideBarContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_NavigationSideBarItemDelegate_t {
    QByteArrayData data[1];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NavigationSideBarItemDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NavigationSideBarItemDelegate_t qt_meta_stringdata_NavigationSideBarItemDelegate = {
    {
QT_MOC_LITERAL(0, 0, 29) // "NavigationSideBarItemDelegate"

    },
    "NavigationSideBarItemDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NavigationSideBarItemDelegate[] = {

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

void NavigationSideBarItemDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject NavigationSideBarItemDelegate::staticMetaObject = { {
    &QStyledItemDelegate::staticMetaObject,
    qt_meta_stringdata_NavigationSideBarItemDelegate.data,
    qt_meta_data_NavigationSideBarItemDelegate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NavigationSideBarItemDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NavigationSideBarItemDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NavigationSideBarItemDelegate.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int NavigationSideBarItemDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_TitleLabel_t {
    QByteArrayData data[1];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TitleLabel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TitleLabel_t qt_meta_stringdata_TitleLabel = {
    {
QT_MOC_LITERAL(0, 0, 10) // "TitleLabel"

    },
    "TitleLabel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TitleLabel[] = {

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

void TitleLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject TitleLabel::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_TitleLabel.data,
    qt_meta_data_TitleLabel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TitleLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TitleLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TitleLabel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TitleLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
