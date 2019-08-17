#include "side-bar-file-system-item.h"
#include "side-bar-model.h"
#include "file-utils.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info.h"
#include "gobject-template.h"
#include "file-watcher.h"

using namespace Peony;

SideBarFileSystemItem::SideBarFileSystemItem(QString uri,
                                             SideBarFileSystemItem *parentItem,
                                             SideBarModel *model,
                                             QObject *parent) : SideBarAbstractItem (model, parent)
{
    m_parent = parentItem;

    if (parentItem == nullptr) {
        m_is_root_child = true;
        m_uri = "computer:///";
        m_display_name = tr("Computer");
        m_icon_name = "computer";
    } else {
        m_uri = uri;
        m_display_name = FileUtils::getFileDisplayName(uri);
        m_icon_name = FileUtils::getFileIconName(uri);
    }

    m_watcher = std::make_shared<FileWatcher>(uri);
    connect(m_watcher.get(), &FileWatcher::fileCreated, [=](const QString &uri){
        qDebug()<<"created:"<<uri;
        SideBarFileSystemItem *item = new SideBarFileSystemItem(uri,
                                                                this,
                                                                m_model);
        m_children->append(item);
        if (m_uri == "computer:///") {
            //check is mounted.
            auto targetUri = FileUtils::getTargetUri(uri);
            m_is_mounted = !targetUri.isEmpty();
        }
        m_model->insertRows(m_children->count() - 1, 1, firstColumnIndex());
    });

    connect(m_watcher.get(), &FileWatcher::fileDeleted, [=](const QString &uri){
        qDebug()<<"deleted:"<<uri;
        for (auto child : *m_children) {
            if (child->uri() == uri) {
                int index = m_children->indexOf(child);
                m_children->removeOne(child);
                child->deleteLater();
                m_model->removeRows(index, 1, firstColumnIndex());
                if (m_uri == "computer:///") {
                    //check is mounted.
                    auto targetUri = FileUtils::getTargetUri(uri);
                    m_is_mounted = !targetUri.isEmpty();
                }
                break;
            }
        }
    });
}

QModelIndex SideBarFileSystemItem::firstColumnIndex()
{
    return m_model->firstCloumnIndex(this);
}

QModelIndex SideBarFileSystemItem::lastColumnIndex()
{
    return m_model->lastCloumnIndex(this);
}

void SideBarFileSystemItem::clearChildren()
{
    m_watcher->stopMonitor();
    SideBarAbstractItem::clearChildren();
}

void SideBarFileSystemItem::findChildren()
{
    clearChildren();

    FileEnumerator *e = new FileEnumerator;
    e->setEnumerateDirectory(m_uri);
    connect(e, &FileEnumerator::prepared, [=](const GErrorWrapperPtr &err){
        e->enumerateSync();
        auto infos = e->getChildren();
        if (infos.isEmpty())
            goto end;
        for (auto info: infos) {
            SideBarFileSystemItem *item = new SideBarFileSystemItem(info->uri(),
                                                                    this,
                                                                    m_model);
            //check is mounted.
            auto targetUri = FileUtils::getTargetUri(info->uri());
            m_is_mounted = !targetUri.isEmpty();
            m_children->append(item);
            qDebug()<<info->uri();
        }
        //BUG: insert failed???!!!
        //it seems that the children of items is not rechable.
        m_model->insertRows(0, infos.count(), firstColumnIndex());
end:
        Q_EMIT this->findChildrenFinished();
        if (err != nullptr) {
            qDebug()<<"prepared:"<<err.get()->message();
        }
        delete e;
    });
    e->prepare();

    m_watcher->startMonitor();

    Q_EMIT findChildrenFinished();
}

void SideBarFileSystemItem::findChildrenAsync()
{
    //TODO add async method.
    findChildren();
}

bool SideBarFileSystemItem::isRemoveable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {

    }
    return false;
}

bool SideBarFileSystemItem::isEjectable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {

    }
    return false;
}

bool SideBarFileSystemItem::isMountable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {
        return true;
    }
    return false;
}

bool SideBarFileSystemItem::isMounted()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {
        return m_is_mounted;
    }
    return false;
}
