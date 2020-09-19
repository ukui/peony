/****************************************************************************
** Meta object code from reading C++ file 'desktop-item-proxy-model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "desktop-item-proxy-model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'desktop-item-proxy-model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__DesktopItemProxyModel_t {
    QByteArrayData data[1];
    char stringdata0[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__DesktopItemProxyModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__DesktopItemProxyModel_t qt_meta_stringdata_Peony__DesktopItemProxyModel = {
    {
QT_MOC_LITERAL(0, 0, 28) // "Peony::DesktopItemProxyModel"

    },
    "Peony::DesktopItemProxyModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__DesktopItemProxyModel[] = {

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

void Peony::DesktopItemProxyModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::DesktopItemProxyModel::staticMetaObject = { {
    &QSortFilterProxyModel::staticMetaObject,
    qt_meta_stringdata_Peony__DesktopItemProxyModel.data,
    qt_meta_data_Peony__DesktopItemProxyModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::DesktopItemProxyModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::DesktopItemProxyModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__DesktopItemProxyModel.stringdata0))
        return static_cast<void*>(this);
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int Peony::DesktopItemProxyModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
