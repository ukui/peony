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
 * Authors: MeihongHe <hemeihong@kylinos.cn>
 *
 */

#include "tab-status-bar.h"
#include "file-info.h"
#include "file-utils.h"
#include "search-vfs-uri-parser.h"
#include "tab-widget.h"

#include "global-settings.h"
#include "main-window.h"

#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QStyle>

#include <QUrl>

#include <QToolBar>
#include <QSlider>
#include <QDebug>

TabStatusBar::TabStatusBar(TabWidget *tab, QWidget *parent) : QStatusBar(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    m_styled_toolbar = new QToolBar;

    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("padding: 0;");
    setSizeGripEnabled(false);
    setMinimumHeight(30);

    m_tab = tab;
    m_label = new ElidedLabel(this);
    m_label->setContentsMargins(25, 0, 0, 0);
    addWidget(m_label, 1);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0, 100);

    auto mainWindow = qobject_cast<MainWindow *>(this->topLevelWidget());
    auto settings = Peony::GlobalSettings::getInstance();
    int defaultZoomLevel = settings->getValue(DEFAULT_VIEW_ID).toInt();
    if (mainWindow) {
        defaultZoomLevel = mainWindow->currentViewZoomLevel();
    }
    m_slider->setValue(defaultZoomLevel);

    connect(m_slider, &QSlider::valueChanged, this, &TabStatusBar::zoomLevelChangedRequest);
}

TabStatusBar::~TabStatusBar()
{
    m_styled_toolbar->deleteLater();
}

int TabStatusBar::currentZoomLevel()
{
    if (m_slider->isEnabled()) {
        m_slider->value();
    }
    return -1;
}

void TabStatusBar::update()
{
    if (!m_tab)
        return;

    auto selections = m_tab->getCurrentSelectionFileInfos();
    if (! selections.isEmpty()) {
        QString directoriesString;
        int directoryCount = 0;
        QString filesString;
        int fileCount = 0;
        goffset size = 0;
        for (auto selection : selections) {
            if(selection->isDir()) {
                directoryCount++;
            } else if (!selection->isVolume()) {
                fileCount++;
                size += selection->size();
            }
        }
        auto format_size = g_format_size(size);
        if (selections.count() == 1) {
            if (directoryCount == 1)
                directoriesString = QString(", %1").arg(selections.first()->displayName());
            if (fileCount == 1)
                filesString = QString(", %1, %2").arg(selections.first()->displayName()).arg(format_size);
        } else if (directoryCount > 1 && (fileCount > 1)) {
            directoriesString = tr("; %1 folders").arg(directoryCount);
            filesString = tr("; %1 files, %2 total").arg(fileCount).arg(format_size);
        } else if (directoryCount > 1 && (fileCount > 1)) {
            directoriesString = tr("; %1 folder").arg(directoryCount);
            filesString = tr("; %1 file, %2").arg(fileCount).arg(format_size);
        } else if (fileCount == 0) {
            directoriesString = tr("; %1 folders").arg(directoryCount);
        } else {
            filesString = tr("; %1 files, %2 total").arg(fileCount).arg(format_size);
        }

        m_label->setText(tr("%1 selected").arg(selections.count()) + directoriesString + filesString);
        //showMessage(tr("%1 files selected ").arg(selections.count()));
        g_free(format_size);
    }
    else {
        auto uri = m_tab->getCurrentUri();
        auto displayName = Peony::FileUtils::getFileDisplayName(uri);
        //qDebug() << "status bar text:" <<displayName <<uri;
        if (uri.startsWith("search:///"))
        {
            QString nameRegexp = Peony::SearchVFSUriParser::getSearchUriNameRegexp(uri);
            QString targetDirectory = Peony::SearchVFSUriParser::getSearchUriTargetDirectory(uri);
            displayName = tr("Search \"%1\" in \"%2\"").arg(nameRegexp).arg(targetDirectory);
            m_label->setText(displayName);
        }
        else {
            QUrl url = m_tab->getCurrentUri();
            m_label->setText(url.toDisplayString());
        }
    }
}

void TabStatusBar::update(const QString &message)
{
    m_label->setText(message);
}

void TabStatusBar::updateZoomLevelState(int zoomLevel)
{
    m_slider->setValue(zoomLevel);
}

void TabStatusBar::onZoomRequest(bool zoomIn)
{
    int value = m_slider->value();
    if (zoomIn) {
        value++;
    } else {
        value--;
    }
    m_slider->setValue(value);
}


void TabStatusBar::paintEvent(QPaintEvent *e)
{
    return;
}

void TabStatusBar::mousePressEvent(QMouseEvent *e)
{
    return;
}

void TabStatusBar::resizeEvent(QResizeEvent *e)
{
    QStatusBar::resizeEvent(e);
    auto pos = this->rect().topRight();
    auto size = m_slider->size();
    m_slider->move(pos.x() - size.width() - 20, this->size().height()/2 - size.height()/2);
}

ElidedLabel::ElidedLabel(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setContentsMargins(30, 0, 120, 0);
}

void ElidedLabel::setText(const QString &text)
{
    m_text = text;
    this->update();
}

void ElidedLabel::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    bool active = opt.state &QStyle::State_Active;

    QColor base = active? qApp->palette().color(QPalette::Active, QPalette::Base): qApp->palette().color(QPalette::Inactive, QPalette::Base);

    QFontMetrics fm(this->font());
    auto elidedText = fm.elidedText(m_text, Qt::TextElideMode::ElideRight, this->size().width() - 150);
    QPainter p(this);
    //p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    QLinearGradient linearGradient;
    linearGradient.setStart(QPoint(10, this->height()));
    linearGradient.setFinalStop(QPoint(10, 0));
    linearGradient.setColorAt(0, base);
    linearGradient.setColorAt(0.75, base);
    linearGradient.setColorAt(1, Qt::transparent);

    int overlap = qApp->style()->pixelMetric(QStyle::PM_ScrollView_ScrollBarOverlap);
    int layoutWidth = qApp->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int adjustedY2 = qMin(0, overlap - layoutWidth);

    QPainterPath path;
    path.addRect(this->rect().adjusted(0, 0, adjustedY2 - this->height(), 0));

    p.fillPath(path, linearGradient);

    QPainterPath path2;

    int radius = this->height();
    QPoint pos = QPoint(this->width() + adjustedY2 - this->height(), this->height());
    //QRect targetRect = QRect(pos.x() - radius, pos.y() - radius, radius*2, radius*2);
    path2.moveTo(pos);
    //path2.arcTo(targetRect, 0, 90);

    QRadialGradient radialGradient;
    radialGradient.setCenter(pos);
    radialGradient.setFocalPoint(pos);
    radialGradient.setRadius(radius);
    radialGradient.setColorAt(0, base);
    radialGradient.setColorAt(0.75, base);
    radialGradient.setColorAt(1, Qt::transparent);
    p.fillPath(path2, radialGradient);

    style()->drawItemText(&p, this->rect().adjusted(30, 0, -120, 0), Qt::AlignLeft, qApp->palette(), this->isEnabled(), elidedText, QPalette::WindowText);
}

