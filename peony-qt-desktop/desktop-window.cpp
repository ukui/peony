#include "desktop-window.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-info.h"
#include "file-info-job.h"
#include "file-launch-manager.h"

#include "desktop-menu.h"

#include "desktop-item-model.h"
#include "desktop-icon-view.h"

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"
#include "file-trash-operation.h"
#include "clipboard-utils.h"

#include "peony-desktop-application.h"
#include "singleapplication.h"

#include <QStandardPaths>
#include <QLabel>

#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>

#include <QItemSelectionModel>

#include <QGraphicsOpacityEffect>

#include <QFileDialog>

#include <QProcess>

#include <QDebug>

using namespace Peony;

DesktopWindow::DesktopWindow(QScreen *screen, bool is_primary, QWidget *parent)
    : QStackedWidget(parent)
{
    m_screen = screen;
    m_is_primary = is_primary;
    setContentsMargins(0, 0, 0, 0);
    m_layout = static_cast<QStackedLayout*>(this->layout());
    m_layout->setStackingMode(QStackedLayout::StackAll);
    qDebug()<<"DesktopWindow is_primary:"<<is_primary<<screen->objectName()<<screen->name();
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setAttribute(Qt::WA_TranslucentBackground);

    setGeometry(screen->geometry());
    setFixedSize(screen->size());

    setContextMenuPolicy(Qt::CustomContextMenu);

    m_trans_timer.setSingleShot(true);
    m_opacity_effect = new QGraphicsOpacityEffect(this);

    m_bg_font = new QLabel(this);
    m_bg_font->setContentsMargins(0, 0, 0, 0);
    m_bg_back = new QLabel(this);
    m_bg_back->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_bg_font);
    m_layout->addWidget(m_bg_back);

    setBg(getCurrentBgPath());

    connect(m_screen, &QScreen::geometryChanged, this, &DesktopWindow::geometryChangedProcess);
    connect(m_screen, &QScreen::virtualGeometryChanged, this, &DesktopWindow::virtualGeometryChangedProcess);

    if (! m_is_primary)
    {
        m_view = nullptr;
        return;
    }

    connect(m_screen, &QScreen::availableGeometryChanged, this, &DesktopWindow::availableGeometryChangedProcess);

    m_view = new DesktopIconView(this);
    m_layout->addWidget(m_view);
    setCurrentWidget(m_view);
    qDebug()<<"create view:"<<m_screen->availableGeometry();
    m_view->setFixedSize(m_screen->availableGeometry().size());
    m_view->setGeometry(m_screen->availableGeometry());

    connect(m_view, &QListView::doubleClicked, [=](const QModelIndex &index){
        qDebug()<<"double click"<<index.data(FileItemModel::UriRole);
        auto uri = index.data(FileItemModel::UriRole).toString();
        auto info = FileInfo::fromUri(uri, false);
        auto job = new FileInfoJob(info);
        job->setAutoDelete();
        job->connect(job, &FileInfoJob::queryAsyncFinished, [=](){
            if (info->isDir() || info->isVolume() || info->isVirtual()) {
                QProcess p;
                p.setProgram("peony-qt");
                p.setArguments(QStringList()<<uri);
                p.startDetached();
            } else {
                FileLaunchManager::openAsync(uri);
            }
        });
        job->queryAsync();
    });

    //edit trigger

    //menu
    connect(m_view, &QListView::customContextMenuRequested, [=](const QPoint &pos){
        //FIXME: use other menu
        qDebug()<<"menu request";
        if (!m_view->indexAt(pos).isValid())
            m_view->clearSelection();

        QTimer::singleShot(1, [=](){
            DesktopMenu menu(m_view);
            if (m_view->getSelections().isEmpty()) {
                auto action = menu.addAction(tr("set background"));
                connect(action, &QAction::triggered, [=](){
                    QFileDialog dlg;
                    dlg.setNameFilters(QStringList()<<"*.jpg"<<"*.png");
                    if (dlg.exec()) {
                        auto url = dlg.selectedUrls().first();
                        this->setBg(url.path());
                        //qDebug()<<url;
                        Q_EMIT this->changeBg(url.path());
                    }
                });
            }
            menu.exec(QCursor::pos());
        });
    });

    initShortcut();
}

DesktopWindow::~DesktopWindow()
{

}

const QString DesktopWindow::getCurrentBgPath()
{
    //FIXME: implement custom bg settings storage
    if (m_current_bg_path.isEmpty()) {
        m_current_bg_path = "/usr/share/backgrounds/ubuntukylin-default-settings.jpg";
    }
    return m_current_bg_path;
}

void DesktopWindow::setBg(const QString &path)
{
    qDebug()<<path;
    if (path.isNull()) {
        return;
    }

    m_bg_back_pixmap = m_bg_font_pixmap;
    m_bg_back->setPixmap(m_bg_back_pixmap);

    m_bg_font_pixmap = QPixmap(path);
    //FIXME: implement different pixmap clip algorithm.
    m_bg_font_pixmap = m_bg_font_pixmap.scaled(m_screen->size(),
                                               Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation);
    m_bg_font->setPixmap(m_bg_font_pixmap);

    m_opacity_effect->setOpacity(0);
    m_bg_font->setGraphicsEffect(m_opacity_effect);
    m_opacity = 0;
    m_trans_timer.start(50);

    m_trans_timer.connect(&m_trans_timer, &QTimer::timeout, [=](){
        qDebug()<<m_opacity;
        if (m_opacity > 0.95) {
            m_opacity = 1.0;
            m_bg_back_pixmap.detach();
            m_bg_back->setPixmap(QPixmap());
            m_trans_timer.stop();
            return;
        }
        m_opacity += 0.05;
        m_opacity_effect->setOpacity(m_opacity);
        m_bg_font->setGraphicsEffect(m_opacity_effect);
        m_trans_timer.start(50);
    });
}

void DesktopWindow::setScreen(QScreen *screen)
{
    m_screen = screen;
}

void DesktopWindow::setIsPrimary(bool is_primary)
{
    m_is_primary = is_primary;
}

void DesktopWindow::connectSignal()
{
    connect(m_screen, &QScreen::geometryChanged, this, &DesktopWindow::geometryChangedProcess);
    connect(m_screen, &QScreen::virtualGeometryChanged, this, &DesktopWindow::virtualGeometryChangedProcess);

    if (m_is_primary)
    {
        connect(m_screen, &QScreen::availableGeometryChanged, this, &DesktopWindow::availableGeometryChangedProcess);
    }
}

void DesktopWindow::disconnectSignal()
{
    disconnect(m_screen, &QScreen::geometryChanged, this, &DesktopWindow::geometryChangedProcess);
    disconnect(m_screen, &QScreen::virtualGeometryChanged, this, &DesktopWindow::virtualGeometryChangedProcess);

    if (m_is_primary)
    {
        disconnect(m_screen, &QScreen::availableGeometryChanged, this, &DesktopWindow::availableGeometryChangedProcess);
    }
}

void DesktopWindow::scaleBg(const QRect &geometry)
{
    QString path = getCurrentBgPath();
    if (path.isNull()) {
        return;
    }

    qDebug()<<"scaleBg:"<<m_screen->geometry()<<geometry;
    m_bg_font_pixmap = QPixmap(path);
    //FIXME: implement different pixmap clip algorithm.
    m_bg_font_pixmap = m_bg_font_pixmap.scaled(geometry.size(),
                                               Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation);

    m_bg_font->setPixmap(m_bg_font_pixmap);
    m_bg_back_pixmap = m_bg_font_pixmap;
    m_bg_back->setPixmap(m_bg_back_pixmap);
}

void DesktopWindow::initShortcut()
{
    //shotcut
    QAction *copyAction = new QAction(this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, [=](){
        auto selectedUris = m_view->getSelections();
        if (!selectedUris.isEmpty())
            ClipboardUtils::setClipboardFiles(selectedUris, false);
    });
    addAction(copyAction);

    QAction *cutAction = new QAction(this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, [=](){
        auto selectedUris = m_view->getSelections();
        if (!selectedUris.isEmpty())
            ClipboardUtils::setClipboardFiles(selectedUris, true);
    });
    addAction(cutAction);

    QAction *pasteAction = new QAction(this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, [=](){
        auto clipUris = ClipboardUtils::getClipboardFilesUris();
        if (ClipboardUtils::isClipboardHasFiles()) {
            auto uris = ClipboardUtils::getClipboardFilesUris();
            if (ClipboardUtils::isClipboardFilesBeCut()) {
                auto op = new FileMoveOperation(uris, m_view->getDirectoryUri());
                FileOperationManager::getInstance()->startOperation(op, true);
            } else {
                auto op = new FileCopyOperation(uris, m_view->getDirectoryUri());
                FileOperationManager::getInstance()->startOperation(op, true);
            }
        }
    });
    addAction(pasteAction);

    QAction *trashAction = new QAction(this);
    trashAction->setShortcut(QKeySequence::Delete);
    connect(trashAction, &QAction::triggered, [=](){
        auto selectedUris = m_view->getSelections();
        if (!selectedUris.isEmpty()) {
            auto op = new FileTrashOperation(selectedUris);
            FileOperationManager::getInstance()->startOperation(op, true);
        }
    });
    addAction(trashAction);

    QAction *undoAction = new QAction(this);
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, [=](){
        FileOperationManager::getInstance()->undo();
    });
    addAction(undoAction);

    QAction *redoAction = new QAction(this);
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, [=](){
        FileOperationManager::getInstance()->redo();
    });
    addAction(redoAction);

    QAction *zoomInAction = new QAction(this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, [=](){
        m_view->zoomIn();
    });
    addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, [=](){
        m_view->zoomOut();
    });
    addAction(zoomOutAction);
}

void DesktopWindow::availableGeometryChangedProcess(const QRect &geometry)
{
    qDebug()<<"availableGeometryChangedProcess"<<geometry<<m_screen->geometry()<<m_screen->availableGeometry()<<m_screen->name();
    updateView();
}

void DesktopWindow::virtualGeometryChangedProcess(const QRect &geometry)
{
    qDebug()<<"virtualGeometryChangedProcess"<<geometry<<m_screen->geometry()<<m_screen->virtualGeometry()<<m_screen->name();
    this->setGeometry(m_screen->geometry());
    scaleBg(m_screen->geometry());
}

void DesktopWindow::geometryChangedProcess(const QRect &geometry)
{
    //screen resolution ratio change
    qDebug()<<"geometryChangedProcess:"<<geometry<<m_screen->geometry()<<this->geometry()<<m_screen->name();
    updateWinGeometry();
    scaleBg(geometry);
}

void DesktopWindow::updateView()
{
    if (m_view)
    {
        qDebug()<<"updateView"<<m_screen->name()<<m_screen->availableGeometry()<<this->geometry();
        m_view->setGeometry(m_screen->availableGeometry());
        m_view->setFixedSize(m_screen->availableGeometry().size());
    }
}

void DesktopWindow::updateWinGeometry()
{
    qDebug()<<"befoere updateWinGeometry:"<<this->objectName()<<this->getScreen()->geometry()<<this->geometry()<<this->getScreen()->virtualGeometry();
    this->setGeometry(m_screen->geometry());
    this->setFixedSize(m_screen->geometry().size());
    qDebug()<<"end updateWinGeometry:"<<this->objectName()<<this->getScreen()->geometry()<<this->geometry()<<this->getScreen()->virtualGeometry();
}
