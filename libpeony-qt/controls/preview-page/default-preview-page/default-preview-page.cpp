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
 *
 */

#include "default-preview-page.h"
#include "thumbnail-manager.h"

#include <QLabel>

#include <QFile>
#include <QFileInfo>
#include <QUrl>

#include <QPixmap>
#include <QIcon>
#include <QMimeDatabase>

#include <QGridLayout>
#include <QFormLayout>

#include <QLocale>
#include <QDateTime>

#include <QThreadPool>

#include <QResizeEvent>

#include <QImageReader>

#include <QPainter>

#include "icon-container.h"

#include "file-info.h"
#include "file-info-manager.h"
#include "file-watcher.h"

#include "file-info-job.h"

#include "file-count-operation.h"

using namespace Peony;

DefaultPreviewPage::DefaultPreviewPage(QWidget *parent) : QStackedWidget (parent)
{
    auto label = new QLabel(tr("Select the file you want to preview..."), this);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignCenter);
    m_empty_tab_widget = label;

    /*
    label = new QLabel(this);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignCenter);
    */
    auto previewPage = new FilePreviewPage(this);
    previewPage->installEventFilter(this);
    m_preview_tab_widget = previewPage;

    addWidget(m_preview_tab_widget);
    addWidget(m_empty_tab_widget);

    setCurrentWidget(m_empty_tab_widget);
}

DefaultPreviewPage::~DefaultPreviewPage()
{
    cancel();
}

bool DefaultPreviewPage::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == m_preview_tab_widget) {
        if (ev->type() == QEvent::Resize) {
            auto e = static_cast<QResizeEvent*>(ev);
            auto page = qobject_cast<FilePreviewPage*>(m_preview_tab_widget);
            int width = e->size().width()/3;
            width = width>96? width: 96;
            page->resizeIcon(QSize(width, width));
        }
    }
    return QStackedWidget::eventFilter(obj, ev);
}

void DefaultPreviewPage::prepare(const QString &uri, PreviewType type)
{
    m_current_uri = uri;
    m_info = FileInfo::fromUri(uri);
    m_current_type = type;
    //m_support = uri.contains("file:///");
    if (uri.contains("file:///") || uri.contains("recent:///")){
        m_support = true;
    }

    m_watcher = std::make_shared<FileWatcher>(uri);
    connect(m_watcher.get(), &FileWatcher::locationChanged, [=](const QString &, const QString &newUri) {
        this->prepare(newUri);
        this->startPreview();
    });
    m_watcher->startMonitor();
}

void DefaultPreviewPage::prepare(const QString &uri)
{
    prepare(uri, Other);
}

void DefaultPreviewPage::startPreview()
{
    if (m_support) {
        auto previewPage = qobject_cast<FilePreviewPage*>(m_preview_tab_widget);
        auto info = FileInfo::fromUri(m_current_uri);
        previewPage->updateInfo(m_info.get());
        setCurrentWidget(previewPage);
    } else {
        QLabel *label = qobject_cast<QLabel*>(m_empty_tab_widget);
        label->setText(tr("Can not preview this file."));
    }
}

void DefaultPreviewPage::cancel()
{
    m_preview_tab_widget->cancel();
    setCurrentWidget(m_empty_tab_widget);
    QLabel *label = qobject_cast<QLabel*>(m_empty_tab_widget);
    label->setText(tr("Select the file you want to preview..."));
}

void DefaultPreviewPage::closePreviewPage()
{
    cancel();
    deleteLater();
}

void DefaultPreviewPage::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(this->rect(), this->palette().base());
    QStackedWidget::paintEvent(e);
}

FilePreviewPage::FilePreviewPage(QWidget *parent) : QFrame(parent)
{
    m_layout = new QGridLayout(this);
    setLayout(m_layout);

    m_icon = new IconContainer(this);
    m_icon->setIconSize(QSize(96, 96));
    m_layout->addWidget(m_icon);

    m_form = new QFormLayout(this);
    m_display_name_label = new QLabel(this);
    m_display_name_label->setWordWrap(true);
    m_form->addRow(tr("File Name:"), m_display_name_label);
    m_type_label = new QLabel(this);
    m_form->addRow(tr("File Type:"), m_type_label);
    m_time_access_label = new QLabel(this);
    m_form->addRow(tr("Time Access:"), m_time_access_label);
    m_time_modified_label = new QLabel(this);
    m_form->addRow(tr("Time Modified:"), m_time_modified_label);
    m_file_count_label = new QLabel(this);
    m_form->addRow(tr("Children Count:"), m_file_count_label);
    m_total_size_label = new QLabel(this);
    m_form->addRow(tr("Size:"), m_total_size_label);

    //image
    m_image_size = new QLabel(this);
    m_form->addRow(tr("Image size:"), m_image_size);
    m_image_format = new QLabel(this);
    m_form->addRow(tr("Image format:"), m_image_format);

    m_form->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    m_form->setFormAlignment(Qt::AlignHCenter);
    m_form->setLabelAlignment(Qt::AlignRight);

    QWidget *form = new QWidget(this);
    form->setLayout(m_form);
    m_layout->addWidget(form, 1, 0);
}

FilePreviewPage::~FilePreviewPage()
{

}

void FilePreviewPage::updateInfo(FileInfo *info)
{
    if (info->displayName().isEmpty()) {
        FileInfoJob j(info->uri());
        j.querySync();
    }
    auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(info->uri());
    if (!thumbnail.isNull()) {
        QUrl url = info->uri();
        thumbnail.addFile(url.path());
    }
    auto icon = QIcon::fromTheme(info->iconName(), QIcon::fromTheme("text-x-generic"));
    m_icon->setIcon(thumbnail.isNull()? icon: thumbnail);
    //m_icon->setIcon(info->thumbnail().isNull()? QIcon::fromTheme(info->iconName()): info->thumbnail());
    m_display_name_label->setText(info->displayName());
    m_type_label->setText(info->fileType());

    QLocale locale;
    auto access = QDateTime::fromMSecsSinceEpoch(info->accessTime()*1000);
    auto modify = QDateTime::fromMSecsSinceEpoch(info->modifiedTime()*1000);
    if (locale.language() == QLocale::Chinese)
    {
        m_time_access_label->setText(access.toString(Qt::SystemLocaleShortDate));
        m_time_modified_label->setText(modify.toString(Qt::SystemLocaleShortDate));
    }
    else
    {
        m_time_access_label->setText(access.toString(Qt::ISODate));
        m_time_modified_label->setText(modify.toString(Qt::ISODate));
    }

    m_file_count_label->setText(tr(""));
    if (info->isDir()) {
        m_form->itemAt(4, QFormLayout::LabelRole)->widget()->setVisible(true);
        m_file_count_label->setVisible(true);
    } else {
        m_form->itemAt(4, QFormLayout::LabelRole)->widget()->setVisible(false);
        m_file_count_label->setVisible(false);
    }

    if (info->mimeType().startsWith("image/")) {
        QUrl url = info->uri();
        QImageReader r(url.path());
        auto image_size_row_left = m_form->itemAt(6, QFormLayout::LabelRole)->widget();
        image_size_row_left->setVisible(true);

        m_image_size->setText(tr("%1x%2").arg(r.size().width()).arg(r.size().height()));
        auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(info->uri());
        bool rgba = thumbnail.pixmap(r.size()).hasAlphaChannel();
        m_image_size->setVisible(true);
        auto image_format_row_left = m_form->itemAt(7, QFormLayout::LabelRole)->widget();
        image_format_row_left->setVisible(true);
        m_image_format->setText(rgba? "RGBA": "RGB");
        m_image_format->setVisible(true);
    } else {
        auto image_size_row_left = m_form->itemAt(6, QFormLayout::LabelRole)->widget();
        image_size_row_left->setVisible(false);
        m_image_size->setVisible(false);
        auto image_format_row_left = m_form->itemAt(7, QFormLayout::LabelRole)->widget();
        image_format_row_left->setVisible(false);
        m_image_format->setVisible(false);
    }
    if (info->fileType().startsWith("video/")) {

    }
    if (info->fileType().startsWith("audio/")) {

    }

    countAsync(info->uri());
}

void FilePreviewPage::countAsync(const QString &uri)
{
    cancel();
    m_file_count = 0;
    m_hidden_count = 0;
    m_total_size = 0;

    QStringList uris;
    uris<<uri;
    auto info = FileInfo::fromUri(uri);
    m_count_op = new FileCountOperation(uris, !info->isDir());
    connect(m_count_op, &FileOperation::operationStarted, this, &FilePreviewPage::resetCount, Qt::BlockingQueuedConnection);
    connect(m_count_op, &FileOperation::operationPreparedOne, this, &FilePreviewPage::onPreparedOne, Qt::BlockingQueuedConnection);
    connect(m_count_op, &FileCountOperation::countDone, this, &FilePreviewPage::onCountDone, Qt::BlockingQueuedConnection);
    QThreadPool::globalInstance()->start(m_count_op);
}

void FilePreviewPage::updateCount()
{
    m_file_count_label->setText(tr("%1 total, %2 hidden").arg(m_file_count).arg(m_hidden_count));
    //auto format = g_format_size(m_total_size);
    //Calculated by 1024 bytes
    auto format = g_format_size_full(m_total_size,G_FORMAT_SIZE_IEC_UNITS);
    m_total_size_label->setText(format);
    g_free(format);
}

void FilePreviewPage::cancel()
{
    if (m_count_op) {
        m_count_op->blockSignals(true);
        m_count_op->cancel();
        onCountDone();
    }
    m_count_op = nullptr;
}

void FilePreviewPage::resizeIcon(QSize size)
{
    m_icon->setIconSize(size);
}

void FilePreviewPage::resetCount()
{
    m_file_count = 0;
    m_hidden_count = 0;
    m_total_size = 0;
    updateCount();
}

void FilePreviewPage::onCountDone()
{
    if (!m_count_op)
        return;
    m_count_op->getInfo(m_file_count, m_hidden_count, m_total_size);
    this->updateCount();
    m_count_op = nullptr;
    m_file_count = 0;
    m_hidden_count = 0;
    m_total_size = 0;
}
