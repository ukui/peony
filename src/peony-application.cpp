#include "peony-application.h"
#include "menu-plugin-iface.h"
#include "file-info.h"

#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QString>
#include <QMenu>
#include <QTimer>
#include <QVBoxLayout>

#include "preview-page-factory-manager.h"
#include "preview-page-plugin-iface.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include "standard-view-proxy.h"
#include "directory-view-container.h"

#include "path-edit.h"
#include "location-bar.h"
#include <QStandardPaths>
#include <QStackedLayout>

#include "tool-bar.h"
#include <QMainWindow>

#include "tab-page.h"

#include "side-bar.h"

#include "navigation-tool-bar.h"

#include "navigation-bar.h"

#include "fm-window.h"

#include <QFile>

#include <QStyleFactory>

#include "search-vfs-register.h"

#include <QMessageBox>

#include "menu-plugin-manager.h"
#include "directory-view-menu.h"
#include "icon-view.h"

#include "plugin-manager.h"

#include "list-view.h"

#include "basic-properties-page.h"

PeonyApplication::PeonyApplication(int &argc, char *argv[]) : QApplication (argc, argv)
{
    Peony::PluginManager::init();

    auto testIcon = QIcon::fromTheme("folder");
    if (testIcon.isNull()) {
        QIcon::setThemeName("ukui-icon-theme");
        QMessageBox::warning(nullptr, tr("Warning"), tr("Peony-Qt can not get the system's icon theme. "
                                                        "There are 2 reasons might lead to this problem:\n\n"
                                                        "1. Peony-Qt might be running as root, "
                                                        "that means you have the higher permission "
                                                        "and can do some things which normally forbidden. "
                                                        "But, you should learn that if you were in a "
                                                        "root, the virtual file system will lose some "
                                                        "featrue such as you can not use \"My Computer\", "
                                                        "the theme and icons might also went wrong. So, run "
                                                        "peony-qt in a root is not recommended.\n\n"
                                                        "2. You are using a non-qt theme for your system but "
                                                        "you didn't install the platform theme plugin for qt's "
                                                        "applications. If you are using gtk-theme, try installing "
                                                        "the qt5-gtk2-platformtheme package to resolve this problem."));
    }
    Peony::SearchVFSRegister::registSearchVFS();
    //QIcon::setThemeName("ukui-icon-theme-one");
    setAttribute(Qt::AA_UseHighDpiPixmaps);
    //setAttribute(Qt::AA_EnableHighDpiScaling);

    QFile file(":/data/libpeony-qt-styled.qss");
    file.open(QFile::ReadOnly);
    setStyleSheet(QString::fromLatin1(file.readAll()));
    qDebug()<<file.readAll();
    file.close();


    //setStyle(QStyleFactory::create("windows"));

    //check if first run
    //if not send message to server
    //else
    //  load plgin
    //  read from command line
    //  do with args
#ifdef MENU
    QDir pluginsDir(qApp->applicationDirPath());
    qDebug()<<pluginsDir;
    pluginsDir.cdUp();
    pluginsDir.cd("testdir");
    pluginsDir.setFilter(QDir::Files);

    qDebug()<<pluginsDir.entryList().count();
    Q_FOREACH(QString fileName, pluginsDir.entryList(QDir::Files)) {
        qDebug()<<fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        qDebug()<<pluginLoader.fileName();
        qDebug()<<pluginLoader.metaData();
        qDebug()<<pluginLoader.load();
        QObject *plugin = pluginLoader.instance();
        qDebug()<<"test start";
        if (plugin) {
            Peony::MenuPluginInterface *iface = qobject_cast<Peony::MenuPluginInterface *>(plugin);
            if (iface) {
                qDebug()<<iface->testPlugin()<<iface->name()<<iface->description();

                QWidget *widget = new QWidget;
                widget->setAttribute(Qt::WA_DeleteOnClose);
                QMenu *menu = new QMenu(widget);
                QStringList l;
                Peony::MenuPluginInterface::Types types = Peony::MenuPluginInterface::Types(Peony::MenuPluginInterface::File|
                                                                                            Peony::MenuPluginInterface::Volume|
                                                                                            Peony::MenuPluginInterface::DirectoryBackground|
                                                                                            Peony::MenuPluginInterface::DesktopBackground|
                                                                                            Peony::MenuPluginInterface::Other);
                auto fileActions = iface->menuActions(types, nullptr, l);
                for (auto action : fileActions) {
                    action->setParent(menu);
                }
                menu->addActions(fileActions);

                widget->setContextMenuPolicy(Qt::CustomContextMenu);
                widget->connect(widget, &QWidget::customContextMenuRequested, [menu](const QPoint /*&pos*/){
                    //menu->exec(pos);
                    menu->exec(QCursor::pos());
                });
                widget->show();
            }
        }
        qDebug()<<"testEnd";
    }
#endif

//#define PREVIEW
#ifdef PREVIEW
    qDebug()<<"preview test";
    auto previewManager = Peony::PreviewPageFactoryManager::getInstance();
    qDebug()<<previewManager->getPluginNames();
    for (auto name : previewManager->getPluginNames()) {
        auto plugin = previewManager->getPlugin(name);
        auto w = plugin->createPreviewPage();
        w->startPreview();
        QTimer::singleShot(1000, [=](){
            w->cancel();
        });
        auto w1 = dynamic_cast<QWidget*>(w);
        w1->setAttribute(Qt::WA_DeleteOnClose);
        w1->show();
    }

#endif

//#define DIRECTORY_VIEW
#ifdef DIRECTORY_VIEW
    QDir pluginsDir(qApp->applicationDirPath());
    qDebug()<<pluginsDir;
    pluginsDir.cdUp();
    pluginsDir.cd("testdir2");
    pluginsDir.setFilter(QDir::Files);

    qDebug()<<"directory view test";
    auto directoryViewManager = Peony::DirectoryViewFactoryManager::getInstance();
    qDebug()<<directoryViewManager->getFactoryNames();
    auto names = directoryViewManager->getFactoryNames();
    for (auto name : names) {
        qDebug()<<name;
        auto factory = directoryViewManager->getFactory(name);
        auto view = factory->create();
        auto proxy = new Peony::DirectoryView::StandardViewProxy(view);
        view->setProxy(proxy);
        //BUG: it is not safe loading a new uri when
        //a directory is loading. enve thoug the file enumemeration
        //is cancelled and, the async method from GFileEnumerator
        //might still run in thread and return.
        //This cause program went crash.

        //view->setDirectoryUri("file:///");
        //view->beginLocationChange();
        //view->stopLocationChange();
        qDebug()<<"2";
        //proxy->setDirectoryUri("network:///");
        //proxy->setDirectoryUri("file:///home/lanyue");
        proxy->setDirectoryUri("file:///");
        proxy->beginLocationChange();
        QTimer::singleShot(1000, [=](){
            //proxy->invertSelections();
        });
        connect(proxy, &Peony::DirectoryViewProxyIface::viewDoubleClicked, [=](const QString &uri){
            qDebug()<<"double clicked"<<uri;
            auto info = Peony::FileInfo::fromUri(uri);
            if (info->isDir() || info->isVolume() || uri.startsWith("network:")) {
                proxy->setDirectoryUri(uri);
                proxy->beginLocationChange();
            }
        });

        auto widget = dynamic_cast<QWidget*>(view);

        //widget->setAttribute(Qt::WA_DeleteOnClose);
        QWidget *container = new QWidget;
        container->setAttribute(Qt::WA_DeleteOnClose);
        widget->setParent(container);
        QVBoxLayout *layout = new QVBoxLayout(container);

        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);

        container->setLayout(layout);
        container->layout()->addWidget(widget);

        container->show();
    }
#endif

//#define DIRECTORY_VIEW2
#ifdef DIRECTORY_VIEW2
    QMainWindow *w = new QMainWindow;
    QToolBar *t = new QToolBar(w);
    Peony::PathEdit *e = new Peony::PathEdit;
    e->setUri("file:///");
    t->addWidget(e);
    w->addToolBar(Qt::TopToolBarArea, t);

    auto tabPage = new Peony::TabPage(w);
    w->setCentralWidget(tabPage);

    w->connect(e, &QLineEdit::returnPressed, [=](){
        tabPage->addPage(e->text());
    });

    w->show();

#endif

//#define PATH_EDIT
#ifdef PATH_EDIT

    //Peony::PathEdit *edit = new Peony::PathEdit;
    //edit->setUri("file:///home/lanyue");
    //edit->show();

    QWidget *widget = new QWidget;
    widget->setAttribute(Qt::WA_DeleteOnClose);

    QStackedLayout *layout = new QStackedLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    Peony::LocationBar *bar = new Peony::LocationBar;
    Peony::PathEdit *edit = new Peony::PathEdit(widget);
    bar->setRootUri("file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    bar->connect(bar, &Peony::LocationBar::groupChangedRequest, bar, &Peony::LocationBar::setRootUri);
    bar->connect(bar, &Peony::LocationBar::blankClicked, [=](){
        layout->setCurrentWidget(edit);
        edit->setUri(bar->getCurentUri());
    });
    edit->setUri(bar->getCurentUri());
    edit->connect(edit, &Peony::PathEdit::returnPressed, [=](){
        bar->setRootUri(edit->text());
        layout->setCurrentWidget(bar);
    });
    edit->connect(edit, &Peony::PathEdit::editCancelled, [=](){
        layout->setCurrentWidget(bar);
    });
    layout->addWidget(bar);
    layout->addWidget(edit);

    widget->setLayout(layout);
    widget->setFixedHeight(edit->height());
    widget->show();
#endif

//#define TOOLBAR
#ifdef TOOLBAR

    Peony::ToolBar *toolbar = new Peony::ToolBar;
    QMainWindow *w = new QMainWindow;
    w->addToolBar(toolbar);

    w->show();

#endif

//#define SIDEBAR
#ifdef SIDEBAR
    auto sidebar = new Peony::SideBar;
    sidebar->show();
#endif

//#define NAVIGATION_BAR
#ifdef NAVIGATION_BAR
    QMainWindow *w = new QMainWindow;
    auto nbar = new Peony::NavigationBar;
    w->addToolBar(Qt::TopToolBarArea, nbar);
    w->show();
#endif

#define FM_WINDOW
#ifdef FM_WINDOW
    auto window = new Peony::FMWindow("file:///");
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
#endif

//#define MENU
#ifdef MENU
    Peony::DirectoryView::IconView *view = new Peony::DirectoryView::IconView;
    view->setDirectoryUri("file:///");
    view->beginLocationChange();
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->show();
#endif

//#define LIST_VIEW
#ifdef LIST_VIEW
    Peony::DirectoryView::ListView *listView = new Peony::DirectoryView::ListView;
    auto model = new Peony::FileItemModel;
    auto proxyModel = new Peony::FileItemProxyFilterSortModel;

    listView->bindModel(model, proxyModel);
    listView->setDirectoryUri("file:///");
    listView->beginLocationChange();

    listView->show();
#endif

//#define BASIC_PROPERTIES_PAGE
#ifdef BASIC_PROPERTIES_PAGE
    Peony::BasicPropertiesPage *p = new Peony::BasicPropertiesPage("file:///");
    p->show();
#endif
}
