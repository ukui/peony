/****************************************************************************
** Meta object code from reading C++ file 'directory-view-plugin-iface.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../plugin-iface/directory-view-plugin-iface.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'directory-view-plugin-iface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__DirectoryViewProxyIface_t {
    QByteArrayData data[29];
    char stringdata0[385];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__DirectoryViewProxyIface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__DirectoryViewProxyIface_t qt_meta_stringdata_Peony__DirectoryViewProxyIface = {
    {
QT_MOC_LITERAL(0, 0, 30), // "Peony::DirectoryViewProxyIface"
QT_MOC_LITERAL(1, 31, 11), // "openRequest"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 3), // "uri"
QT_MOC_LITERAL(4, 48, 9), // "newWindow"
QT_MOC_LITERAL(5, 58, 17), // "viewDoubleClicked"
QT_MOC_LITERAL(6, 76, 20), // "viewDirectoryChanged"
QT_MOC_LITERAL(7, 97, 20), // "viewSelectionChanged"
QT_MOC_LITERAL(8, 118, 11), // "menuRequest"
QT_MOC_LITERAL(9, 130, 3), // "pos"
QT_MOC_LITERAL(10, 134, 27), // "updateWindowLocationRequest"
QT_MOC_LITERAL(11, 162, 11), // "getSortType"
QT_MOC_LITERAL(12, 174, 12), // "getSortOrder"
QT_MOC_LITERAL(13, 187, 4), // "open"
QT_MOC_LITERAL(14, 192, 4), // "uris"
QT_MOC_LITERAL(15, 197, 15), // "setDirectoryUri"
QT_MOC_LITERAL(16, 213, 19), // "beginLocationChange"
QT_MOC_LITERAL(17, 233, 18), // "stopLocationChange"
QT_MOC_LITERAL(18, 252, 10), // "closeProxy"
QT_MOC_LITERAL(19, 263, 13), // "setSelections"
QT_MOC_LITERAL(20, 277, 16), // "invertSelections"
QT_MOC_LITERAL(21, 294, 17), // "scrollToSelection"
QT_MOC_LITERAL(22, 312, 11), // "setCutFiles"
QT_MOC_LITERAL(23, 324, 11), // "setSortType"
QT_MOC_LITERAL(24, 336, 8), // "sortType"
QT_MOC_LITERAL(25, 345, 12), // "setSortOrder"
QT_MOC_LITERAL(26, 358, 9), // "sortOrder"
QT_MOC_LITERAL(27, 368, 7), // "editUri"
QT_MOC_LITERAL(28, 376, 8) // "editUris"

    },
    "Peony::DirectoryViewProxyIface\0"
    "openRequest\0\0uri\0newWindow\0viewDoubleClicked\0"
    "viewDirectoryChanged\0viewSelectionChanged\0"
    "menuRequest\0pos\0updateWindowLocationRequest\0"
    "getSortType\0getSortOrder\0open\0uris\0"
    "setDirectoryUri\0beginLocationChange\0"
    "stopLocationChange\0closeProxy\0"
    "setSelections\0invertSelections\0"
    "scrollToSelection\0setCutFiles\0setSortType\0"
    "sortType\0setSortOrder\0sortOrder\0editUri\0"
    "editUris"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__DirectoryViewProxyIface[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,  119,    2, 0x06 /* Public */,
       5,    1,  124,    2, 0x06 /* Public */,
       6,    0,  127,    2, 0x06 /* Public */,
       7,    0,  128,    2, 0x06 /* Public */,
       8,    1,  129,    2, 0x06 /* Public */,
      10,    1,  132,    2, 0x06 /* Public */,
      11,    0,  135,    2, 0x06 /* Public */,
      12,    0,  136,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    2,  137,    2, 0x0a /* Public */,
      15,    1,  142,    2, 0x0a /* Public */,
      16,    0,  145,    2, 0x0a /* Public */,
      17,    0,  146,    2, 0x0a /* Public */,
      18,    0,  147,    2, 0x0a /* Public */,
      19,    1,  148,    2, 0x0a /* Public */,
      20,    0,  151,    2, 0x0a /* Public */,
      21,    1,  152,    2, 0x0a /* Public */,
      22,    1,  155,    2, 0x0a /* Public */,
      23,    1,  158,    2, 0x0a /* Public */,
      25,    1,  161,    2, 0x0a /* Public */,
      27,    1,  164,    2, 0x0a /* Public */,
      28,    1,  167,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    9,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Int,
    QMetaType::Int,

 // slots: parameters
    QMetaType::Void, QMetaType::QStringList, QMetaType::Bool,   14,    4,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QStringList,   14,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QStringList,   14,

       0        // eod
};

void Peony::DirectoryViewProxyIface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DirectoryViewProxyIface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openRequest((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->viewDoubleClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->viewDirectoryChanged(); break;
        case 3: _t->viewSelectionChanged(); break;
        case 4: _t->menuRequest((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 5: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: { int _r = _t->getSortType();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 7: { int _r = _t->getSortOrder();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->open((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->setDirectoryUri((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->beginLocationChange(); break;
        case 11: _t->stopLocationChange(); break;
        case 12: _t->closeProxy(); break;
        case 13: _t->setSelections((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 14: _t->invertSelections(); break;
        case 15: _t->scrollToSelection((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->setCutFiles((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 17: _t->setSortType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->setSortOrder((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->editUri((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: _t->editUris((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DirectoryViewProxyIface::*)(const QStringList & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::openRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DirectoryViewProxyIface::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::viewDoubleClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DirectoryViewProxyIface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::viewDirectoryChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DirectoryViewProxyIface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::viewSelectionChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DirectoryViewProxyIface::*)(const QPoint & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::menuRequest)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DirectoryViewProxyIface::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::updateWindowLocationRequest)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = int (DirectoryViewProxyIface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::getSortType)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = int (DirectoryViewProxyIface::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirectoryViewProxyIface::getSortOrder)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::DirectoryViewProxyIface::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Peony__DirectoryViewProxyIface.data,
    qt_meta_data_Peony__DirectoryViewProxyIface,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::DirectoryViewProxyIface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::DirectoryViewProxyIface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__DirectoryViewProxyIface.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peony::DirectoryViewProxyIface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void Peony::DirectoryViewProxyIface::openRequest(const QStringList & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::DirectoryViewProxyIface::viewDoubleClicked(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peony::DirectoryViewProxyIface::viewDirectoryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Peony::DirectoryViewProxyIface::viewSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Peony::DirectoryViewProxyIface::menuRequest(const QPoint & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Peony::DirectoryViewProxyIface::updateWindowLocationRequest(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
