/****************************************************************************
** Meta object code from reading C++ file 'file-enumerator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-enumerator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-enumerator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileEnumerator_t {
    QByteArrayData data[13];
    char stringdata0[167];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileEnumerator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileEnumerator_t qt_meta_stringdata_Peony__FileEnumerator = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Peony::FileEnumerator"
QT_MOC_LITERAL(1, 22, 8), // "prepared"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 37), // "std::shared_ptr<Peony::GError..."
QT_MOC_LITERAL(4, 70, 3), // "err"
QT_MOC_LITERAL(5, 74, 9), // "targetUri"
QT_MOC_LITERAL(6, 84, 8), // "critical"
QT_MOC_LITERAL(7, 93, 15), // "childrenUpdated"
QT_MOC_LITERAL(8, 109, 7), // "uriList"
QT_MOC_LITERAL(9, 117, 17), // "enumerateFinished"
QT_MOC_LITERAL(10, 135, 9), // "successed"
QT_MOC_LITERAL(11, 145, 14), // "enumerateAsync"
QT_MOC_LITERAL(12, 160, 6) // "cancel"

    },
    "Peony::FileEnumerator\0prepared\0\0"
    "std::shared_ptr<Peony::GErrorWrapper>\0"
    "err\0targetUri\0critical\0childrenUpdated\0"
    "uriList\0enumerateFinished\0successed\0"
    "enumerateAsync\0cancel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileEnumerator[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   59,    2, 0x06 /* Public */,
       1,    2,   66,    2, 0x26 /* Public | MethodCloned */,
       1,    1,   71,    2, 0x26 /* Public | MethodCloned */,
       1,    0,   74,    2, 0x26 /* Public | MethodCloned */,
       7,    1,   75,    2, 0x06 /* Public */,
       9,    1,   78,    2, 0x06 /* Public */,
       9,    0,   81,    2, 0x26 /* Public | MethodCloned */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   82,    2, 0x0a /* Public */,
      12,    0,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::Bool,    4,    5,    6,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    5,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Peony::FileEnumerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileEnumerator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->prepared((*reinterpret_cast< const std::shared_ptr<Peony::GErrorWrapper>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->prepared((*reinterpret_cast< const std::shared_ptr<Peony::GErrorWrapper>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->prepared((*reinterpret_cast< const std::shared_ptr<Peony::GErrorWrapper>(*)>(_a[1]))); break;
        case 3: _t->prepared(); break;
        case 4: _t->childrenUpdated((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 5: _t->enumerateFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->enumerateFinished(); break;
        case 7: _t->enumerateAsync(); break;
        case 8: _t->cancel(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileEnumerator::*)(const std::shared_ptr<Peony::GErrorWrapper> & , const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileEnumerator::prepared)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FileEnumerator::*)(const QStringList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileEnumerator::childrenUpdated)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (FileEnumerator::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileEnumerator::enumerateFinished)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileEnumerator::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__FileEnumerator.data,
    qt_meta_data_Peony__FileEnumerator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileEnumerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileEnumerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileEnumerator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::FileEnumerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Peony::FileEnumerator::prepared(const std::shared_ptr<Peony::GErrorWrapper> & _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 4
void Peony::FileEnumerator::childrenUpdated(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Peony::FileEnumerator::enumerateFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
