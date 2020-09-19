/****************************************************************************
** Meta object code from reading C++ file 'file-item-model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "model/file-item-model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-item-model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileItemModel_t {
    QByteArrayData data[24];
    char stringdata0[265];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileItemModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileItemModel_t qt_meta_stringdata_Peony__FileItemModel = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Peony::FileItemModel"
QT_MOC_LITERAL(1, 21, 19), // "findChildrenStarted"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 20), // "findChildrenFinished"
QT_MOC_LITERAL(4, 63, 7), // "updated"
QT_MOC_LITERAL(5, 71, 15), // "onFoundChildren"
QT_MOC_LITERAL(6, 87, 11), // "QModelIndex"
QT_MOC_LITERAL(7, 99, 6), // "parent"
QT_MOC_LITERAL(8, 106, 11), // "onItemAdded"
QT_MOC_LITERAL(9, 118, 9), // "FileItem*"
QT_MOC_LITERAL(10, 128, 4), // "item"
QT_MOC_LITERAL(11, 133, 13), // "onItemRemoved"
QT_MOC_LITERAL(12, 147, 18), // "cancelFindChildren"
QT_MOC_LITERAL(13, 166, 12), // "setRootIndex"
QT_MOC_LITERAL(14, 179, 5), // "index"
QT_MOC_LITERAL(15, 185, 10), // "ColumnType"
QT_MOC_LITERAL(16, 196, 8), // "FileName"
QT_MOC_LITERAL(17, 205, 12), // "ModifiedDate"
QT_MOC_LITERAL(18, 218, 8), // "FileType"
QT_MOC_LITERAL(19, 227, 8), // "FileSize"
QT_MOC_LITERAL(20, 236, 5), // "Owner"
QT_MOC_LITERAL(21, 242, 5), // "Other"
QT_MOC_LITERAL(22, 248, 8), // "ItemRole"
QT_MOC_LITERAL(23, 257, 7) // "UriRole"

    },
    "Peony::FileItemModel\0findChildrenStarted\0"
    "\0findChildrenFinished\0updated\0"
    "onFoundChildren\0QModelIndex\0parent\0"
    "onItemAdded\0FileItem*\0item\0onItemRemoved\0"
    "cancelFindChildren\0setRootIndex\0index\0"
    "ColumnType\0FileName\0ModifiedDate\0"
    "FileType\0FileSize\0Owner\0Other\0ItemRole\0"
    "UriRole"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileItemModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       2,   70, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   57,    2, 0x0a /* Public */,
       8,    1,   60,    2, 0x0a /* Public */,
      11,    1,   63,    2, 0x0a /* Public */,
      12,    0,   66,    2, 0x0a /* Public */,
      13,    1,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,   14,

 // enums: name, alias, flags, count, data
      15,   15, 0x0,    6,   80,
      22,   22, 0x0,    1,   92,

 // enum data: key, value
      16, uint(Peony::FileItemModel::FileName),
      17, uint(Peony::FileItemModel::ModifiedDate),
      18, uint(Peony::FileItemModel::FileType),
      19, uint(Peony::FileItemModel::FileSize),
      20, uint(Peony::FileItemModel::Owner),
      21, uint(Peony::FileItemModel::Other),
      23, uint(Peony::FileItemModel::UriRole),

       0        // eod
};

void Peony::FileItemModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileItemModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->findChildrenStarted(); break;
        case 1: _t->findChildrenFinished(); break;
        case 2: _t->updated(); break;
        case 3: _t->onFoundChildren((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->onItemAdded((*reinterpret_cast< FileItem*(*)>(_a[1]))); break;
        case 5: _t->onItemRemoved((*reinterpret_cast< FileItem*(*)>(_a[1]))); break;
        case 6: _t->cancelFindChildren(); break;
        case 7: _t->setRootIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileItemModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItemModel::findChildrenStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileItemModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItemModel::findChildrenFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileItemModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileItemModel::updated)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileItemModel::staticMetaObject = { {
    &QAbstractItemModel::staticMetaObject,
    qt_meta_stringdata_Peony__FileItemModel.data,
    qt_meta_data_Peony__FileItemModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileItemModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileItemModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileItemModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractItemModel::qt_metacast(_clname);
}

int Peony::FileItemModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
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
void Peony::FileItemModel::findChildrenStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Peony::FileItemModel::findChildrenFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Peony::FileItemModel::updated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
