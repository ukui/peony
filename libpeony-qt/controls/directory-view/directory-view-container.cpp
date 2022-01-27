/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 * Authors: Meihong He <hemeihong@kylinos.cn>
 *
 */

#include "directory-view-container.h"
#include "directory-view-plugin-iface.h"
#include "directory-view-plugin-iface2.h"
#include "directory-view-widget.h"
#include "directory-view-factory-manager.h"
#include "file-utils.h"
#include "global-settings.h"

#include "file-label-model.h"

#include "directory-view-factory-manager.h"

#include "file-item-proxy-filter-sort-model.h"

#include "file-info.h"

#include <QVBoxLayout>
#include <QAction>

#include <QApplication>

using namespace Peony;

DirectoryViewContainer::DirectoryViewContainer(QWidget *parent) : QWidget(parent)
{
    m_model = new FileItemModel(this);
    m_proxy_model = new FileItemProxyFilterSortModel(this);
    m_proxy_model->setSourceModel(m_model);

    //m_proxy = new DirectoryView::StandardViewProxy;

    setContentsMargins(0, 0, 0, 0);
    m_layout = new QVBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    setLayout(m_layout);

//    connect(m_proxy, &DirectoryViewProxyIface::viewDirectoryChanged,
//            this, &DirectoryViewContainer::directoryChanged);

//    connect(m_proxy, &DirectoryViewProxyIface::viewSelectionChanged,
//            this, &DirectoryViewContainer::selectionChanged);

//    connect(m_proxy, &DirectoryViewProxyIface::menuRequest,
//            this, &DirectoryViewContainer::menuRequest);
    connect(m_model, &FileItemModel::changePathRequest, this, &DirectoryViewContainer::signal_responseUnmounted);

    this->setProperty("statusBarUpdate", false);
    connect(m_model, &FileItemModel::updated, [=](){
        if(this->property("statusBarUpdate").isValid() && this->property("statusBarUpdate").toBool() == false){
            this->setProperty("statusBarUpdate", true);
            QTimer::singleShot(400, this, [=](){
                Q_EMIT this->statusBarChanged();
                this->setProperty("statusBarUpdate", false);
            });
        }
    });

    connect(FileLabelModel::getGlobalModel(), &FileLabelModel::dataChanged, this, [=](){
        refresh();
    });

    if (QGSettings::isSchemaInstalled("org.ukui.control-center.panel.plugins")) {
        m_control_center_plugin = new QGSettings("org.ukui.control-center.panel.plugins", QByteArray(), this);
        connect(m_control_center_plugin, &QGSettings::changed, this, [=](const QString &key) {
           qDebug() << "panel settings changed:" <<key;
           if (getView()->viewId() == "List View" && (key == "date" || key == "hoursystem"))
              refresh();
        });
    }
}

DirectoryViewContainer::~DirectoryViewContainer()
{
//    m_proxy->closeProxy();
//    if (m_proxy->getView())
//        m_proxy->getView()->closeView();
}

const QStringList DirectoryViewContainer::getBackList()
{
    QStringList l;
    for (auto uri : m_back_list) {
        l<<uri;
    }
    return l;
}

const QStringList DirectoryViewContainer::getForwardList()
{
    QStringList l;
    for (auto uri : m_forward_list) {
        l<<uri;
    }
    return l;
}

bool DirectoryViewContainer::canGoBack()
{
    return !m_back_list.isEmpty();
}

void DirectoryViewContainer::goBack()
{
    if (!canGoBack())
        return;

    auto uri = m_back_list.takeLast();
    m_forward_list.prepend(getCurrentUri());
    Q_EMIT updateWindowLocationRequest(uri, false);
}

bool DirectoryViewContainer::canGoForward()
{
    return !m_forward_list.isEmpty();
}

void DirectoryViewContainer::goForward()
{
    if (!canGoForward())
        return;

    auto uri = m_forward_list.takeFirst();
    m_back_list.append(getCurrentUri());
    Q_EMIT updateWindowLocationRequest(uri, false);
}

bool DirectoryViewContainer::canCdUp()
{
    if (!m_view)
        return false;
    return !FileUtils::getParentUri(m_view->getDirectoryUri()).isNull();
}

void DirectoryViewContainer::cdUp()
{
    if (!canCdUp())
        return;

    auto uri = FileUtils::getParentUri(m_view->getDirectoryUri());
    if (uri.isNull())
        return;

    Q_EMIT updateWindowLocationRequest(uri, true);
}

void DirectoryViewContainer::setSortFilter(int FileTypeIndex, int FileMTimeIndex, int FileSizeIndex)
{
    qDebug()<<"setSortFilter:"<<FileTypeIndex<<"MTime:"<<FileMTimeIndex<<"size:"<<FileSizeIndex;
    m_proxy_model->setFilterConditions(FileTypeIndex, FileMTimeIndex, FileSizeIndex);
}

void DirectoryViewContainer::setFilterLabelConditions(QString name)
{
    m_proxy_model->setFilterLabelConditions(name);
}

void DirectoryViewContainer::setShowHidden(bool showHidden)
{
    m_proxy_model->setShowHidden(showHidden);
}

void DirectoryViewContainer::setUseDefaultNameSortOrder(bool use)
{
    m_proxy_model->setUseDefaultNameSortOrder(use);
}

void DirectoryViewContainer::setSortFolderFirst(bool folderFirst)
{
    m_proxy_model->setFolderFirst(folderFirst);
}

void DirectoryViewContainer::addFilterCondition(int option, int classify, bool updateNow)
{
    m_proxy_model->addFilterCondition(option, classify, updateNow);
}

void DirectoryViewContainer::addFileNameFilter(QString key, bool updateNow)
{
    m_proxy_model->addFileNameFilter(key, updateNow);
}

void DirectoryViewContainer::removeFilterCondition(int option, int classify, bool updateNow)
{
    m_proxy_model->removeFilterCondition(option, classify, updateNow);
}

void DirectoryViewContainer::clearConditions()
{
    m_proxy_model->clearConditions();
}

void DirectoryViewContainer::updateFilter()
{
    m_proxy_model->update();
}

void DirectoryViewContainer::goToUri(const QString &uri, bool addHistory, bool forceUpdate)
{
    int zoomLevel = -1;
    if (m_view)
        zoomLevel = m_view->currentZoomLevel();

    if (forceUpdate)
        goto update;

    if (uri.isNull())
        return;

    if (getCurrentUri() == uri)
        return;

update:
    if (addHistory) {
        m_forward_list.clear();
        //qDebug() << "getCurrentUri():" <<getCurrentUri()<<uri;
        //fix bug 41094, avoid go back to same path issue
        if (! getCurrentUri().startsWith("search://")
            && !FileUtils::isSamePath(getCurrentUri(), uri)) {
            m_back_list.append(getCurrentUri());
        }
    }

    auto viewId = DirectoryViewFactoryManager2::getInstance()->getDefaultViewId(zoomLevel, uri);
    switchViewType(viewId);

    //update status bar zoom level
    updateStatusBarSliderStateRequest();
    if (zoomLevel < 0)
        zoomLevel = getView()->currentZoomLevel();

    setZoomLevelRequest(zoomLevel);
    //qDebug() << "setZoomLevelRequest:" <<zoomLevel;
    if (m_view)
        m_view->setCurrentZoomLevel(zoomLevel);

    m_current_uri = uri;

    //special uri process
    if (m_current_uri.endsWith("/."))
        m_current_uri = m_current_uri.left(m_current_uri.length()-2);
    if (m_current_uri.endsWith("/.."))
        m_current_uri = m_current_uri.left(m_current_uri.length()-3);

    if (m_view) {
        m_view->setDirectoryUri(m_current_uri);
        m_view->beginLocationChange();
        //m_active_view_prxoy->setDirectoryUri(uri);
    }
    updatePreviewPageRequest();
}

void DirectoryViewContainer::switchViewType(const QString &viewId)
{
    /*
    if (!m_proxy)
        return;

    if (m_proxy->getView()) {
        if (viewId == m_proxy->getView()->viewId())
            return;
    }
    */

    if (getView()) {
        if (getView()->viewId() == viewId)
            return;
    }

    auto viewManager = DirectoryViewFactoryManager2::getInstance();
    auto factory = viewManager->getFactory(viewId);
    if (!factory)
        return;

    auto settings = GlobalSettings::getInstance();
    auto sortType = settings->isExist(SORT_COLUMN)? settings->getValue(SORT_COLUMN).toInt() : 0;
    auto sortOrder = settings->isExist(SORT_ORDER)? settings->getValue(SORT_ORDER).toInt() : 0;

    auto oldView = m_view;
    QStringList selection;
    if (oldView) {
        sortType = oldView->getSortType();
        sortOrder = oldView->getSortOrder();
        selection = oldView->getSelections();
        m_layout->removeWidget(dynamic_cast<QWidget*>(oldView));
        oldView->deleteLater();
    }
    auto view = factory->create();
    m_view = view;
    view->setParent(this);
    //connect the view's signal.
    view->bindModel(m_model, m_proxy_model);
    //view->setProxy(m_proxy);

    //fix go to root path issue after refresh
    view->setDirectoryUri(getCurrentUri());

    view->setSortType(sortType);
    view->setSortOrder(sortOrder);

    connect(m_view, &DirectoryViewWidget::menuRequest, this, &DirectoryViewContainer::menuRequest);
    connect(m_view, &DirectoryViewWidget::viewDirectoryChanged, this, [=](){
        if (DirectoryViewFactoryManager2::getInstance()->internalViews().contains(m_view->viewId())) {
            auto dirInfo = FileInfo::fromUri(m_current_uri);
            if (dirInfo.get()->isEmptyInfo() && !dirInfo.get()->uri().startsWith("search://")) {
                goBack();
                if (!m_forward_list.isEmpty())
                    m_forward_list.takeFirst();
            } else {
                Q_EMIT this->directoryChanged();
            }
        } else {
            Q_EMIT this->directoryChanged();
        }
    });
    connect(m_view, &DirectoryViewWidget::viewDoubleClicked, this, &DirectoryViewContainer::viewDoubleClicked);
    connect(m_view, &DirectoryViewWidget::viewDoubleClicked, this, &DirectoryViewContainer::onViewDoubleClicked);
    connect(m_view, &DirectoryViewWidget::viewSelectionChanged, this, &DirectoryViewContainer::selectionChanged);

    connect(m_view, &DirectoryViewWidget::zoomRequest, this, &DirectoryViewContainer::zoomRequest);

    connect(m_view, &DirectoryViewWidget::updateWindowSelectionRequest, this, &DirectoryViewContainer::updateWindowSelectionRequest);

    //similar to double clicked, but just jump directory only.
    //note that if view use double clicked signal, this signal should
    //not sended again.
    connect(m_view, &DirectoryViewWidget::updateWindowLocationRequest, this, [=](const QString &uri) {
        Q_EMIT this->updateWindowLocationRequest(uri, true);
    });
    connect(m_view, &DirectoryViewWidget::signal_itemAdded, this, [=](const QString& uri) {
        Q_EMIT this->signal_itemAdded(uri);
    });

    //m_proxy->switchView(view);
    m_layout->addWidget(dynamic_cast<QWidget*>(view), Qt::AlignBottom);
    DirectoryViewFactoryManager2::getInstance()->setDefaultViewId(viewId);
    if (!selection.isEmpty()) {
        view->setSelections(selection);
    }

    QAction *cdUpAction = new QAction(m_view);
    cdUpAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_Up));
    connect(cdUpAction, &QAction::triggered, this, [=]() {
        this->cdUp();
    });
    this->addAction(cdUpAction);

    QAction *goBackAction = new QAction(m_view);
    goBackAction->setShortcut(QKeySequence::Back);
    connect(goBackAction, &QAction::triggered, this, [=]() {
        this->goBack();
    });
    this->addAction(goBackAction);

    QAction *goForwardAction = new QAction(m_view);
    goForwardAction->setShortcut(QKeySequence::Forward);
    connect(goForwardAction, &QAction::triggered, this, [=]() {
        this->goForward();
    });
    this->addAction(goForwardAction);

    QAction *editAction = new QAction(m_view);
    editAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_E)<<Qt::Key_F2);
    connect(editAction, &QAction::triggered, this, [=]() {
        auto selections = m_view->getSelections();

        bool hasStandardPath = FileUtils::containsStandardPath(selections);
        if (selections.count() == 1 && !hasStandardPath) {
            QString one = selections.first();
            if(one.startsWith("filesafe:///") && one.remove("filesafe:///").indexOf("/") == -1) {
                return ;
            }
            m_view->editUri(selections.first());
        }
    });
    this->addAction(editAction);

    Q_EMIT viewTypeChanged();
}

void DirectoryViewContainer::refresh()
{
    if (!m_view)
        return;
    m_view->beginLocationChange();
}

void DirectoryViewContainer::bindNewProxy(DirectoryViewProxyIface *proxy)
{
    //disconnect old proxy
    //connect new proxy
}

const QStringList DirectoryViewContainer::getCurrentSelections()
{
    if (m_view)
        return m_view->getSelections();
    return QStringList();
}

const QString DirectoryViewContainer::getCurrentUri()
{
    if (m_view) {
        return m_view->getDirectoryUri();
    }
    return nullptr;
}

const QStringList DirectoryViewContainer::getAllFileUris()
{
    if (m_view)
        return m_view->getAllFileUris();
    return QStringList();
}

void DirectoryViewContainer::stopLoading()
{
    if (m_view) {
        m_view->stopLocationChange();
        Q_EMIT this->directoryChanged();
    }
}

void DirectoryViewContainer::tryJump(int index)
{
    QStringList l;
    l<<m_back_list<<getCurrentUri()<<m_forward_list;
    if (0 <= index && index < l.count()) {
        auto targetUri = l.at(index);
        m_back_list.clear();
        m_forward_list.clear();
        for (int i = 0; i < l.count(); i++) {
            if (i < index) {
                m_back_list<<l.at(i);
            }
            if (i > index) {
                m_forward_list<<l.at(i);
            }
        }
        Q_EMIT updateWindowLocationRequest(targetUri, false, true);
    }
}

FileItemModel::ColumnType DirectoryViewContainer::getSortType()
{
    if (!m_view)
        return FileItemModel::FileName;
    int type = m_view->getSortType();
    return FileItemModel::ColumnType(type);
}

void DirectoryViewContainer::setSortType(FileItemModel::ColumnType type)
{
    if (!m_view)
        return;
    m_view->setSortType(type);
    Peony::GlobalSettings::getInstance()->setValue(SORT_COLUMN, type);
}

Qt::SortOrder DirectoryViewContainer::getSortOrder()
{
    if (!m_view)
        return Qt::AscendingOrder;
    int order = m_view->getSortOrder();
    return Qt::SortOrder(order);
}

void DirectoryViewContainer::setSortOrder(Qt::SortOrder order)
{
    if (order < 0)
        return;
    if (!m_view)
        return;
    Peony::GlobalSettings::getInstance()->setValue(SORT_ORDER, order);
    m_view->setSortOrder(order);
}

void DirectoryViewContainer::onViewDoubleClicked(const QString& uri)
{

}
