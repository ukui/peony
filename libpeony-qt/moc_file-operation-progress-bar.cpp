/****************************************************************************
** Meta object code from reading C++ file 'file-operation-progress-bar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-operation/file-operation-progress-bar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-operation-progress-bar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileOperationProgressBar_t {
    QByteArrayData data[8];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileOperationProgressBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileOperationProgressBar_t qt_meta_stringdata_FileOperationProgressBar = {
    {
QT_MOC_LITERAL(0, 0, 24), // "FileOperationProgressBar"
QT_MOC_LITERAL(1, 25, 8), // "canceled"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 18), // "mainProgressChange"
QT_MOC_LITERAL(4, 54, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(5, 71, 4), // "item"
QT_MOC_LITERAL(6, 76, 9), // "showDelay"
QT_MOC_LITERAL(7, 86, 4) // "msec"

    },
    "FileOperationProgressBar\0canceled\0\0"
    "mainProgressChange\0QListWidgetItem*\0"
    "item\0showDelay\0msec"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileOperationProgressBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   35,    2, 0x0a /* Public */,
       6,    1,   38,    2, 0x0a /* Public */,
       6,    0,   41,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,

       0        // eod
};

void FileOperationProgressBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperationProgressBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->canceled(); break;
        case 1: _t->mainProgressChange((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 2: _t->showDelay((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->showDelay(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileOperationProgressBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperationProgressBar::canceled)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FileOperationProgressBar::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_FileOperationProgressBar.data,
    qt_meta_data_FileOperationProgressBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FileOperationProgressBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileOperationProgressBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileOperationProgressBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int FileOperationProgressBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void FileOperationProgressBar::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_ProgressBar_t {
    QByteArrayData data[28];
    char stringdata0[331];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProgressBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProgressBar_t qt_meta_stringdata_ProgressBar = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ProgressBar"
QT_MOC_LITERAL(1, 12, 9), // "cancelled"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 8), // "finished"
QT_MOC_LITERAL(4, 32, 12), // "ProgressBar*"
QT_MOC_LITERAL(5, 45, 3), // "fop"
QT_MOC_LITERAL(6, 49, 9), // "sendValue"
QT_MOC_LITERAL(7, 59, 8), // "QString&"
QT_MOC_LITERAL(8, 68, 6), // "QIcon&"
QT_MOC_LITERAL(9, 75, 11), // "onCancelled"
QT_MOC_LITERAL(10, 87, 11), // "updateValue"
QT_MOC_LITERAL(11, 99, 17), // "onElementFoundOne"
QT_MOC_LITERAL(12, 117, 3), // "uri"
QT_MOC_LITERAL(13, 121, 4), // "size"
QT_MOC_LITERAL(14, 126, 17), // "onElementFoundAll"
QT_MOC_LITERAL(15, 144, 28), // "onFileOperationProgressedOne"
QT_MOC_LITERAL(16, 173, 7), // "destUri"
QT_MOC_LITERAL(17, 181, 14), // "updateProgress"
QT_MOC_LITERAL(18, 196, 6), // "srcUri"
QT_MOC_LITERAL(19, 203, 5), // "fIcon"
QT_MOC_LITERAL(20, 209, 7), // "current"
QT_MOC_LITERAL(21, 217, 5), // "total"
QT_MOC_LITERAL(22, 223, 28), // "onFileOperationProgressedAll"
QT_MOC_LITERAL(23, 252, 17), // "onElementClearOne"
QT_MOC_LITERAL(24, 270, 20), // "switchToRollbackPage"
QT_MOC_LITERAL(25, 291, 11), // "onStartSync"
QT_MOC_LITERAL(26, 303, 10), // "onFinished"
QT_MOC_LITERAL(27, 314, 16) // "onFileRollbacked"

    },
    "ProgressBar\0cancelled\0\0finished\0"
    "ProgressBar*\0fop\0sendValue\0QString&\0"
    "QIcon&\0onCancelled\0updateValue\0"
    "onElementFoundOne\0uri\0size\0onElementFoundAll\0"
    "onFileOperationProgressedOne\0destUri\0"
    "updateProgress\0srcUri\0fIcon\0current\0"
    "total\0onFileOperationProgressedAll\0"
    "onElementClearOne\0switchToRollbackPage\0"
    "onStartSync\0onFinished\0onFileRollbacked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProgressBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,
       3,    1,   90,    2, 0x06 /* Public */,
       6,    3,   93,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  100,    2, 0x0a /* Public */,
      10,    1,  101,    2, 0x0a /* Public */,
      11,    2,  104,    2, 0x0a /* Public */,
      14,    0,  109,    2, 0x0a /* Public */,
      15,    3,  110,    2, 0x0a /* Public */,
      17,    5,  117,    2, 0x0a /* Public */,
      22,    0,  128,    2, 0x0a /* Public */,
      23,    1,  129,    2, 0x0a /* Public */,
      24,    0,  132,    2, 0x0a /* Public */,
      25,    0,  133,    2, 0x0a /* Public */,
      26,    0,  134,    2, 0x0a /* Public */,
      27,    2,  135,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 8, QMetaType::Double,    2,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong,   12,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::LongLong,   12,   16,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::ULongLong, QMetaType::ULongLong,   18,   16,   19,   20,   21,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   16,   18,

       0        // eod
};

void ProgressBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProgressBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelled(); break;
        case 1: _t->finished((*reinterpret_cast< ProgressBar*(*)>(_a[1]))); break;
        case 2: _t->sendValue((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QIcon(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 3: _t->onCancelled(); break;
        case 4: _t->updateValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->onElementFoundOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const qint64(*)>(_a[2]))); break;
        case 6: _t->onElementFoundAll(); break;
        case 7: _t->onFileOperationProgressedOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const qint64(*)>(_a[3]))); break;
        case 8: _t->updateProgress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const quint64(*)>(_a[4])),(*reinterpret_cast< const quint64(*)>(_a[5]))); break;
        case 9: _t->onFileOperationProgressedAll(); break;
        case 10: _t->onElementClearOne((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->switchToRollbackPage(); break;
        case 12: _t->onStartSync(); break;
        case 13: _t->onFinished(); break;
        case 14: _t->onFileRollbacked((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ProgressBar* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProgressBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressBar::cancelled)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProgressBar::*)(ProgressBar * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressBar::finished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProgressBar::*)(QString & , QIcon & , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProgressBar::sendValue)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProgressBar::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ProgressBar.data,
    qt_meta_data_ProgressBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProgressBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgressBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProgressBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ProgressBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void ProgressBar::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ProgressBar::finished(ProgressBar * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ProgressBar::sendValue(QString & _t1, QIcon & _t2, double _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_MainProgressBar_t {
    QByteArrayData data[8];
    char stringdata0[76];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainProgressBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainProgressBar_t qt_meta_stringdata_MainProgressBar = {
    {
QT_MOC_LITERAL(0, 0, 15), // "MainProgressBar"
QT_MOC_LITERAL(1, 16, 9), // "minimized"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 11), // "closeWindow"
QT_MOC_LITERAL(4, 39, 8), // "cancelld"
QT_MOC_LITERAL(5, 48, 11), // "updateValue"
QT_MOC_LITERAL(6, 60, 8), // "QString&"
QT_MOC_LITERAL(7, 69, 6) // "QIcon&"

    },
    "MainProgressBar\0minimized\0\0closeWindow\0"
    "cancelld\0updateValue\0QString&\0QIcon&"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainProgressBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   36,    2, 0x0a /* Public */,
       5,    3,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 7, QMetaType::Double,    2,    2,    2,

       0        // eod
};

void MainProgressBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainProgressBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->minimized(); break;
        case 1: _t->closeWindow(); break;
        case 2: _t->cancelld(); break;
        case 3: _t->updateValue((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QIcon(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainProgressBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainProgressBar::minimized)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainProgressBar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainProgressBar::closeWindow)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainProgressBar::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_MainProgressBar.data,
    qt_meta_data_MainProgressBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainProgressBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainProgressBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainProgressBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MainProgressBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void MainProgressBar::minimized()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MainProgressBar::closeWindow()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_OtherButton_t {
    QByteArrayData data[4];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OtherButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OtherButton_t qt_meta_stringdata_OtherButton = {
    {
QT_MOC_LITERAL(0, 0, 11), // "OtherButton"
QT_MOC_LITERAL(1, 12, 7), // "clicked"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 4) // "show"

    },
    "OtherButton\0clicked\0\0show"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OtherButton[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void OtherButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OtherButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OtherButton::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OtherButton::clicked)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject OtherButton::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_OtherButton.data,
    qt_meta_data_OtherButton,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OtherButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OtherButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OtherButton.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int OtherButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void OtherButton::clicked(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
