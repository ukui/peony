/****************************************************************************
** Meta object code from reading C++ file 'file-operation.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-operation/file-operation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-operation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileOperation_t {
    QByteArrayData data[29];
    char stringdata0[475];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperation_t qt_meta_stringdata_Peony__FileOperation = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Peony::FileOperation"
QT_MOC_LITERAL(1, 21, 16), // "invalidOperation"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 7), // "message"
QT_MOC_LITERAL(4, 47, 13), // "invalidExited"
QT_MOC_LITERAL(5, 61, 16), // "operationStarted"
QT_MOC_LITERAL(6, 78, 7), // "errored"
QT_MOC_LITERAL(7, 86, 19), // "FileOperationError&"
QT_MOC_LITERAL(8, 106, 5), // "error"
QT_MOC_LITERAL(9, 112, 20), // "FileProgressCallback"
QT_MOC_LITERAL(10, 133, 6), // "srcUri"
QT_MOC_LITERAL(11, 140, 7), // "destUri"
QT_MOC_LITERAL(12, 148, 4), // "icon"
QT_MOC_LITERAL(13, 153, 19), // "current_file_offset"
QT_MOC_LITERAL(14, 173, 17), // "current_file_size"
QT_MOC_LITERAL(15, 191, 26), // "operationRequestShowWizard"
QT_MOC_LITERAL(16, 218, 24), // "operationFallbackRetried"
QT_MOC_LITERAL(17, 243, 20), // "operationPreparedOne"
QT_MOC_LITERAL(18, 264, 4), // "size"
QT_MOC_LITERAL(19, 269, 17), // "operationPrepared"
QT_MOC_LITERAL(20, 287, 22), // "operationProgressedOne"
QT_MOC_LITERAL(21, 310, 19), // "operationProgressed"
QT_MOC_LITERAL(22, 330, 27), // "operationAfterProgressedOne"
QT_MOC_LITERAL(23, 358, 24), // "operationAfterProgressed"
QT_MOC_LITERAL(24, 383, 22), // "operationRollbackedOne"
QT_MOC_LITERAL(25, 406, 24), // "operationStartRollbacked"
QT_MOC_LITERAL(26, 431, 18), // "operationStartSnyc"
QT_MOC_LITERAL(27, 450, 17), // "operationFinished"
QT_MOC_LITERAL(28, 468, 6) // "cancel"

    },
    "Peony::FileOperation\0invalidOperation\0"
    "\0message\0invalidExited\0operationStarted\0"
    "errored\0FileOperationError&\0error\0"
    "FileProgressCallback\0srcUri\0destUri\0"
    "icon\0current_file_offset\0current_file_size\0"
    "operationRequestShowWizard\0"
    "operationFallbackRetried\0operationPreparedOne\0"
    "size\0operationPrepared\0operationProgressedOne\0"
    "operationProgressed\0operationAfterProgressedOne\0"
    "operationAfterProgressed\0"
    "operationRollbackedOne\0operationStartRollbacked\0"
    "operationStartSnyc\0operationFinished\0"
    "cancel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperation[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  104,    2, 0x06 /* Public */,
       4,    1,  107,    2, 0x06 /* Public */,
       5,    0,  110,    2, 0x06 /* Public */,
       6,    1,  111,    2, 0x06 /* Public */,
       9,    5,  114,    2, 0x06 /* Public */,
      15,    0,  125,    2, 0x06 /* Public */,
      16,    0,  126,    2, 0x06 /* Public */,
      17,    2,  127,    2, 0x06 /* Public */,
      19,    0,  132,    2, 0x06 /* Public */,
      20,    3,  133,    2, 0x06 /* Public */,
      21,    0,  140,    2, 0x06 /* Public */,
      22,    1,  141,    2, 0x06 /* Public */,
      23,    0,  144,    2, 0x06 /* Public */,
      24,    2,  145,    2, 0x06 /* Public */,
      25,    0,  150,    2, 0x06 /* Public */,
      26,    0,  151,    2, 0x06 /* Public */,
      27,    0,  152,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      28,    0,  153,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::LongLong, QMetaType::LongLong,   10,   11,   12,   13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong,   10,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::LongLong,   10,   11,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Peony::FileOperation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->invalidOperation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->invalidExited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->operationStarted(); break;
        case 3: _t->errored((*reinterpret_cast< FileOperationError(*)>(_a[1]))); break;
        case 4: _t->FileProgressCallback((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const qint64(*)>(_a[4])),(*reinterpret_cast< const qint64(*)>(_a[5]))); break;
        case 5: _t->operationRequestShowWizard(); break;
        case 6: _t->operationFallbackRetried(); break;
        case 7: _t->operationPreparedOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const qint64(*)>(_a[2]))); break;
        case 8: _t->operationPrepared(); break;
        case 9: _t->operationProgressedOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const qint64(*)>(_a[3]))); break;
        case 10: _t->operationProgressed(); break;
        case 11: _t->operationAfterProgressedOne((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->operationAfterProgressed(); break;
        case 13: _t->operationRollbackedOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 14: _t->operationStartRollbacked(); break;
        case 15: _t->operationStartSnyc(); break;
        case 16: _t->operationFinished(); break;
        case 17: _t->cancel(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileOperation::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::invalidOperation)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::invalidExited)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationStarted)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(FileOperationError & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::errored)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(const QString & , const QString & , const QString & , const qint64 & , const qint64 & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::FileProgressCallback)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationRequestShowWizard)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationFallbackRetried)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(const QString & , const qint64 & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationPreparedOne)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationPrepared)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(const QString & , const QString & , const qint64 & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationProgressedOne)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationProgressed)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationAfterProgressedOne)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationAfterProgressed)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationRollbackedOne)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationStartRollbacked)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationStartSnyc)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (FileOperation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperation::operationFinished)) {
                *result = 16;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperation::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperation.data,
    qt_meta_data_Peony__FileOperation,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperation.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QRunnable"))
        return static_cast< QRunnable*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileOperation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void Peony::FileOperation::invalidOperation(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::FileOperation::invalidExited(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peony::FileOperation::operationStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Peony::FileOperation::errored(FileOperationError & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Peony::FileOperation::FileProgressCallback(const QString & _t1, const QString & _t2, const QString & _t3, const qint64 & _t4, const qint64 & _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Peony::FileOperation::operationRequestShowWizard()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Peony::FileOperation::operationFallbackRetried()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void Peony::FileOperation::operationPreparedOne(const QString & _t1, const qint64 & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Peony::FileOperation::operationPrepared()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void Peony::FileOperation::operationProgressedOne(const QString & _t1, const QString & _t2, const qint64 & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Peony::FileOperation::operationProgressed()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void Peony::FileOperation::operationAfterProgressedOne(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Peony::FileOperation::operationAfterProgressed()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void Peony::FileOperation::operationRollbackedOne(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void Peony::FileOperation::operationStartRollbacked()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void Peony::FileOperation::operationStartSnyc()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void Peony::FileOperation::operationFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
