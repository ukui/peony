/****************************************************************************
** Meta object code from reading C++ file 'search-vfs-manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vfs/search-vfs-manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'search-vfs-manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__SearchVFSManager_t {
    QByteArrayData data[10];
    char stringdata0[122];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__SearchVFSManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__SearchVFSManager_t qt_meta_stringdata_Peony__SearchVFSManager = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Peony::SearchVFSManager"
QT_MOC_LITERAL(1, 24, 12), // "clearHistory"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 15), // "clearHistoryOne"
QT_MOC_LITERAL(4, 54, 9), // "searchUri"
QT_MOC_LITERAL(5, 64, 10), // "addHistory"
QT_MOC_LITERAL(6, 75, 7), // "results"
QT_MOC_LITERAL(7, 83, 10), // "hasHistory"
QT_MOC_LITERAL(8, 94, 9), // "serachUri"
QT_MOC_LITERAL(9, 104, 17) // "getHistroyResults"

    },
    "Peony::SearchVFSManager\0clearHistory\0"
    "\0clearHistoryOne\0searchUri\0addHistory\0"
    "results\0hasHistory\0serachUri\0"
    "getHistroyResults"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__SearchVFSManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x0a /* Public */,
       3,    1,   40,    2, 0x0a /* Public */,
       5,    2,   43,    2, 0x0a /* Public */,
       7,    1,   48,    2, 0x0a /* Public */,
       9,    1,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList,    4,    6,
    QMetaType::Bool, QMetaType::QString,    8,
    QMetaType::QStringList, QMetaType::QString,    4,

       0        // eod
};

void Peony::SearchVFSManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SearchVFSManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clearHistory(); break;
        case 1: _t->clearHistoryOne((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->addHistory((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 3: { bool _r = _t->hasHistory((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: { QStringList _r = _t->getHistroyResults((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::SearchVFSManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__SearchVFSManager.data,
    qt_meta_data_Peony__SearchVFSManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::SearchVFSManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::SearchVFSManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__SearchVFSManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::SearchVFSManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
