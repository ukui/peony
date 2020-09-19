/****************************************************************************
** Meta object code from reading C++ file 'file-node-reporter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-operation/file-node-reporter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-node-reporter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileNodeReporter_t {
    QByteArrayData data[7];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileNodeReporter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileNodeReporter_t qt_meta_stringdata_Peony__FileNodeReporter = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Peony::FileNodeReporter"
QT_MOC_LITERAL(1, 24, 9), // "nodeFound"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 3), // "uri"
QT_MOC_LITERAL(4, 39, 6), // "offset"
QT_MOC_LITERAL(5, 46, 21), // "enumerateNodeFinished"
QT_MOC_LITERAL(6, 68, 17) // "nodeOperationDone"

    },
    "Peony::FileNodeReporter\0nodeFound\0\0"
    "uri\0offset\0enumerateNodeFinished\0"
    "nodeOperationDone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileNodeReporter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x06 /* Public */,
       5,    0,   34,    2, 0x06 /* Public */,
       6,    2,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong,    3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong,    3,    4,

       0        // eod
};

void Peony::FileNodeReporter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileNodeReporter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nodeFound((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const qint64(*)>(_a[2]))); break;
        case 1: _t->enumerateNodeFinished(); break;
        case 2: _t->nodeOperationDone((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const qint64(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileNodeReporter::*)(const QString & , const qint64 & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileNodeReporter::nodeFound)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileNodeReporter::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileNodeReporter::enumerateNodeFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileNodeReporter::*)(const QString & , const qint64 & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileNodeReporter::nodeOperationDone)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileNodeReporter::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileNodeReporter.data,
    qt_meta_data_Peony__FileNodeReporter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileNodeReporter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileNodeReporter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileNodeReporter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileNodeReporter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Peony::FileNodeReporter::nodeFound(const QString & _t1, const qint64 & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::FileNodeReporter::enumerateNodeFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Peony::FileNodeReporter::nodeOperationDone(const QString & _t1, const qint64 & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
