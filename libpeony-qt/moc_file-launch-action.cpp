/****************************************************************************
** Meta object code from reading C++ file 'file-launch-action.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-launcher/file-launch-action.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-launch-action.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileLaunchAction_t {
    QByteArrayData data[8];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileLaunchAction_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileLaunchAction_t qt_meta_stringdata_Peony__FileLaunchAction = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Peony::FileLaunchAction"
QT_MOC_LITERAL(1, 24, 13), // "lauchFileSync"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 12), // "forceWithArg"
QT_MOC_LITERAL(4, 52, 10), // "skipDialog"
QT_MOC_LITERAL(5, 63, 14), // "lauchFileAsync"
QT_MOC_LITERAL(6, 78, 15), // "lauchFilesAsync"
QT_MOC_LITERAL(7, 94, 5) // "files"

    },
    "Peony::FileLaunchAction\0lauchFileSync\0"
    "\0forceWithArg\0skipDialog\0lauchFileAsync\0"
    "lauchFilesAsync\0files"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileLaunchAction[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x0a /* Public */,
       1,    1,   64,    2, 0x2a /* Public | MethodCloned */,
       1,    0,   67,    2, 0x2a /* Public | MethodCloned */,
       5,    2,   68,    2, 0x0a /* Public */,
       5,    1,   73,    2, 0x2a /* Public | MethodCloned */,
       5,    0,   76,    2, 0x2a /* Public | MethodCloned */,
       6,    3,   77,    2, 0x0a /* Public */,
       6,    2,   84,    2, 0x2a /* Public | MethodCloned */,
       6,    1,   89,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList, QMetaType::Bool, QMetaType::Bool,    7,    3,    4,
    QMetaType::Void, QMetaType::QStringList, QMetaType::Bool,    7,    3,
    QMetaType::Void, QMetaType::QStringList,    7,

       0        // eod
};

void Peony::FileLaunchAction::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileLaunchAction *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->lauchFileSync((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->lauchFileSync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->lauchFileSync(); break;
        case 3: _t->lauchFileAsync((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->lauchFileAsync((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->lauchFileAsync(); break;
        case 6: _t->lauchFilesAsync((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 7: _t->lauchFilesAsync((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 8: _t->lauchFilesAsync((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileLaunchAction::staticMetaObject = { {
    &QAction::staticMetaObject,
    qt_meta_stringdata_Peony__FileLaunchAction.data,
    qt_meta_data_Peony__FileLaunchAction,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileLaunchAction::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileLaunchAction::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileLaunchAction.stringdata0))
        return static_cast<void*>(this);
    return QAction::qt_metacast(_clname);
}

int Peony::FileLaunchAction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAction::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
