#include "tool-bar.h"
#include "fm-window.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include "clipboard-utils.h"
#include "file-operation-utils.h"

#include "view-factory-model.h"

#include <QAction>
#include <QComboBox>

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
    QAction *newWindowAction = addAction(QIcon::fromTheme("window", QIcon::fromTheme("folder")),
                                         tr("Open in new &Window"));
    QAction *newTabActon = addAction(QIcon::fromTheme("open", QIcon::fromTheme("folder")),
                                     tr("Open in new &Tab"));
    QAction *newTerminalAction = addAction(QIcon::fromTheme("terminal", QIcon::fromTheme("folder")),
                                           tr("Open in Terminal"));

    addSeparator();

    //view switch
    //FIXME: how about support uri?
    auto viewManager = DirectoryViewFactoryManager::getInstance();

    auto defaultViewId = viewManager->getDefaultViewId();

    QComboBox *viewCombox = new QComboBox(this);
    auto model = new ViewFactoryModel(this);
    model->setDirectoryUri("file:///");
    viewCombox->setModel(model);

    addWidget(viewCombox);

    addSeparator();

    //file operations
    QAction *copyAction = addAction(QIcon::fromTheme("gtk-copy"), tr("&Copy"));

    QAction *pasteAction = addAction(QIcon::fromTheme("gtk-paste"), tr("Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);

    QAction *cutAction = addAction(QIcon::fromTheme("gtk-cut"), tr("Cut"));
    cutAction->setShortcut(QKeySequence::Cut);

    QAction *trashAction = addAction(QIcon::fromTheme("user-trash"), tr("Trash"));
    trashAction->setShortcut(QKeySequence::Delete);

    addSeparator();

    //advance usage

    //other options

    //connect signal
    if (!hasTopWindow) {
        connect(newWindowAction, &QAction::triggered, [=](){
            Q_EMIT this->optionRequest(OpenInNewWindow);
        });
        connect(newTabActon, &QAction::triggered, [=](){
            Q_EMIT this->optionRequest(OpenInNewTab);
        });
        connect(newTerminalAction, &QAction::triggered, [=](){
            Q_EMIT this->optionRequest(OpenInTerminal);
        });

        connect(viewCombox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [=](const QString &text){
            Q_EMIT this->optionRequest(SwitchView);
        });

        connect(copyAction, &QAction::triggered, [=](){
            Q_EMIT this->optionRequest(Copy);
        });
        connect(pasteAction, &QAction::triggered, [=](){
            Q_EMIT this->optionRequest(Paste);
        });
        connect(cutAction, &QAction::triggered, [=](){
            Q_EMIT this->optionRequest(Cut);
        });
        connect(trashAction, &QAction::triggered, [=](){
            Q_EMIT this->optionRequest(Trash);
        });
    } else {
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

        connect(newTerminalAction, &QAction::triggered, [=](){
            auto uri = m_top_window->getCurrentUri();
            //FIXME: call terminal
            Q_EMIT this->optionRequest(OpenInTerminal);
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
                if (ClipboardUtils::isClipboardFilesBeCut()) {
                    FileOperationUtils::move(ClipboardUtils::getClipboardFilesUris(), m_top_window->getCurrentUri(), true);
                    //FIXME: how to deal with a failed move?
                    ClipboardUtils::clearClipboard();
                } else {
                    FileOperationUtils::copy(ClipboardUtils::getClipboardFilesUris(), m_top_window->getCurrentUri(), true);
                }
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
    }
}
