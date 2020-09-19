/****************************************************************************
** Meta object code from reading C++ file 'icon-view-delegate.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "controls/directory-view/delegate/icon-view-delegate.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'icon-view-delegate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__DirectoryView__IconViewDelegate_t {
    QByteArrayData data[7];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__DirectoryView__IconViewDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__DirectoryView__IconViewDelegate_t qt_meta_stringdata_Peony__DirectoryView__IconViewDelegate = {
    {
QT_MOC_LITERAL(0, 0, 38), // "Peony::DirectoryView::IconVie..."
QT_MOC_LITERAL(1, 39, 9), // "isEditing"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 7), // "editing"
QT_MOC_LITERAL(4, 58, 11), // "setCutFiles"
QT_MOC_LITERAL(5, 70, 15), // "QModelIndexList"
QT_MOC_LITERAL(6, 86, 7) // "indexes"

    },
    "Peony::DirectoryView::IconViewDelegate\0"
    "isEditing\0\0editing\0setCutFiles\0"
    "QModelIndexList\0indexes"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__DirectoryView__IconViewDelegate[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,

       0        // eod
};

void Peony::DirectoryView::IconViewDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<IconViewDelegate *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->isEditing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setCutFiles((*reinterpret_cast< const QModelIndexList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QModelIndexList >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (IconViewDelegate::*)(bool ) const;
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IconViewDelegate::isEditing)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::DirectoryView::IconViewDelegate::staticMetaObject = { {
    &QStyledItemDelegate::staticMetaObject,
    qt_meta_stringdata_Peony__DirectoryView__IconViewDelegate.data,
    qt_meta_data_Peony__DirectoryView__IconViewDelegate,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::DirectoryView::IconViewDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::DirectoryView::IconViewDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__DirectoryView__IconViewDelegate.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int Peony::DirectoryView::IconViewDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Peony::DirectoryView::IconViewDelegate::isEditing(bool _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(const_cast< Peony::DirectoryView::IconViewDelegate *>(this), &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
