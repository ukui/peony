/****************************************************************************
** Meta object code from reading C++ file 'file-info-job.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-info-job.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-info-job.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileInfoJob_t {
    QByteArrayData data[9];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileInfoJob_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileInfoJob_t qt_meta_stringdata_Peony__FileInfoJob = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Peony::FileInfoJob"
QT_MOC_LITERAL(1, 19, 18), // "queryAsyncFinished"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 9), // "successed"
QT_MOC_LITERAL(4, 49, 11), // "infoUpdated"
QT_MOC_LITERAL(5, 61, 10), // "queryAsync"
QT_MOC_LITERAL(6, 72, 6), // "cancel"
QT_MOC_LITERAL(7, 79, 10), // "getAppName"
QT_MOC_LITERAL(8, 90, 9) // "desktopfp"

    },
    "Peony::FileInfoJob\0queryAsyncFinished\0"
    "\0successed\0infoUpdated\0queryAsync\0"
    "cancel\0getAppName\0desktopfp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileInfoJob[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   43,    2, 0x0a /* Public */,
       6,    0,   44,    2, 0x0a /* Public */,
       7,    1,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QString, QMetaType::QString,    8,

       0        // eod
};

void Peony::FileInfoJob::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileInfoJob *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->queryAsyncFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->infoUpdated(); break;
        case 2: _t->queryAsync(); break;
        case 3: _t->cancel(); break;
        case 4: { QString _r = _t->getAppName((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileInfoJob::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileInfoJob::queryAsyncFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileInfoJob::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileInfoJob::infoUpdated)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileInfoJob::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileInfoJob.data,
    qt_meta_data_Peony__FileInfoJob,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileInfoJob::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileInfoJob::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileInfoJob.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileInfoJob::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Peony::FileInfoJob::queryAsyncFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::FileInfoJob::infoUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
