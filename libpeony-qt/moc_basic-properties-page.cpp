/****************************************************************************
** Meta object code from reading C++ file 'basic-properties-page.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/property-page/basic-properties-page.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'basic-properties-page.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__BasicPropertiesPage_t {
    QByteArrayData data[12];
    char stringdata0[130];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__BasicPropertiesPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__BasicPropertiesPage_t qt_meta_stringdata_Peony__BasicPropertiesPage = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Peony::BasicPropertiesPage"
QT_MOC_LITERAL(1, 27, 19), // "onSingleFileChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 6), // "oldUri"
QT_MOC_LITERAL(4, 55, 6), // "newUri"
QT_MOC_LITERAL(5, 62, 15), // "countFilesAsync"
QT_MOC_LITERAL(6, 78, 4), // "uris"
QT_MOC_LITERAL(7, 83, 14), // "onFileCountOne"
QT_MOC_LITERAL(8, 98, 3), // "uri"
QT_MOC_LITERAL(9, 102, 4), // "size"
QT_MOC_LITERAL(10, 107, 11), // "cancelCount"
QT_MOC_LITERAL(11, 119, 10) // "updateInfo"

    },
    "Peony::BasicPropertiesPage\0"
    "onSingleFileChanged\0\0oldUri\0newUri\0"
    "countFilesAsync\0uris\0onFileCountOne\0"
    "uri\0size\0cancelCount\0updateInfo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__BasicPropertiesPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x09 /* Protected */,
       5,    1,   44,    2, 0x09 /* Protected */,
       7,    2,   47,    2, 0x09 /* Protected */,
      10,    0,   52,    2, 0x09 /* Protected */,
      11,    1,   53,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QStringList,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::ULongLong,    8,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,

       0        // eod
};

void Peony::BasicPropertiesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BasicPropertiesPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onSingleFileChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->countFilesAsync((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 2: _t->onFileCountOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint64(*)>(_a[2]))); break;
        case 3: _t->cancelCount(); break;
        case 4: _t->updateInfo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::BasicPropertiesPage::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_Peony__BasicPropertiesPage.data,
    qt_meta_data_Peony__BasicPropertiesPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::BasicPropertiesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::BasicPropertiesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__BasicPropertiesPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Peony::BasicPropertiesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
