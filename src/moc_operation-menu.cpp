/****************************************************************************
** Meta object code from reading C++ file 'operation-menu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/operation-menu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'operation-menu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OperationMenu_t {
    QByteArrayData data[3];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OperationMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OperationMenu_t qt_meta_stringdata_OperationMenu = {
    {
QT_MOC_LITERAL(0, 0, 13), // "OperationMenu"
QT_MOC_LITERAL(1, 14, 10), // "updateMenu"
QT_MOC_LITERAL(2, 25, 0) // ""

    },
    "OperationMenu\0updateMenu\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OperationMenu[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void OperationMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OperationMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateMenu(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject OperationMenu::staticMetaObject = { {
    &QMenu::staticMetaObject,
    qt_meta_stringdata_OperationMenu.data,
    qt_meta_data_OperationMenu,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OperationMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OperationMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OperationMenu.stringdata0))
        return static_cast<void*>(this);
    return QMenu::qt_metacast(_clname);
}

int OperationMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_OperationMenuEditWidget_t {
    QByteArrayData data[3];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OperationMenuEditWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OperationMenuEditWidget_t qt_meta_stringdata_OperationMenuEditWidget = {
    {
QT_MOC_LITERAL(0, 0, 23), // "OperationMenuEditWidget"
QT_MOC_LITERAL(1, 24, 17), // "operationAccepted"
QT_MOC_LITERAL(2, 42, 0) // ""

    },
    "OperationMenuEditWidget\0operationAccepted\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OperationMenuEditWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void OperationMenuEditWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OperationMenuEditWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->operationAccepted(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OperationMenuEditWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OperationMenuEditWidget::operationAccepted)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject OperationMenuEditWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_OperationMenuEditWidget.data,
    qt_meta_data_OperationMenuEditWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OperationMenuEditWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OperationMenuEditWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OperationMenuEditWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int OperationMenuEditWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void OperationMenuEditWidget::operationAccepted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
