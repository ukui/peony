/****************************************************************************
** Meta object code from reading C++ file 'default-preview-page.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/preview-page/default-preview-page/default-preview-page.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'default-preview-page.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__DefaultPreviewPage_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__DefaultPreviewPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__DefaultPreviewPage_t qt_meta_stringdata_Peony__DefaultPreviewPage = {
    {
QT_MOC_LITERAL(0, 0, 25) // "Peony::DefaultPreviewPage"

    },
    "Peony::DefaultPreviewPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__DefaultPreviewPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Peony::DefaultPreviewPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::DefaultPreviewPage::staticMetaObject = { {
    &QStackedWidget::staticMetaObject,
    qt_meta_stringdata_Peony__DefaultPreviewPage.data,
    qt_meta_data_Peony__DefaultPreviewPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::DefaultPreviewPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::DefaultPreviewPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__DefaultPreviewPage.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "PreviewPageIface"))
        return static_cast< PreviewPageIface*>(this);
    return QStackedWidget::qt_metacast(_clname);
}

int Peony::DefaultPreviewPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStackedWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_Peony__FilePreviewPage_t {
    QByteArrayData data[14];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FilePreviewPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FilePreviewPage_t qt_meta_stringdata_Peony__FilePreviewPage = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Peony::FilePreviewPage"
QT_MOC_LITERAL(1, 23, 10), // "updateInfo"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 9), // "FileInfo*"
QT_MOC_LITERAL(4, 45, 4), // "info"
QT_MOC_LITERAL(5, 50, 10), // "countAsync"
QT_MOC_LITERAL(6, 61, 3), // "uri"
QT_MOC_LITERAL(7, 65, 11), // "updateCount"
QT_MOC_LITERAL(8, 77, 6), // "cancel"
QT_MOC_LITERAL(9, 84, 10), // "resizeIcon"
QT_MOC_LITERAL(10, 95, 4), // "size"
QT_MOC_LITERAL(11, 100, 10), // "resetCount"
QT_MOC_LITERAL(12, 111, 13), // "onPreparedOne"
QT_MOC_LITERAL(13, 125, 11) // "onCountDone"

    },
    "Peony::FilePreviewPage\0updateInfo\0\0"
    "FileInfo*\0info\0countAsync\0uri\0updateCount\0"
    "cancel\0resizeIcon\0size\0resetCount\0"
    "onPreparedOne\0onCountDone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FilePreviewPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x08 /* Private */,
       5,    1,   57,    2, 0x08 /* Private */,
       7,    0,   60,    2, 0x08 /* Private */,
       8,    0,   61,    2, 0x08 /* Private */,
       9,    1,   62,    2, 0x08 /* Private */,
      11,    0,   65,    2, 0x09 /* Protected */,
      12,    2,   66,    2, 0x09 /* Protected */,
      13,    0,   71,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QSize,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::ULongLong,    6,   10,
    QMetaType::Void,

       0        // eod
};

void Peony::FilePreviewPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FilePreviewPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateInfo((*reinterpret_cast< FileInfo*(*)>(_a[1]))); break;
        case 1: _t->countAsync((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->updateCount(); break;
        case 3: _t->cancel(); break;
        case 4: _t->resizeIcon((*reinterpret_cast< QSize(*)>(_a[1]))); break;
        case 5: _t->resetCount(); break;
        case 6: _t->onPreparedOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint64(*)>(_a[2]))); break;
        case 7: _t->onCountDone(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FilePreviewPage::staticMetaObject = { {
    &QFrame::staticMetaObject,
    qt_meta_stringdata_Peony__FilePreviewPage.data,
    qt_meta_data_Peony__FilePreviewPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FilePreviewPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FilePreviewPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FilePreviewPage.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int Peony::FilePreviewPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
