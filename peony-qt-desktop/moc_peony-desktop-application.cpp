/****************************************************************************
** Meta object code from reading C++ file 'peony-desktop-application.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "peony-desktop-application.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'peony-desktop-application.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PeonyDesktopApplication_t {
    QByteArrayData data[20];
    char stringdata0[275];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PeonyDesktopApplication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PeonyDesktopApplication_t qt_meta_stringdata_PeonyDesktopApplication = {
    {
QT_MOC_LITERAL(0, 0, 23), // "PeonyDesktopApplication"
QT_MOC_LITERAL(1, 24, 8), // "parseCmd"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 2), // "id"
QT_MOC_LITERAL(4, 37, 3), // "msg"
QT_MOC_LITERAL(5, 41, 9), // "isPrimary"
QT_MOC_LITERAL(6, 51, 15), // "isPrimaryScreen"
QT_MOC_LITERAL(7, 67, 8), // "QScreen*"
QT_MOC_LITERAL(8, 76, 6), // "screen"
QT_MOC_LITERAL(9, 83, 29), // "layoutDirectionChangedProcess"
QT_MOC_LITERAL(10, 113, 19), // "Qt::LayoutDirection"
QT_MOC_LITERAL(11, 133, 9), // "direction"
QT_MOC_LITERAL(12, 143, 27), // "primaryScreenChangedProcess"
QT_MOC_LITERAL(13, 171, 18), // "screenAddedProcess"
QT_MOC_LITERAL(14, 190, 20), // "screenRemovedProcess"
QT_MOC_LITERAL(15, 211, 9), // "addWindow"
QT_MOC_LITERAL(16, 221, 11), // "checkPrimay"
QT_MOC_LITERAL(17, 233, 15), // "changeBgProcess"
QT_MOC_LITERAL(18, 249, 6), // "bgPath"
QT_MOC_LITERAL(19, 256, 18) // "checkWindowProcess"

    },
    "PeonyDesktopApplication\0parseCmd\0\0id\0"
    "msg\0isPrimary\0isPrimaryScreen\0QScreen*\0"
    "screen\0layoutDirectionChangedProcess\0"
    "Qt::LayoutDirection\0direction\0"
    "primaryScreenChangedProcess\0"
    "screenAddedProcess\0screenRemovedProcess\0"
    "addWindow\0checkPrimay\0changeBgProcess\0"
    "bgPath\0checkWindowProcess"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PeonyDesktopApplication[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   64,    2, 0x09 /* Protected */,
       6,    1,   71,    2, 0x09 /* Protected */,
       9,    1,   74,    2, 0x0a /* Public */,
      12,    1,   77,    2, 0x0a /* Public */,
      13,    1,   80,    2, 0x0a /* Public */,
      14,    1,   83,    2, 0x0a /* Public */,
      15,    2,   86,    2, 0x0a /* Public */,
      15,    1,   91,    2, 0x2a /* Public | MethodCloned */,
      17,    1,   94,    2, 0x0a /* Public */,
      19,    0,   97,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::QByteArray, QMetaType::Bool,    3,    4,    5,
    QMetaType::Bool, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Bool,    8,   16,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,

       0        // eod
};

void PeonyDesktopApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PeonyDesktopApplication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->parseCmd((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: { bool _r = _t->isPrimaryScreen((*reinterpret_cast< QScreen*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->layoutDirectionChangedProcess((*reinterpret_cast< Qt::LayoutDirection(*)>(_a[1]))); break;
        case 3: _t->primaryScreenChangedProcess((*reinterpret_cast< QScreen*(*)>(_a[1]))); break;
        case 4: _t->screenAddedProcess((*reinterpret_cast< QScreen*(*)>(_a[1]))); break;
        case 5: _t->screenRemovedProcess((*reinterpret_cast< QScreen*(*)>(_a[1]))); break;
        case 6: _t->addWindow((*reinterpret_cast< QScreen*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: _t->addWindow((*reinterpret_cast< QScreen*(*)>(_a[1]))); break;
        case 8: _t->changeBgProcess((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->checkWindowProcess(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QScreen* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QScreen* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QScreen* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QScreen* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QScreen* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QScreen* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PeonyDesktopApplication::staticMetaObject = { {
    &SingleApplication::staticMetaObject,
    qt_meta_stringdata_PeonyDesktopApplication.data,
    qt_meta_data_PeonyDesktopApplication,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PeonyDesktopApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PeonyDesktopApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PeonyDesktopApplication.stringdata0))
        return static_cast<void*>(this);
    return SingleApplication::qt_metacast(_clname);
}

int PeonyDesktopApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SingleApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
