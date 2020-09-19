/****************************************************************************
** Meta object code from reading C++ file 'desktop-item-model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "desktop-item-model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'desktop-item-model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__DesktopItemModel_t {
    QByteArrayData data[13];
    char stringdata0[175];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__DesktopItemModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__DesktopItemModel_t qt_meta_stringdata_Peony__DesktopItemModel = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Peony::DesktopItemModel"
QT_MOC_LITERAL(1, 24, 20), // "requestLayoutNewItem"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 3), // "uri"
QT_MOC_LITERAL(4, 50, 23), // "requestClearIndexWidget"
QT_MOC_LITERAL(5, 74, 26), // "requestUpdateItemPositions"
QT_MOC_LITERAL(6, 101, 9), // "refreshed"
QT_MOC_LITERAL(7, 111, 11), // "fileCreated"
QT_MOC_LITERAL(8, 123, 7), // "refresh"
QT_MOC_LITERAL(9, 131, 19), // "onEnumerateFinished"
QT_MOC_LITERAL(10, 151, 4), // "Role"
QT_MOC_LITERAL(11, 156, 7), // "UriRole"
QT_MOC_LITERAL(12, 164, 10) // "IsLinkRole"

    },
    "Peony::DesktopItemModel\0requestLayoutNewItem\0"
    "\0uri\0requestClearIndexWidget\0"
    "requestUpdateItemPositions\0refreshed\0"
    "fileCreated\0refresh\0onEnumerateFinished\0"
    "Role\0UriRole\0IsLinkRole"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__DesktopItemModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       1,   68, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    0,   57,    2, 0x06 /* Public */,
       5,    1,   58,    2, 0x06 /* Public */,
       5,    0,   61,    2, 0x26 /* Public | MethodCloned */,
       6,    0,   62,    2, 0x06 /* Public */,
       7,    1,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   66,    2, 0x0a /* Public */,
       9,    0,   67,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

 // enums: name, alias, flags, count, data
      10,   10, 0x0,    2,   73,

 // enum data: key, value
      11, uint(Peony::DesktopItemModel::UriRole),
      12, uint(Peony::DesktopItemModel::IsLinkRole),

       0        // eod
};

void Peony::DesktopItemModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DesktopItemModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestLayoutNewItem((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->requestClearIndexWidget(); break;
        case 2: _t->requestUpdateItemPositions((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->requestUpdateItemPositions(); break;
        case 4: _t->refreshed(); break;
        case 5: _t->fileCreated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->refresh(); break;
        case 7: _t->onEnumerateFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DesktopItemModel::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DesktopItemModel::requestLayoutNewItem)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DesktopItemModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DesktopItemModel::requestClearIndexWidget)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DesktopItemModel::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DesktopItemModel::requestUpdateItemPositions)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DesktopItemModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DesktopItemModel::refreshed)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DesktopItemModel::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DesktopItemModel::fileCreated)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::DesktopItemModel::staticMetaObject = { {
    &QAbstractListModel::staticMetaObject,
    qt_meta_stringdata_Peony__DesktopItemModel.data,
    qt_meta_data_Peony__DesktopItemModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::DesktopItemModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::DesktopItemModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__DesktopItemModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int Peony::DesktopItemModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Peony::DesktopItemModel::requestLayoutNewItem(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::DesktopItemModel::requestClearIndexWidget()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Peony::DesktopItemModel::requestUpdateItemPositions(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 4
void Peony::DesktopItemModel::refreshed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Peony::DesktopItemModel::fileCreated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
