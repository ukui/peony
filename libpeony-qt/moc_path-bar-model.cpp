/****************************************************************************
** Meta object code from reading C++ file 'path-bar-model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "model/path-bar-model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'path-bar-model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__PathBarModel_t {
    QByteArrayData data[8];
    char stringdata0[67];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__PathBarModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__PathBarModel_t qt_meta_stringdata_Peony__PathBarModel = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Peony::PathBarModel"
QT_MOC_LITERAL(1, 20, 7), // "updated"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 11), // "setRootPath"
QT_MOC_LITERAL(4, 41, 4), // "path"
QT_MOC_LITERAL(5, 46, 5), // "force"
QT_MOC_LITERAL(6, 52, 10), // "setRootUri"
QT_MOC_LITERAL(7, 63, 3) // "uri"

    },
    "Peony::PathBarModel\0updated\0\0setRootPath\0"
    "path\0force\0setRootUri\0uri"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__PathBarModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    2,   40,    2, 0x0a /* Public */,
       3,    1,   45,    2, 0x2a /* Public | MethodCloned */,
       6,    2,   48,    2, 0x0a /* Public */,
       6,    1,   53,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    4,    5,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    7,    5,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void Peony::PathBarModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PathBarModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updated(); break;
        case 1: _t->setRootPath((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->setRootPath((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->setRootUri((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->setRootUri((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PathBarModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PathBarModel::updated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::PathBarModel::staticMetaObject = { {
    &QStringListModel::staticMetaObject,
    qt_meta_stringdata_Peony__PathBarModel.data,
    qt_meta_data_Peony__PathBarModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::PathBarModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::PathBarModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__PathBarModel.stringdata0))
        return static_cast<void*>(this);
    return QStringListModel::qt_metacast(_clname);
}

int Peony::PathBarModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStringListModel::qt_metacall(_c, _id, _a);
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
void Peony::PathBarModel::updated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
