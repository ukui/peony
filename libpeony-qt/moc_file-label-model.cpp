/****************************************************************************
** Meta object code from reading C++ file 'file-label-model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "model/file-label-model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-label-model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileLabelModel_t {
    QByteArrayData data[10];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileLabelModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileLabelModel_t qt_meta_stringdata_FileLabelModel = {
    {
QT_MOC_LITERAL(0, 0, 14), // "FileLabelModel"
QT_MOC_LITERAL(1, 15, 16), // "fileLabelChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 3), // "uri"
QT_MOC_LITERAL(4, 37, 7), // "setName"
QT_MOC_LITERAL(5, 45, 14), // "FileLabelItem*"
QT_MOC_LITERAL(6, 60, 4), // "item"
QT_MOC_LITERAL(7, 65, 4), // "name"
QT_MOC_LITERAL(8, 70, 8), // "setColor"
QT_MOC_LITERAL(9, 79, 5) // "color"

    },
    "FileLabelModel\0fileLabelChanged\0\0uri\0"
    "setName\0FileLabelItem*\0item\0name\0"
    "setColor\0color"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileLabelModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   32,    2, 0x0a /* Public */,
       8,    2,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5, QMetaType::QString,    6,    7,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QColor,    6,    9,

       0        // eod
};

void FileLabelModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileLabelModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->fileLabelChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setName((*reinterpret_cast< FileLabelItem*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->setColor((*reinterpret_cast< FileLabelItem*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FileLabelItem* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< FileLabelItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileLabelModel::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileLabelModel::fileLabelChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FileLabelModel::staticMetaObject = { {
    &QAbstractListModel::staticMetaObject,
    qt_meta_stringdata_FileLabelModel.data,
    qt_meta_data_FileLabelModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FileLabelModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileLabelModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileLabelModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int FileLabelModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FileLabelModel::fileLabelChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_FileLabelItem_t {
    QByteArrayData data[6];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileLabelItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileLabelItem_t qt_meta_stringdata_FileLabelItem = {
    {
QT_MOC_LITERAL(0, 0, 13), // "FileLabelItem"
QT_MOC_LITERAL(1, 14, 11), // "nameChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 4), // "name"
QT_MOC_LITERAL(4, 32, 12), // "colorChanged"
QT_MOC_LITERAL(5, 45, 5) // "color"

    },
    "FileLabelItem\0nameChanged\0\0name\0"
    "colorChanged\0color"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileLabelItem[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QColor,    5,

       0        // eod
};

void FileLabelItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileLabelItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->colorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileLabelItem::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileLabelItem::nameChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileLabelItem::*)(const QColor & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileLabelItem::colorChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FileLabelItem::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_FileLabelItem.data,
    qt_meta_data_FileLabelItem,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FileLabelItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileLabelItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileLabelItem.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FileLabelItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void FileLabelItem::nameChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FileLabelItem::colorChanged(const QColor & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
