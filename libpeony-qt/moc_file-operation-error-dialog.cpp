/****************************************************************************
** Meta object code from reading C++ file 'file-operation-error-dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-operation/file-operation-error-dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-operation-error-dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileOperationErrorDialog_t {
    QByteArrayData data[8];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationErrorDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationErrorDialog_t qt_meta_stringdata_Peony__FileOperationErrorDialog = {
    {
QT_MOC_LITERAL(0, 0, 31), // "Peony::FileOperationErrorDialog"
QT_MOC_LITERAL(1, 32, 11), // "handleError"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 6), // "srcUri"
QT_MOC_LITERAL(4, 52, 10), // "destDirUri"
QT_MOC_LITERAL(5, 63, 16), // "GErrorWrapperPtr"
QT_MOC_LITERAL(6, 80, 3), // "err"
QT_MOC_LITERAL(7, 84, 10) // "isCritical"

    },
    "Peony::FileOperationErrorDialog\0"
    "handleError\0\0srcUri\0destDirUri\0"
    "GErrorWrapperPtr\0err\0isCritical"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationErrorDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    4,   24,    2, 0x0a /* Public */,
       1,    3,   33,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 5, QMetaType::Bool,    3,    4,    6,    7,
    QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 5,    3,    4,    6,

       0        // eod
};

void Peony::FileOperationErrorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperationErrorDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { int _r = _t->handleError((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const GErrorWrapperPtr(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 1: { int _r = _t->handleError((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const GErrorWrapperPtr(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationErrorDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationErrorDialog.data,
    qt_meta_data_Peony__FileOperationErrorDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationErrorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationErrorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationErrorDialog.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "FileOperationErrorHandler"))
        return static_cast< FileOperationErrorHandler*>(this);
    if (!strcmp(_clname, "org.ukui.peony-qt.FileOperationErrorHandler"))
        return static_cast< Peony::FileOperationErrorHandler*>(this);
    return QDialog::qt_metacast(_clname);
}

int Peony::FileOperationErrorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
