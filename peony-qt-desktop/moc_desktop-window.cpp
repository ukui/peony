/****************************************************************************
** Meta object code from reading C++ file 'desktop-window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "desktop-window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'desktop-window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__DesktopWindow_t {
    QByteArrayData data[17];
    char stringdata0[234];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__DesktopWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__DesktopWindow_t qt_meta_stringdata_Peony__DesktopWindow = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Peony::DesktopWindow"
QT_MOC_LITERAL(1, 21, 8), // "changeBg"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 6), // "bgPath"
QT_MOC_LITERAL(4, 38, 11), // "checkWindow"
QT_MOC_LITERAL(5, 50, 9), // "setBgPath"
QT_MOC_LITERAL(6, 60, 31), // "availableGeometryChangedProcess"
QT_MOC_LITERAL(7, 92, 8), // "geometry"
QT_MOC_LITERAL(8, 101, 29), // "virtualGeometryChangedProcess"
QT_MOC_LITERAL(9, 131, 22), // "geometryChangedProcess"
QT_MOC_LITERAL(10, 154, 7), // "scaleBg"
QT_MOC_LITERAL(11, 162, 10), // "updateView"
QT_MOC_LITERAL(12, 173, 17), // "updateWinGeometry"
QT_MOC_LITERAL(13, 191, 13), // "connectSignal"
QT_MOC_LITERAL(14, 205, 16), // "disconnectSignal"
QT_MOC_LITERAL(15, 222, 5), // "setBg"
QT_MOC_LITERAL(16, 228, 5) // "color"

    },
    "Peony::DesktopWindow\0changeBg\0\0bgPath\0"
    "checkWindow\0setBgPath\0"
    "availableGeometryChangedProcess\0"
    "geometry\0virtualGeometryChangedProcess\0"
    "geometryChangedProcess\0scaleBg\0"
    "updateView\0updateWinGeometry\0connectSignal\0"
    "disconnectSignal\0setBg\0color"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__DesktopWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    0,   82,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   83,    2, 0x0a /* Public */,
       6,    1,   86,    2, 0x0a /* Public */,
       8,    1,   89,    2, 0x0a /* Public */,
       9,    1,   92,    2, 0x0a /* Public */,
      10,    1,   95,    2, 0x0a /* Public */,
      11,    0,   98,    2, 0x0a /* Public */,
      12,    0,   99,    2, 0x0a /* Public */,
      13,    0,  100,    2, 0x0a /* Public */,
      14,    0,  101,    2, 0x0a /* Public */,
      15,    1,  102,    2, 0x09 /* Protected */,
      15,    1,  105,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QRect,    7,
    QMetaType::Void, QMetaType::QRect,    7,
    QMetaType::Void, QMetaType::QRect,    7,
    QMetaType::Void, QMetaType::QRect,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QColor,   16,

       0        // eod
};

void Peony::DesktopWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DesktopWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeBg((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->checkWindow(); break;
        case 2: _t->setBgPath((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->availableGeometryChangedProcess((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 4: _t->virtualGeometryChangedProcess((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 5: _t->geometryChangedProcess((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 6: _t->scaleBg((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 7: _t->updateView(); break;
        case 8: _t->updateWinGeometry(); break;
        case 9: _t->connectSignal(); break;
        case 10: _t->disconnectSignal(); break;
        case 11: _t->setBg((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->setBg((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DesktopWindow::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DesktopWindow::changeBg)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DesktopWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DesktopWindow::checkWindow)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::DesktopWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_Peony__DesktopWindow.data,
    qt_meta_data_Peony__DesktopWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::DesktopWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::DesktopWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__DesktopWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Peony::DesktopWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void Peony::DesktopWindow::changeBg(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peony::DesktopWindow::checkWindow()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
