/****************************************************************************
** Meta object code from reading C++ file 'tab-widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "control/tab-widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tab-widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TabWidget_t {
    QByteArrayData data[95];
    char stringdata0[1312];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TabWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TabWidget_t qt_meta_stringdata_TabWidget = {
    {
QT_MOC_LITERAL(0, 0, 9), // "TabWidget"
QT_MOC_LITERAL(1, 10, 19), // "currentIndexChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 5), // "index"
QT_MOC_LITERAL(4, 37, 8), // "tabMoved"
QT_MOC_LITERAL(5, 46, 4), // "from"
QT_MOC_LITERAL(6, 51, 2), // "to"
QT_MOC_LITERAL(7, 54, 11), // "tabInserted"
QT_MOC_LITERAL(8, 66, 10), // "tabRemoved"
QT_MOC_LITERAL(9, 77, 22), // "searchRecursiveChanged"
QT_MOC_LITERAL(10, 100, 9), // "recursive"
QT_MOC_LITERAL(11, 110, 26), // "activePageSelectionChanged"
QT_MOC_LITERAL(12, 137, 17), // "activePageChanged"
QT_MOC_LITERAL(13, 155, 25), // "activePageLocationChanged"
QT_MOC_LITERAL(14, 181, 25), // "activePageViewTypeChanged"
QT_MOC_LITERAL(15, 207, 29), // "activePageViewSortTypeChanged"
QT_MOC_LITERAL(16, 237, 17), // "viewDoubleClicked"
QT_MOC_LITERAL(17, 255, 3), // "uri"
QT_MOC_LITERAL(18, 259, 27), // "updateWindowLocationRequest"
QT_MOC_LITERAL(19, 287, 10), // "addHistory"
QT_MOC_LITERAL(20, 298, 11), // "forceUpdate"
QT_MOC_LITERAL(21, 310, 11), // "menuRequest"
QT_MOC_LITERAL(22, 322, 3), // "pos"
QT_MOC_LITERAL(23, 326, 11), // "zoomRequest"
QT_MOC_LITERAL(24, 338, 6), // "zoomIn"
QT_MOC_LITERAL(25, 345, 18), // "closeWindowRequest"
QT_MOC_LITERAL(26, 364, 10), // "clearTrash"
QT_MOC_LITERAL(27, 375, 11), // "closeSearch"
QT_MOC_LITERAL(28, 387, 16), // "recoverFromTrash"
QT_MOC_LITERAL(29, 404, 23), // "currentSelectionChanged"
QT_MOC_LITERAL(30, 428, 17), // "tabBarIndexUpdate"
QT_MOC_LITERAL(31, 446, 15), // "setCurrentIndex"
QT_MOC_LITERAL(32, 462, 14), // "setPreviewPage"
QT_MOC_LITERAL(33, 477, 24), // "Peony::PreviewPageIface*"
QT_MOC_LITERAL(34, 502, 11), // "previewPage"
QT_MOC_LITERAL(35, 514, 7), // "addPage"
QT_MOC_LITERAL(36, 522, 6), // "jumpTo"
QT_MOC_LITERAL(37, 529, 7), // "goToUri"
QT_MOC_LITERAL(38, 537, 14), // "switchViewType"
QT_MOC_LITERAL(39, 552, 6), // "viewId"
QT_MOC_LITERAL(40, 559, 18), // "updateTabPageTitle"
QT_MOC_LITERAL(41, 578, 6), // "goBack"
QT_MOC_LITERAL(42, 585, 9), // "goForward"
QT_MOC_LITERAL(43, 595, 4), // "cdUp"
QT_MOC_LITERAL(44, 600, 7), // "refresh"
QT_MOC_LITERAL(45, 608, 11), // "stopLoading"
QT_MOC_LITERAL(46, 620, 7), // "tryJump"
QT_MOC_LITERAL(47, 628, 12), // "clearHistory"
QT_MOC_LITERAL(48, 641, 11), // "setSortType"
QT_MOC_LITERAL(49, 653, 4), // "type"
QT_MOC_LITERAL(50, 658, 12), // "setSortOrder"
QT_MOC_LITERAL(51, 671, 13), // "Qt::SortOrder"
QT_MOC_LITERAL(52, 685, 5), // "order"
QT_MOC_LITERAL(53, 691, 13), // "setSortFilter"
QT_MOC_LITERAL(54, 705, 13), // "FileTypeIndex"
QT_MOC_LITERAL(55, 719, 14), // "FileMTimeIndex"
QT_MOC_LITERAL(56, 734, 13), // "FileSizeIndex"
QT_MOC_LITERAL(57, 748, 13), // "setShowHidden"
QT_MOC_LITERAL(58, 762, 10), // "showHidden"
QT_MOC_LITERAL(59, 773, 26), // "setUseDefaultNameSortOrder"
QT_MOC_LITERAL(60, 800, 3), // "use"
QT_MOC_LITERAL(61, 804, 18), // "setSortFolderFirst"
QT_MOC_LITERAL(62, 823, 11), // "folderFirst"
QT_MOC_LITERAL(63, 835, 18), // "addFilterCondition"
QT_MOC_LITERAL(64, 854, 6), // "option"
QT_MOC_LITERAL(65, 861, 8), // "classify"
QT_MOC_LITERAL(66, 870, 9), // "updateNow"
QT_MOC_LITERAL(67, 880, 21), // "removeFilterCondition"
QT_MOC_LITERAL(68, 902, 15), // "clearConditions"
QT_MOC_LITERAL(69, 918, 12), // "updateFilter"
QT_MOC_LITERAL(70, 931, 23), // "updateAdvanceConditions"
QT_MOC_LITERAL(71, 955, 20), // "setCurrentSelections"
QT_MOC_LITERAL(72, 976, 4), // "uris"
QT_MOC_LITERAL(73, 981, 7), // "editUri"
QT_MOC_LITERAL(74, 989, 8), // "editUris"
QT_MOC_LITERAL(75, 998, 19), // "onViewDoubleClicked"
QT_MOC_LITERAL(76, 1018, 23), // "getTriggeredPreviewPage"
QT_MOC_LITERAL(77, 1042, 23), // "setTriggeredPreviewPage"
QT_MOC_LITERAL(78, 1066, 7), // "trigger"
QT_MOC_LITERAL(79, 1074, 5), // "count"
QT_MOC_LITERAL(80, 1080, 12), // "currentIndex"
QT_MOC_LITERAL(81, 1093, 9), // "removeTab"
QT_MOC_LITERAL(82, 1103, 21), // "updateTrashBarVisible"
QT_MOC_LITERAL(83, 1125, 22), // "updateSearchPathButton"
QT_MOC_LITERAL(84, 1148, 15), // "updateSearchBar"
QT_MOC_LITERAL(85, 1164, 10), // "showSearch"
QT_MOC_LITERAL(86, 1175, 16), // "updateSearchList"
QT_MOC_LITERAL(87, 1192, 13), // "updateButtons"
QT_MOC_LITERAL(88, 1206, 18), // "addNewConditionBar"
QT_MOC_LITERAL(89, 1225, 18), // "removeConditionBar"
QT_MOC_LITERAL(90, 1244, 12), // "searchUpdate"
QT_MOC_LITERAL(91, 1257, 17), // "searchChildUpdate"
QT_MOC_LITERAL(92, 1275, 10), // "browsePath"
QT_MOC_LITERAL(93, 1286, 15), // "handleZoomLevel"
QT_MOC_LITERAL(94, 1302, 9) // "zoomLevel"

    },
    "TabWidget\0currentIndexChanged\0\0index\0"
    "tabMoved\0from\0to\0tabInserted\0tabRemoved\0"
    "searchRecursiveChanged\0recursive\0"
    "activePageSelectionChanged\0activePageChanged\0"
    "activePageLocationChanged\0"
    "activePageViewTypeChanged\0"
    "activePageViewSortTypeChanged\0"
    "viewDoubleClicked\0uri\0updateWindowLocationRequest\0"
    "addHistory\0forceUpdate\0menuRequest\0"
    "pos\0zoomRequest\0zoomIn\0closeWindowRequest\0"
    "clearTrash\0closeSearch\0recoverFromTrash\0"
    "currentSelectionChanged\0tabBarIndexUpdate\0"
    "setCurrentIndex\0setPreviewPage\0"
    "Peony::PreviewPageIface*\0previewPage\0"
    "addPage\0jumpTo\0goToUri\0switchViewType\0"
    "viewId\0updateTabPageTitle\0goBack\0"
    "goForward\0cdUp\0refresh\0stopLoading\0"
    "tryJump\0clearHistory\0setSortType\0type\0"
    "setSortOrder\0Qt::SortOrder\0order\0"
    "setSortFilter\0FileTypeIndex\0FileMTimeIndex\0"
    "FileSizeIndex\0setShowHidden\0showHidden\0"
    "setUseDefaultNameSortOrder\0use\0"
    "setSortFolderFirst\0folderFirst\0"
    "addFilterCondition\0option\0classify\0"
    "updateNow\0removeFilterCondition\0"
    "clearConditions\0updateFilter\0"
    "updateAdvanceConditions\0setCurrentSelections\0"
    "uris\0editUri\0editUris\0onViewDoubleClicked\0"
    "getTriggeredPreviewPage\0setTriggeredPreviewPage\0"
    "trigger\0count\0currentIndex\0removeTab\0"
    "updateTrashBarVisible\0updateSearchPathButton\0"
    "updateSearchBar\0showSearch\0updateSearchList\0"
    "updateButtons\0addNewConditionBar\0"
    "removeConditionBar\0searchUpdate\0"
    "searchChildUpdate\0browsePath\0"
    "handleZoomLevel\0zoomLevel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TabWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      73,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      21,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  379,    2, 0x06 /* Public */,
       4,    2,  382,    2, 0x06 /* Public */,
       7,    1,  387,    2, 0x06 /* Public */,
       8,    1,  390,    2, 0x06 /* Public */,
       9,    1,  393,    2, 0x06 /* Public */,
      11,    0,  396,    2, 0x06 /* Public */,
      12,    0,  397,    2, 0x06 /* Public */,
      13,    0,  398,    2, 0x06 /* Public */,
      14,    0,  399,    2, 0x06 /* Public */,
      15,    0,  400,    2, 0x06 /* Public */,
      16,    1,  401,    2, 0x06 /* Public */,
      18,    3,  404,    2, 0x06 /* Public */,
      18,    2,  411,    2, 0x26 /* Public | MethodCloned */,
      21,    1,  416,    2, 0x06 /* Public */,
      23,    1,  419,    2, 0x06 /* Public */,
      25,    0,  422,    2, 0x06 /* Public */,
      26,    0,  423,    2, 0x06 /* Public */,
      27,    0,  424,    2, 0x06 /* Public */,
      28,    0,  425,    2, 0x06 /* Public */,
      29,    0,  426,    2, 0x06 /* Public */,
      30,    1,  427,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      31,    1,  430,    2, 0x0a /* Public */,
      32,    1,  433,    2, 0x0a /* Public */,
      32,    0,  436,    2, 0x2a /* Public | MethodCloned */,
      35,    2,  437,    2, 0x0a /* Public */,
      35,    1,  442,    2, 0x2a /* Public | MethodCloned */,
      37,    3,  445,    2, 0x0a /* Public */,
      37,    2,  452,    2, 0x2a /* Public | MethodCloned */,
      38,    1,  457,    2, 0x0a /* Public */,
      40,    0,  460,    2, 0x0a /* Public */,
      41,    0,  461,    2, 0x0a /* Public */,
      42,    0,  462,    2, 0x0a /* Public */,
      43,    0,  463,    2, 0x0a /* Public */,
      44,    0,  464,    2, 0x0a /* Public */,
      45,    0,  465,    2, 0x0a /* Public */,
      46,    1,  466,    2, 0x0a /* Public */,
      47,    0,  469,    2, 0x0a /* Public */,
      48,    1,  470,    2, 0x0a /* Public */,
      50,    1,  473,    2, 0x0a /* Public */,
      53,    3,  476,    2, 0x0a /* Public */,
      57,    1,  483,    2, 0x0a /* Public */,
      57,    0,  486,    2, 0x2a /* Public | MethodCloned */,
      59,    1,  487,    2, 0x0a /* Public */,
      61,    1,  490,    2, 0x0a /* Public */,
      63,    3,  493,    2, 0x0a /* Public */,
      63,    2,  500,    2, 0x2a /* Public | MethodCloned */,
      67,    3,  505,    2, 0x0a /* Public */,
      67,    2,  512,    2, 0x2a /* Public | MethodCloned */,
      68,    0,  517,    2, 0x0a /* Public */,
      69,    0,  518,    2, 0x0a /* Public */,
      70,    0,  519,    2, 0x0a /* Public */,
      71,    1,  520,    2, 0x0a /* Public */,
      73,    1,  523,    2, 0x0a /* Public */,
      74,    1,  526,    2, 0x0a /* Public */,
      75,    1,  529,    2, 0x0a /* Public */,
      76,    0,  532,    2, 0x0a /* Public */,
      77,    1,  533,    2, 0x0a /* Public */,
      79,    0,  536,    2, 0x0a /* Public */,
      80,    0,  537,    2, 0x0a /* Public */,
      81,    1,  538,    2, 0x0a /* Public */,
      82,    1,  541,    2, 0x0a /* Public */,
      82,    0,  544,    2, 0x2a /* Public | MethodCloned */,
      83,    1,  545,    2, 0x0a /* Public */,
      83,    0,  548,    2, 0x2a /* Public | MethodCloned */,
      84,    1,  549,    2, 0x0a /* Public */,
      86,    0,  552,    2, 0x0a /* Public */,
      87,    0,  553,    2, 0x0a /* Public */,
      88,    0,  554,    2, 0x0a /* Public */,
      89,    1,  555,    2, 0x0a /* Public */,
      90,    0,  558,    2, 0x0a /* Public */,
      91,    0,  559,    2, 0x0a /* Public */,
      92,    0,  560,    2, 0x0a /* Public */,
      93,    1,  561,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   17,   19,   20,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   17,   19,
    QMetaType::Void, QMetaType::QPoint,   22,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   17,   36,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   17,   19,   20,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   17,   19,
    QMetaType::Void, QMetaType::QString,   39,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void, 0x80000000 | 51,   52,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   54,   55,   56,
    QMetaType::Void, QMetaType::Bool,   58,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   60,
    QMetaType::Void, QMetaType::Bool,   62,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   64,   65,   66,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   64,   65,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   64,   65,   66,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   64,   65,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   72,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::QStringList,   72,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Bool,   78,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   85,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   94,

       0        // eod
};

void TabWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TabWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->tabMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->tabInserted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->tabRemoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->searchRecursiveChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->activePageSelectionChanged(); break;
        case 6: _t->activePageChanged(); break;
        case 7: _t->activePageLocationChanged(); break;
        case 8: _t->activePageViewTypeChanged(); break;
        case 9: _t->activePageViewSortTypeChanged(); break;
        case 10: _t->viewDoubleClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 12: _t->updateWindowLocationRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 13: _t->menuRequest((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 14: _t->zoomRequest((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->closeWindowRequest(); break;
        case 16: _t->clearTrash(); break;
        case 17: _t->closeSearch(); break;
        case 18: _t->recoverFromTrash(); break;
        case 19: _t->currentSelectionChanged(); break;
        case 20: _t->tabBarIndexUpdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->setPreviewPage((*reinterpret_cast< Peony::PreviewPageIface*(*)>(_a[1]))); break;
        case 23: _t->setPreviewPage(); break;
        case 24: _t->addPage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 25: _t->addPage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 26: _t->goToUri((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 27: _t->goToUri((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 28: _t->switchViewType((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 29: _t->updateTabPageTitle(); break;
        case 30: _t->goBack(); break;
        case 31: _t->goForward(); break;
        case 32: _t->cdUp(); break;
        case 33: _t->refresh(); break;
        case 34: _t->stopLoading(); break;
        case 35: _t->tryJump((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 36: _t->clearHistory(); break;
        case 37: _t->setSortType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 38: _t->setSortOrder((*reinterpret_cast< Qt::SortOrder(*)>(_a[1]))); break;
        case 39: _t->setSortFilter((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 40: _t->setShowHidden((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 41: _t->setShowHidden(); break;
        case 42: _t->setUseDefaultNameSortOrder((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 43: _t->setSortFolderFirst((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 44: _t->addFilterCondition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 45: _t->addFilterCondition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 46: _t->removeFilterCondition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 47: _t->removeFilterCondition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 48: _t->clearConditions(); break;
        case 49: _t->updateFilter(); break;
        case 50: _t->updateAdvanceConditions(); break;
        case 51: _t->setCurrentSelections((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 52: _t->editUri((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 53: _t->editUris((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 54: _t->onViewDoubleClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 55: { bool _r = _t->getTriggeredPreviewPage();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 56: _t->setTriggeredPreviewPage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: { int _r = _t->count();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 58: { int _r = _t->currentIndex();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 59: _t->removeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 60: _t->updateTrashBarVisible((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 61: _t->updateTrashBarVisible(); break;
        case 62: _t->updateSearchPathButton((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 63: _t->updateSearchPathButton(); break;
        case 64: _t->updateSearchBar((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 65: _t->updateSearchList(); break;
        case 66: _t->updateButtons(); break;
        case 67: _t->addNewConditionBar(); break;
        case 68: _t->removeConditionBar((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 69: _t->searchUpdate(); break;
        case 70: _t->searchChildUpdate(); break;
        case 71: _t->browsePath(); break;
        case 72: _t->handleZoomLevel((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TabWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::currentIndexChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::tabMoved)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::tabInserted)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::tabRemoved)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::searchRecursiveChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::activePageSelectionChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::activePageChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::activePageLocationChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::activePageViewTypeChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::activePageViewSortTypeChanged)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::viewDoubleClicked)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::updateWindowLocationRequest)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(const QPoint & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::menuRequest)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::zoomRequest)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::closeWindowRequest)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::clearTrash)) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::closeSearch)) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::recoverFromTrash)) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::currentSelectionChanged)) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (TabWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TabWidget::tabBarIndexUpdate)) {
                *result = 20;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TabWidget::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_TabWidget.data,
    qt_meta_data_TabWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TabWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabWidget.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int TabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 73)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 73;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 73)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 73;
    }
    return _id;
}

// SIGNAL 0
void TabWidget::currentIndexChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TabWidget::tabMoved(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TabWidget::tabInserted(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TabWidget::tabRemoved(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void TabWidget::searchRecursiveChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void TabWidget::activePageSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void TabWidget::activePageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void TabWidget::activePageLocationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void TabWidget::activePageViewTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void TabWidget::activePageViewSortTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void TabWidget::viewDoubleClicked(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void TabWidget::updateWindowLocationRequest(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 13
void TabWidget::menuRequest(const QPoint & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void TabWidget::zoomRequest(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void TabWidget::closeWindowRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void TabWidget::clearTrash()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void TabWidget::closeSearch()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void TabWidget::recoverFromTrash()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void TabWidget::currentSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void TabWidget::tabBarIndexUpdate(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}
struct qt_meta_stringdata_PreviewPageContainer_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PreviewPageContainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PreviewPageContainer_t qt_meta_stringdata_PreviewPageContainer = {
    {
QT_MOC_LITERAL(0, 0, 20) // "PreviewPageContainer"

    },
    "PreviewPageContainer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PreviewPageContainer[] = {

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

void PreviewPageContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject PreviewPageContainer::staticMetaObject = { {
    &QStackedWidget::staticMetaObject,
    qt_meta_stringdata_PreviewPageContainer.data,
    qt_meta_data_PreviewPageContainer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PreviewPageContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreviewPageContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PreviewPageContainer.stringdata0))
        return static_cast<void*>(this);
    return QStackedWidget::qt_metacast(_clname);
}

int PreviewPageContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStackedWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_PreviewPageButtonGroups_t {
    QByteArrayData data[5];
    char stringdata0[67];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PreviewPageButtonGroups_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PreviewPageButtonGroups_t qt_meta_stringdata_PreviewPageButtonGroups = {
    {
QT_MOC_LITERAL(0, 0, 23), // "PreviewPageButtonGroups"
QT_MOC_LITERAL(1, 24, 24), // "previewPageButtonTrigger"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 7), // "trigger"
QT_MOC_LITERAL(4, 58, 8) // "pluginId"

    },
    "PreviewPageButtonGroups\0"
    "previewPageButtonTrigger\0\0trigger\0"
    "pluginId"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PreviewPageButtonGroups[] = {

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
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    4,

       0        // eod
};

void PreviewPageButtonGroups::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PreviewPageButtonGroups *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->previewPageButtonTrigger((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PreviewPageButtonGroups::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PreviewPageButtonGroups::previewPageButtonTrigger)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PreviewPageButtonGroups::staticMetaObject = { {
    &QButtonGroup::staticMetaObject,
    qt_meta_stringdata_PreviewPageButtonGroups.data,
    qt_meta_data_PreviewPageButtonGroups,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PreviewPageButtonGroups::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreviewPageButtonGroups::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PreviewPageButtonGroups.stringdata0))
        return static_cast<void*>(this);
    return QButtonGroup::qt_metacast(_clname);
}

int PreviewPageButtonGroups::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QButtonGroup::qt_metacall(_c, _id, _a);
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
void PreviewPageButtonGroups::previewPageButtonTrigger(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
