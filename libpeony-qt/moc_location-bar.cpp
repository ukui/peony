/****************************************************************************
** Meta object code from reading C++ file 'location-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/navigation-bar/location-bar/location-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'location-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__LocationBar_t {
    QByteArrayData data[6];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__LocationBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__LocationBar_t qt_meta_stringdata_Peony__LocationBar = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Peony::LocationBar"
QT_MOC_LITERAL(1, 19, 19), // "groupChangedRequest"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 3), // "uri"
QT_MOC_LITERAL(4, 44, 12), // "blankClicked"
QT_MOC_LITERAL(5, 57, 10) // "setRootUri"

    },
    "Peony::LocationBar\0groupChangedRequest\0"
    "\0uri\0blankClicked\0setRootUri"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__LocationBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    0,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Peony::LocationBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LocationBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->groupChangedRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->blankClicked(); break;
        case 2: _t->setRootUri((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LocationBar::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LocationBar::groupChangedRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LocationBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LocationBar::blankClicked)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::LocationBar::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_Peony__LocationBar.data,
    qt_meta_data_Peony__LocationBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::LocationBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::LocationBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__LocationBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Peony::LocationBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Peony::LocationBar::groupChangedRequest(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::LocationBar::blankClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
