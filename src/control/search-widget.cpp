#include "search-widget.h"
#include "advanced-location-bar.h"
#include "search-vfs-uri-parser.h"
#include <QToolButton>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QHBoxLayout>
#include <QDebug>
#include <QStandardPaths>
#include <QParallelAnimationGroup>

using namespace Peony;

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    m_closeSearchButton = new QToolButton(this);
    m_closeSearchButton->setAutoRaise(false);
    m_closeSearchButton->setMinimumSize(QSize(46, 46));
    m_closeSearchButton->setIconSize(QSize(16, 16));
    m_closeSearchButton->setToolTip("");
    m_closeSearchButton->setVisible(false);

    m_locationBar = new AdvancedLocationBar(this);
    m_searchButton = new QToolButton(this);
    m_searchButton->setIcon(QIcon::fromTheme("edit-find-symbolic"));
    m_searchButton->setToolTip(tr("Search"));
    m_searchButton->setAutoRaise(false);
    m_searchButton->setMinimumSize(QSize(46, 46));
    m_searchButton->setIconSize(QSize(16, 16));

    connect(m_locationBar, &AdvancedLocationBar::refreshRequest, this, &SearchWidget::refreshRequest);

    connect(m_locationBar, &AdvancedLocationBar::updateFileTypeFilter, this, &SearchWidget::updateFileTypeFilter);

    connect(m_locationBar, &AdvancedLocationBar::searchRequest, [=](const QString &path, const QString &key){
        //key is null, clean search content, show all files
        if (key == "" || key.isNull()) {
            Q_EMIT this->updateLocationRequest(path, false);
        } else {
            if (m_searchGlobal) {
                QString homePath = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
                auto targetUri = Peony::SearchVFSUriParser::parseSearchKey(homePath, key, true, false, "", m_searchRecursive);
                targetUri = targetUri.replace("&recursive=0", "&recursive=1");
                Q_EMIT this->updateLocationRequest(targetUri, false);
            } else {
                auto targetUri = Peony::SearchVFSUriParser::parseSearchKey(path, key, true, false, "", m_searchRecursive);
                targetUri = targetUri.replace("&recursive=1", "&recursive=0");
                Q_EMIT this->updateLocationRequest(targetUri, false);
            }
        }
    });

    connect(m_locationBar, &AdvancedLocationBar::updateWindowLocationRequest, this, &SearchWidget::updateLocationRequest);
    connect(this, &SearchWidget::updateLocation, m_locationBar, &AdvancedLocationBar::updateLocation);
    connect(this, &SearchWidget::cancelEdit, m_locationBar, &AdvancedLocationBar::cancelEdit);
    connect(this, &SearchWidget::finishEdit, m_locationBar, &AdvancedLocationBar::finishEdit);

    layout->addWidget(m_closeSearchButton);
    layout->addWidget(m_locationBar);
    layout->addWidget(m_searchButton);
    layout->setSpacing(9);
    initAnimation();
}

void SearchWidget::initAnimation()
{
    QPropertyAnimation *m_locationBarAnimation = new QPropertyAnimation(m_locationBar, "geometry");
    m_locationBarAnimation->setDuration(300);
    m_locationBarAnimation->setEasingCurve(QEasingCurve::OutQuad);

    QPropertyAnimation *m_searchAnimation = new QPropertyAnimation(m_searchButton, "geometry");
    m_searchAnimation->setDuration(300);
    m_searchAnimation->setEasingCurve(QEasingCurve::OutQuad);

    QPropertyAnimation *m_closeLocationBarAnimation = new QPropertyAnimation(m_locationBar, "geometry");
    m_closeLocationBarAnimation->setDuration(300);
    m_closeLocationBarAnimation->setEasingCurve(QEasingCurve::OutQuad);

    QPropertyAnimation *m_closeSearchAnimation = new QPropertyAnimation(m_closeSearchButton, "geometry");
    m_closeSearchAnimation->setDuration(300);
    m_closeSearchAnimation->setEasingCurve(QEasingCurve::OutQuad);

    QParallelAnimationGroup *m_closeSearchGroup = new QParallelAnimationGroup(this);
    m_closeSearchGroup->addAnimation(m_closeLocationBarAnimation);
    m_closeSearchGroup->addAnimation(m_searchAnimation);
    m_closeSearchGroup->setDirection(QAbstractAnimation::Forward);

    QParallelAnimationGroup *m_searchGroup = new QParallelAnimationGroup(this);
    m_searchGroup->addAnimation(m_locationBarAnimation);
    m_searchGroup->addAnimation(m_closeSearchAnimation);
    m_searchGroup->setDirection(QAbstractAnimation::Forward);

    connect(m_searchButton, &QToolButton::clicked, this, &SearchWidget::searchButtonClicked);
    connect(m_closeSearchButton, &QToolButton::clicked, this, &SearchWidget::searchButtonClicked);
    connect(this, &SearchWidget::changeSearchMode, this, [=](bool mode) {
        int actionSize = 46;
        QRect locationBarRect = m_locationBar->geometry();
        QRect searchRect = m_searchButton->geometry();
        auto width  = locationBarRect.width();
        auto height  = locationBarRect.height();
        auto y = locationBarRect.y();
        if (mode) {
            m_locationBarAnimation->setStartValue(QRect(this->width() - actionSize - 9, y, actionSize, height));
            m_locationBarAnimation->setEndValue(QRect(this->width() - width - 9, y, width,height));
            m_closeSearchAnimation->setStartValue(QRect(9, searchRect.y(), width,actionSize));
            m_closeSearchAnimation->setEndValue(QRect(9, searchRect.y(), actionSize,actionSize));
            m_searchGroup->start();
        } else {
            m_closeLocationBarAnimation->setStartValue(QRect(9, y, actionSize,height));
            m_closeLocationBarAnimation->setEndValue(QRect(9, y,width,height));
            m_searchAnimation->setStartValue(QRect(actionSize + 9, searchRect.y(), width,actionSize));
            m_searchAnimation->setEndValue(QRect(this->width() - actionSize - 9, searchRect.y(), actionSize,actionSize));
            m_locationBar->setAnimationMode(true);
            m_closeSearchGroup->start();
        }
    });
    connect(m_closeSearchGroup, &QPropertyAnimation::finished, this, [=] {
        m_locationBar->setAnimationMode(false);
    });
}

void SearchWidget::searchButtonClicked()
{
    m_searchMode = ! m_searchMode;
    m_searchButton->setVisible(! m_searchMode);
    qDebug() << "searchButtonClicked" <<m_searchMode;
    Q_EMIT this->updateSearchRequest(m_searchMode);
    setSearchMode(m_searchMode);
    Q_EMIT changeSearchMode(m_searchMode);

}

void SearchWidget::setSearchMode(bool mode)
{
    m_searchButton->setCheckable(mode);
    m_searchButton->setChecked(mode);
    m_searchButton->setDown(mode);
    m_closeSearchButton->setVisible(mode);
    m_locationBar->switchEditMode(mode);
}

void SearchWidget::closeSearch()
{
    m_searchMode = false;
    m_searchButton->setVisible(true);
    m_closeSearchButton->setVisible(false);
    setSearchMode(false);
}

void SearchWidget::startEdit(bool bSearch)
{
    //qDebug() << "bSearch" <<bSearch <<m_searchMode;
    if (bSearch && m_searchMode) {
        return;
    }

    if (bSearch) {
        searchButtonClicked();
    } else {
        m_searchButton->setVisible(true);
        m_searchButton->setCheckable(false);
        m_closeSearchButton->setVisible(false);
        m_locationBar->startEdit();
        m_locationBar->switchEditMode(false);
        if (m_searchMode) {
            Q_EMIT changeSearchMode(false);
            Q_EMIT updateSearchRequest(false);
        }
        m_searchMode = false;
    }
}

void SearchWidget::setGlobalFlag(bool isGlobal)
{
    m_searchGlobal = isGlobal;
    m_locationBar->deselectSearchBox();
}

void SearchWidget::updateSearchRecursive(bool recursive)
{
    m_searchRecursive = recursive;
}

void SearchWidget::updateCloseSearch(QString icon)
{
     m_closeSearchButton->setIcon(QIcon::fromTheme(icon, QIcon::fromTheme("folder")));
     m_closeSearchButton->setToolTip(icon);

}
