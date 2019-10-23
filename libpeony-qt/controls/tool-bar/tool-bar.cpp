#include "tool-bar.h"
#include "search-bar.h"
#include "fm-window.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include "clipboard-utils.h"
#include "file-operation-utils.h"

#include "view-factory-model.h"
#include "view-factory-sort-filter-model.h"
#include "directory-view-container.h"

#include <QAction>
#include <QComboBox>
#include <QPushButton>

#include <QStandardPaths>

#include <QMessageBox>

#include <QDebug>

using namespace Peony;

ToolBar::ToolBar(FMWindow *window, QWidget *parent) : QToolBar(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setFixedHeight(50);

    m_top_window = window;
    init(window? true: false);
}

void ToolBar::init(bool hasTopWindow)
{
    //layout
    QAction *newWindowAction = addAction(QIcon::fromTheme("window-new-symbolic", QIcon::fromTheme("folder")),
                                         tr("Open in new &Window"));
    QAction *newTabActon = addAction(QIcon::fromTheme("tab-new-symbolic", QIcon::fromTheme("folder")),
                                     tr("Open in new &Tab"));

    addSeparator();

    //view switch
    //FIXME: how about support uri?
    auto viewManager = DirectoryViewFactoryManager::getInstance();

    auto defaultViewId = viewManager->getDefaultViewId();

    QComboBox *viewCombox = new QComboBox(this);
    viewCombox->setToolTip(tr("Change Directory View"));
    m_view_option_box = viewCombox;
    auto model = new ViewFactorySortFilterModel(this);
    m_view_factory_model = model;
    model->setDirectoryUri("file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    viewCombox->setModel(model);

    addWidget(viewCombox);
    connect(viewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        auto viewId = viewCombox->itemData(index, Qt::ToolTipRole).toString();
        m_top_window->beginSwitchView(viewId);
    });

    addSeparator();

    //file operations
    QAction *copyAction = addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("&Copy"));
    copyAction->setShortcut(QKeySequence::Copy);

    QAction *pasteAction = addAction(QIcon::fromTheme("edit-paste-symbolic"), tr("Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);

    QAction *cutAction = addAction(QIcon::fromTheme("edit-cut-symbolic"), tr("Cut"));
    cutAction->setShortcut(QKeySequence::Cut);

    QAction *trashAction = addAction(QIcon::fromTheme("edit-delete-symbolic"), tr("Trash"));
    trashAction->setShortcut(QKeySequence::Delete);

    m_file_op_actions<<copyAction<<pasteAction<<cutAction<<trashAction;

    addSeparator();

    //advance usage
    //...

    //separator widget

    //extension

    //other options?

    //trash
    m_clean_trash_action = addAction(QIcon::fromTheme("edit-clear-symbolic"), tr("Clean Trash"), [=](){
        auto result = QMessageBox::question(nullptr, tr("Delete Permanently"), tr("Are you sure that you want to delete these files? "
                                                                                  "Once you start a deletion, the files deleting will never be "
                                                                                  "restored again."));
        if (result == QMessageBox::Yes) {
            auto uris = m_top_window->getCurrentAllFileUris();
            qDebug()<<uris;
            FileOperationUtils::remove(uris);
        }
    });

    m_restore_action = addAction(QIcon::fromTheme("view-refresh-symbolic"), tr("Restore"), [=](){
        FileOperationUtils::restore(m_top_window->getCurrentSelections());
    });

    //connect signal
    connect(newWindowAction, &QAction::triggered, [=](){
        if (m_top_window->getCurrentSelections().isEmpty()) {
            FMWindow *newWindow = new FMWindow(m_top_window->getCurrentUri());
            newWindow->show();
            //FIXME: show when prepared
        } else {
            for (auto uri : m_top_window->getCurrentSelections()) {
                FMWindow *newWindow = new FMWindow(m_top_window->getCurrentUri());
                newWindow->show();
                //FIXME: show when prepared
            }
        }
    });

    connect(newTabActon, &QAction::triggered, [=](){
        QStringList l;
        if (m_top_window->getCurrentSelections().isEmpty()) {
            l<<m_top_window->getCurrentUri();
        } else {
            l = m_top_window->getCurrentSelections();
        }
        m_top_window->addNewTabs(l);
    });

    connect(viewCombox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [=](const QString &text){
        Q_EMIT this->optionRequest(SwitchView);
        //FIXME: i have to add interface to view proxy for view switch.
    });

    connect(copyAction, &QAction::triggered, [=](){
        if (!m_top_window->getCurrentSelections().isEmpty())
            ClipboardUtils::setClipboardFiles(m_top_window->getCurrentSelections(), false);
    });

    connect(pasteAction, &QAction::triggered, [=](){
        if (ClipboardUtils::isClipboardHasFiles()) {
            //FIXME: how about duplicated copy?
            //FIXME: how to deal with a failed move?
            ClipboardUtils::pasteClipboardFiles(m_top_window->getCurrentUri());
        }
    });
    connect(cutAction, &QAction::triggered, [=](){
        if (!m_top_window->getCurrentSelections().isEmpty()) {
            ClipboardUtils::setClipboardFiles(m_top_window->getCurrentSelections(), true);
        }
    });
    connect(trashAction, &QAction::triggered, [=](){
        if (!m_top_window->getCurrentSelections().isEmpty()) {
            FileOperationUtils::trash(m_top_window->getCurrentSelections(), true);
        }
    });

    //extension

    //trash
}

void ToolBar::updateLocation(const QString &uri)
{
    if (uri.isNull())
        return;

    bool isFileOpDisable = uri.startsWith("trash://") || uri.startsWith("search://");
    for (auto action : m_file_op_actions) {
        action->setEnabled(!isFileOpDisable);
    }

    m_view_factory_model->setDirectoryUri(uri);

    auto viewId = m_top_window->getCurrentPage()->getProxy()->getView()->viewId();
    auto index = m_view_factory_model->getIndexFromViewId(viewId);
    if (index.isValid())
        m_view_option_box->setCurrentIndex(index.row());
    else {
        m_view_option_box->setCurrentIndex(0);
    }

    m_clean_trash_action->setVisible(uri.startsWith("trash://"));
    m_restore_action->setVisible(uri.startsWith("trash://"));
}

void ToolBar::updateStates()
{
    if (!m_top_window)
        return;

    auto directory = m_top_window->getCurrentUri();
    auto selection = m_top_window->getCurrentSelections();

    if (directory.startsWith("trash://")) {
        auto files = m_top_window->getCurrentAllFileUris();
        m_clean_trash_action->setEnabled(!files.isEmpty());
        m_restore_action->setEnabled(!selection.isEmpty());
    }
}
