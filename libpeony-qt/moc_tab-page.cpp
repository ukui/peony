/****************************************************************************
** Meta object code from reading C++ file 'tab-page.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/tab-page/tab-page.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tab-page.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__TabPage_t {
    QByteArrayData data[15];
    char stringdata0[224];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__TabPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__TabPage_t qt_meta_stringdata_Peony__TabPage = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Peony::TabPage"
QT_MOC_LITERAL(1, 15, 24), // "currentActiveViewChanged"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 22), // "currentLocationChanged"
QT_MOC_LITERAL(4, 64, 23), // "currentSelectionChanged"
QT_MOC_LITERAL(5, 88, 15), // "viewTypeChanged"
QT_MOC_LITERAL(6, 104, 27), // "updateWindowLocationRequest"
QT_MOC_LITERAL(7, 132, 3), // "uri"
QT_MOC_LITERAL(8, 136, 10), // "addHistory"
QT_MOC_LITERAL(9, 147, 11), // "forceUpdate"
QT_MOC_LITERAL(10, 159, 11), // "menuRequest"
QT_MOC_LITERAL(11, 171, 3), // "pos"
QT_MOC_LITERAL(12, 175, 7), // "addPage"
QT_MOC_LITERAL(13, 183, 21), // "refreshCurrentTabText"
QT_MOC_LITERAL(14, 205, 18) // "stopLocationChange"

    },
    "Peony::TabPage\0currentActiveViewChanged\0"
    "\0currentLocationChanged\0currentSelectionChanged\0"
    "viewTypeChanged\0updateWindowLocationRequest\0"
    "uri\0addHistory\0forceUpdate\0menuRequest\0"
    "pos\0addPage\0refreshCurrentTabText\0"
    "stopLocationChange"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__TabPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    0,   70,    2, 0x06 /* Public */,
       4,    0,   71,    2, 0x06 /* Public */,
       5,    0,   72,    2, 0x06 /* Public */,
       6,    3,   73,    2, 0x06 /* Public */,
       6,    2,   80,    2, 0x26 /* Public | MethodCloned */,
       6,    1,   85,    2, 0x26 /* Public | MethodCloned */,
      10,    1,   88,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   91,    2, 0x0a /* Public */,
      13,    0,   94,    2, 0x0a /* Public */,
      14,    0,   95,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    7,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    7,    8,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QPoint,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Peony::TabPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TabPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentActiveViewChanged(); break;
        case 1: _t->currentLocationChanged(); break;
        case 2: _t->currentSelectionChanged(); break;
        case 3: _t->viewTypeChanged(); break;
        case 4: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->menuRequest((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 8: _t->addPage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->refreshCurrentTabText(); break;
        case 10: _t->stopLocationChange(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TabPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabPage::currentActiveViewChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TabPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabPage::currentLocationChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TabPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabPage::currentSelectionChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TabPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabPage::viewTypeChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TabPage::*)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabPage::updateWindowLocationRequest)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (TabPage::*)(const QPoint & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabPage::menuRequest)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::TabPage::staticMetaObject = { {
    &QTabWidget::staticMetaObject,
    qt_meta_stringdata_Peony__TabPage.data,
    qt_meta_data_Peony__TabPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::TabPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::TabPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__TabPage.stringdata0))
        return static_cast<void*>(this);
    return QTabWidget::qt_metacast(_clname);
}

int Peony::TabPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
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
void Peony::TabPage::currentActiveViewChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Peony::TabPage::currentLocationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Peony::TabPage::currentSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Peony::TabPage::viewTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Peony::TabPage::updateWindowLocationRequest(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 7
void Peony::TabPage::menuRequest(const QPoint & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
