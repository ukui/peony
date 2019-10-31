#include "desktop-window.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-info.h"
#include "file-info-job.h"
#include "file-launch-manager.h"

#include "directory-view-menu.h"

#include "desktop-item-model.h"
#include "desktop-icon-view.h"

#include <QStandardPaths>
#include <QLabel>

#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QStackedLayout>

#include <QItemSelectionModel>

#include <QGraphicsOpacityEffect>

#include <QFileDialog>

#include <QProcess>

#include <QDebug>

using namespace Peony;

static QModelIndex m_last_index = QModelIndex();

DesktopWindow::DesktopWindow(QWidget *parent)
    : QStackedWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    QStackedLayout *layout = static_cast<QStackedLayout*>(this->layout());
    layout->setStackingMode(QStackedLayout::StackAll);
    qDebug()<<QApplication::primaryScreen()->geometry();
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setAttribute(Qt::WA_TranslucentBackground);

    setGeometry(QApplication::primaryScreen()->geometry());
    setFixedSize(QApplication::primaryScreen()->size());

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(qApp, &QApplication::primaryScreenChanged, [=](QScreen *screen){
        qDebug()<<"primary screen changed";
        this->setFixedSize(screen->size());
    });

    m_trans_timer.setSingleShot(true);
    m_opacity_effect = new QGraphicsOpacityEffect(this);

    m_bg_font = new QLabel(this);
    m_bg_font->setContentsMargins(0, 0, 0, 0);
    m_bg_back = new QLabel(this);
    m_bg_back->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_bg_font);
    layout->addWidget(m_bg_back);

    auto model = new DesktopItemModel(this);

    m_view = new DesktopIconView(this);

    m_view->setModel(model);
    layout->addWidget(m_view);
    setCurrentWidget(m_view);
    m_view->setFixedSize(QApplication::primaryScreen()->availableGeometry().size());
    m_view->setGeometry(QApplication::primaryScreen()->availableGeometry());

    setBg(getCurrentBgPath());

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

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection){
        qDebug()<<"selection changed";
        auto currentSelections = selection.indexes();

        for (auto index : deselection.indexes()) {
            m_view->setIndexWidget(index, nullptr);
        }

        if (currentSelections.count() == 1) {
            m_last_index = currentSelections.first();
        } else {
            m_last_index = QModelIndex();
        }
    });

    //edit trigger

    //menu
    connect(m_view, &QListView::customContextMenuRequested, [=](const QPoint &pos){
        //FIXME: use other menu
        QMenu menu;
        auto action = menu.addAction(tr("set background"));
        connect(action, &QAction::triggered, [=](){
            QFileDialog dlg;
            dlg.setNameFilters(QStringList()<<"*.jpg"<<"*.png");
            if (dlg.exec()) {
                auto url = dlg.selectedUrls().first();
                this->setBg(url.path());
                qDebug()<<url;
            }
        });
        menu.addAction(tr("zoom in"), [=](){
            m_view->zoomIn();
            m_view->update();
        });
        menu.addAction(tr("zoom out"), [=](){
            m_view->zoomOut();
            m_view->update();
        });
        menu.exec(QCursor::pos());
        qDebug()<<"menu request";
    });
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
    m_bg_font_pixmap = m_bg_font_pixmap.scaled(QApplication::primaryScreen()->size(),
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
            m_bg_back->setPixmap(m_bg_font_pixmap);
            m_trans_timer.stop();
            return;
        }
        m_opacity += 0.05;
        m_opacity_effect->setOpacity(m_opacity);
        m_bg_font->setGraphicsEffect(m_opacity_effect);
        m_trans_timer.start(50);
    });
}
