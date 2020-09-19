/****************************************************************************
** Meta object code from reading C++ file 'bookmark-manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bookmark-manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bookmark-manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__BookMarkManager_t {
    QByteArrayData data[9];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__BookMarkManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__BookMarkManager_t qt_meta_stringdata_Peony__BookMarkManager = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Peony::BookMarkManager"
QT_MOC_LITERAL(1, 23, 10), // "urisLoaded"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 13), // "bookMarkAdded"
QT_MOC_LITERAL(4, 49, 3), // "uri"
QT_MOC_LITERAL(5, 53, 9), // "successed"
QT_MOC_LITERAL(6, 63, 15), // "bookMarkRemoved"
QT_MOC_LITERAL(7, 79, 11), // "addBookMark"
QT_MOC_LITERAL(8, 91, 14) // "removeBookMark"

    },
    "Peony::BookMarkManager\0urisLoaded\0\0"
    "bookMarkAdded\0uri\0successed\0bookMarkRemoved\0"
    "addBookMark\0removeBookMark"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__BookMarkManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    2,   40,    2, 0x06 /* Public */,
       6,    2,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   50,    2, 0x0a /* Public */,
       8,    1,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    4,

       0        // eod
};

void Peony::BookMarkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BookMarkManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->urisLoaded(); break;
        case 1: _t->bookMarkAdded((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->bookMarkRemoved((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->addBookMark((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->removeBookMark((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BookMarkManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BookMarkManager::urisLoaded)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BookMarkManager::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BookMarkManager::bookMarkAdded)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BookMarkManager::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BookMarkManager::bookMarkRemoved)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::BookMarkManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__BookMarkManager.data,
    qt_meta_data_Peony__BookMarkManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::BookMarkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::BookMarkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__BookMarkManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::BookMarkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Peony::BookMarkManager::urisLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Peony::BookMarkManager::bookMarkAdded(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peony::BookMarkManager::bookMarkRemoved(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
