/****************************************************************************
** Meta object code from reading C++ file 'plugin-manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "plugin-manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plugin-manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__PluginManager_t {
    QByteArrayData data[6];
    char stringdata0[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__PluginManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__PluginManager_t qt_meta_stringdata_Peony__PluginManager = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Peony::PluginManager"
QT_MOC_LITERAL(1, 21, 18), // "pluginStateChanged"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 10), // "pluginName"
QT_MOC_LITERAL(4, 52, 6), // "enable"
QT_MOC_LITERAL(5, 59, 21) // "setPluginEnableByName"

    },
    "Peony::PluginManager\0pluginStateChanged\0"
    "\0pluginName\0enable\0setPluginEnableByName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__PluginManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    2,   29,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,

       0        // eod
};

void Peony::PluginManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PluginManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pluginStateChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->setPluginEnableByName((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PluginManager::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PluginManager::pluginStateChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::PluginManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__PluginManager.data,
    qt_meta_data_Peony__PluginManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::PluginManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::PluginManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__PluginManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::PluginManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Peony::PluginManager::pluginStateChanged(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
