/****************************************************************************
** Meta object code from reading C++ file 'file-operation-progress-wizard.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file-operation/file-operation-progress-wizard.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file-operation-progress-wizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peony__FileOperationProgressWizard_t {
    QByteArrayData data[22];
    char stringdata0[339];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationProgressWizard_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationProgressWizard_t qt_meta_stringdata_Peony__FileOperationProgressWizard = {
    {
QT_MOC_LITERAL(0, 0, 34), // "Peony::FileOperationProgressW..."
QT_MOC_LITERAL(1, 35, 9), // "cancelled"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 9), // "delayShow"
QT_MOC_LITERAL(4, 56, 20), // "switchToPreparedPage"
QT_MOC_LITERAL(5, 77, 17), // "onElementFoundOne"
QT_MOC_LITERAL(6, 95, 3), // "uri"
QT_MOC_LITERAL(7, 99, 4), // "size"
QT_MOC_LITERAL(8, 104, 17), // "onElementFoundAll"
QT_MOC_LITERAL(9, 122, 20), // "switchToProgressPage"
QT_MOC_LITERAL(10, 143, 28), // "onFileOperationProgressedOne"
QT_MOC_LITERAL(11, 172, 7), // "destUri"
QT_MOC_LITERAL(12, 180, 28), // "onFileOperationProgressedAll"
QT_MOC_LITERAL(13, 209, 25), // "switchToAfterProgressPage"
QT_MOC_LITERAL(14, 235, 17), // "onElementClearOne"
QT_MOC_LITERAL(15, 253, 20), // "switchToRollbackPage"
QT_MOC_LITERAL(16, 274, 16), // "onFileRollbacked"
QT_MOC_LITERAL(17, 291, 6), // "srcUri"
QT_MOC_LITERAL(18, 298, 14), // "updateProgress"
QT_MOC_LITERAL(19, 313, 7), // "current"
QT_MOC_LITERAL(20, 321, 5), // "total"
QT_MOC_LITERAL(21, 327, 11) // "onStartSync"

    },
    "Peony::FileOperationProgressWizard\0"
    "cancelled\0\0delayShow\0switchToPreparedPage\0"
    "onElementFoundOne\0uri\0size\0onElementFoundAll\0"
    "switchToProgressPage\0onFileOperationProgressedOne\0"
    "destUri\0onFileOperationProgressedAll\0"
    "switchToAfterProgressPage\0onElementClearOne\0"
    "switchToRollbackPage\0onFileRollbacked\0"
    "srcUri\0updateProgress\0current\0total\0"
    "onStartSync"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationProgressWizard[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    2,   87,    2, 0x0a /* Public */,
       8,    0,   92,    2, 0x0a /* Public */,
       9,    0,   93,    2, 0x0a /* Public */,
      10,    3,   94,    2, 0x0a /* Public */,
      12,    0,  101,    2, 0x0a /* Public */,
      13,    0,  102,    2, 0x0a /* Public */,
      14,    1,  103,    2, 0x0a /* Public */,
      15,    0,  106,    2, 0x0a /* Public */,
      16,    2,  107,    2, 0x0a /* Public */,
      18,    4,  112,    2, 0x0a /* Public */,
      21,    0,  121,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong,    6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::LongLong,    6,   11,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   17,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::ULongLong, QMetaType::ULongLong,   17,   11,   19,   20,
    QMetaType::Void,

       0        // eod
};

void Peony::FileOperationProgressWizard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperationProgressWizard *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelled(); break;
        case 1: _t->delayShow(); break;
        case 2: _t->switchToPreparedPage(); break;
        case 3: _t->onElementFoundOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const qint64(*)>(_a[2]))); break;
        case 4: _t->onElementFoundAll(); break;
        case 5: _t->switchToProgressPage(); break;
        case 6: _t->onFileOperationProgressedOne((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const qint64(*)>(_a[3]))); break;
        case 7: _t->onFileOperationProgressedAll(); break;
        case 8: _t->switchToAfterProgressPage(); break;
        case 9: _t->onElementClearOne((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->switchToRollbackPage(); break;
        case 11: _t->onFileRollbacked((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: _t->updateProgress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< quint64(*)>(_a[3])),(*reinterpret_cast< quint64(*)>(_a[4]))); break;
        case 13: _t->onStartSync(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileOperationProgressWizard::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperationProgressWizard::cancelled)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationProgressWizard::staticMetaObject = { {
    &QWizard::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationProgressWizard.data,
    qt_meta_data_Peony__FileOperationProgressWizard,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationProgressWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationProgressWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationProgressWizard.stringdata0))
        return static_cast<void*>(this);
    return QWizard::qt_metacast(_clname);
}

int Peony::FileOperationProgressWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Peony::FileOperationProgressWizard::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_Peony__FileOperationPreparePage_t {
    QByteArrayData data[3];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationPreparePage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationPreparePage_t qt_meta_stringdata_Peony__FileOperationPreparePage = {
    {
QT_MOC_LITERAL(0, 0, 31), // "Peony::FileOperationPreparePage"
QT_MOC_LITERAL(1, 32, 9), // "cancelled"
QT_MOC_LITERAL(2, 42, 0) // ""

    },
    "Peony::FileOperationPreparePage\0"
    "cancelled\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationPreparePage[] = {

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
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void Peony::FileOperationPreparePage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperationPreparePage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelled(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileOperationPreparePage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperationPreparePage::cancelled)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationPreparePage::staticMetaObject = { {
    &QWizardPage::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationPreparePage.data,
    qt_meta_data_Peony__FileOperationPreparePage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationPreparePage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationPreparePage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationPreparePage.stringdata0))
        return static_cast<void*>(this);
    return QWizardPage::qt_metacast(_clname);
}

int Peony::FileOperationPreparePage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
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
void Peony::FileOperationPreparePage::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_Peony__FileOperationProgressPage_t {
    QByteArrayData data[3];
    char stringdata0[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationProgressPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationProgressPage_t qt_meta_stringdata_Peony__FileOperationProgressPage = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Peony::FileOperationProgressPage"
QT_MOC_LITERAL(1, 33, 9), // "cancelled"
QT_MOC_LITERAL(2, 43, 0) // ""

    },
    "Peony::FileOperationProgressPage\0"
    "cancelled\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationProgressPage[] = {

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
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void Peony::FileOperationProgressPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FileOperationProgressPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancelled(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileOperationProgressPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileOperationProgressPage::cancelled)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationProgressPage::staticMetaObject = { {
    &QWizardPage::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationProgressPage.data,
    qt_meta_data_Peony__FileOperationProgressPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationProgressPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationProgressPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationProgressPage.stringdata0))
        return static_cast<void*>(this);
    return QWizardPage::qt_metacast(_clname);
}

int Peony::FileOperationProgressPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
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
void Peony::FileOperationProgressPage::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_Peony__FileOperationAfterProgressPage_t {
    QByteArrayData data[1];
    char stringdata0[38];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationAfterProgressPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationAfterProgressPage_t qt_meta_stringdata_Peony__FileOperationAfterProgressPage = {
    {
QT_MOC_LITERAL(0, 0, 37) // "Peony::FileOperationAfterProg..."

    },
    "Peony::FileOperationAfterProgressPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationAfterProgressPage[] = {

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

void Peony::FileOperationAfterProgressPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationAfterProgressPage::staticMetaObject = { {
    &QWizardPage::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationAfterProgressPage.data,
    qt_meta_data_Peony__FileOperationAfterProgressPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationAfterProgressPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationAfterProgressPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationAfterProgressPage.stringdata0))
        return static_cast<void*>(this);
    return QWizardPage::qt_metacast(_clname);
}

int Peony::FileOperationAfterProgressPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_Peony__FileOperationRollbackPage_t {
    QByteArrayData data[1];
    char stringdata0[33];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peony__FileOperationRollbackPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peony__FileOperationRollbackPage_t qt_meta_stringdata_Peony__FileOperationRollbackPage = {
    {
QT_MOC_LITERAL(0, 0, 32) // "Peony::FileOperationRollbackPage"

    },
    "Peony::FileOperationRollbackPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peony__FileOperationRollbackPage[] = {

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

void Peony::FileOperationRollbackPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Peony::FileOperationRollbackPage::staticMetaObject = { {
    &QWizardPage::staticMetaObject,
    qt_meta_stringdata_Peony__FileOperationRollbackPage.data,
    qt_meta_data_Peony__FileOperationRollbackPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peony::FileOperationRollbackPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peony::FileOperationRollbackPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peony__FileOperationRollbackPage.stringdata0))
        return static_cast<void*>(this);
    return QWizardPage::qt_metacast(_clname);
}

int Peony::FileOperationRollbackPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
