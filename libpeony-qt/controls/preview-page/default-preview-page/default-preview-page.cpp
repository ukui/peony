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
#include <QGSettings>

#include "icon-container.h"

#include "file-info.h"
#include "file-info-manager.h"
#include "file-watcher.h"

#include "file-info-job.h"

#include "file-count-operation.h"

#include "FMWindowIface.h"

using namespace Peony;

#define LABEL_MAX_WIDTH       165

DefaultPreviewPage::DefaultPreviewPage(QWidget *parent) : QStackedWidget (parent)
{
    setContentsMargins(10, 20, 10, 20);

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

    if (QGSettings::isSchemaInstalled("org.ukui.style")) {
        QGSettings *gSetting = new QGSettings("org.ukui.style", QByteArray(), this);
        connect(gSetting, &QGSettings::changed, this, [=](const QString &key) {
            if ("systemFontSize" == key) {
                if (m_support && m_preview_tab_widget) {
                    if (m_info) {
                        m_preview_tab_widget->updateInfo(m_info.get());
                    }
                }
            }
        });
    }

    if (QGSettings::isSchemaInstalled("org.ukui.control-center.panel.plugins")) {
        QGSettings* settings = new QGSettings("org.ukui.control-center.panel.plugins", QByteArray(), this);
        connect(settings, &QGSettings::changed, this, [=](const QString &key) {
            if ("hoursystem" == key || "date" == key) {
                if (m_support && m_preview_tab_widget) {
                    if (m_info) {
                        FileInfoJob* infoJob = new FileInfoJob(m_info, this);
                        infoJob->setAutoDelete(true);
                        connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=] {
                            m_preview_tab_widget->updateInfo(m_info.get());
                        });
                        infoJob->queryAsync();
                    }
                }
            }
        });
    }
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
            int width = e->size().width() - 50;
            width = qMax(width, 96);
            width = qMin(width, 256);
            page->resizeIcon(QSize(width, width * 2/3));
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
    FMWindowIface *iface = dynamic_cast<FMWindowIface *>(this->topLevelWidget());
    bool locationSupport = true;
    if (iface) {
        if (iface->getCurrentUri() == "computer:///") {
            locationSupport = false;
        }
    }
    if (m_support && locationSupport) {
        auto previewPage = qobject_cast<FilePreviewPage*>(m_preview_tab_widget);
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
    m_form->setSpacing(3);

    m_display_name_label = new QLabel(this);
    QLabel *file_name_label = new QLabel(this);
    file_name_label->setAlignment(Qt::AlignTop);
    file_name_label->setText(tr("File Name:"));
    m_form->addRow(file_name_label, m_display_name_label);

    m_type_label = new QLabel(this);
    m_form->addRow(tr("File Type:"), m_type_label);

    m_time_access_label = new QLabel(this);
    m_form->addRow(tr("Time Access:"), m_time_access_label);

    m_time_modified_label = new QLabel(this);
    m_form->addRow(tr("Time Modified:"), m_time_modified_label);

    m_file_count_label = new QLabel(this);
    QLabel *children_label = new QLabel(this);
    children_label->setAlignment(Qt::AlignTop);
    children_label->setText(tr("Children Count:"));
    m_form->addRow(children_label, m_file_count_label);

    m_total_size_label = new QLabel(this);
    m_form->addRow(tr("Size:"), m_total_size_label);

    //image
    m_image_size = new QLabel(this);
    m_form->addRow(tr("Image resolution:"), m_image_size);

    m_image_format = new QLabel(this);
    m_form->addRow(tr("color model:"), m_image_format);

    m_form->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    m_form->setFormAlignment(Qt::AlignHCenter);
    m_form->setLabelAlignment(Qt::AlignLeft);

    QWidget *form = new QWidget(this);
    form->setLayout(m_form);
    m_layout->addWidget(form, 1, 0);
}

FilePreviewPage::~FilePreviewPage()
{

}

void FilePreviewPage::wrapData(QLabel *p_label, const QString &text)
{
    QString wrapText = text;
    QFontMetrics fontMetrics = p_label->fontMetrics();
    int textSize = fontMetrics.width(wrapText);

    if(textSize > LABEL_MAX_WIDTH){
        int lastIndex = 0;
        for(int i = lastIndex; i < wrapText.length(); i++) {
            if(fontMetrics.width(wrapText.mid(lastIndex, i - lastIndex)) == LABEL_MAX_WIDTH) {
                lastIndex = i;
                wrapText.insert(i, '\n');
            } else if(fontMetrics.width(wrapText.mid(lastIndex, i - lastIndex)) > LABEL_MAX_WIDTH) {
                lastIndex = i;
                wrapText.insert(i - 1, '\n');
            } else {
                continue;
            }
        }
    }

    p_label->setText(wrapText);
}

void FilePreviewPage::updateInfo(FileInfo *info)
{
    if (info->displayName().isEmpty() ||
            info->fileType().isEmpty() ||
            info->accessTime() == 0 ||
            info->modifiedTime() == 0) {
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
    //fix bug:#82320
    QString displayName = info->displayName();
    if (QRegExp("^file:///data/usershare(/{,1})$").exactMatch(info->uri())) {
        displayName = tr("usershare");
    }
    wrapData(m_display_name_label, displayName);

    wrapData(m_type_label, info->fileType());

    wrapData(m_time_access_label, info->accessDate());
    wrapData(m_time_modified_label, info->modifiedDate());

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

    if (info->isSymbolLink()&&!info->symlinkTarget().isEmpty()){
        countAsync("file:///" + info->symlinkTarget());
    } else {
        countAsync(info->uri());
    }
}

void FilePreviewPage::countAsync(const QString &uri)
{
    cancel();
    m_file_count = 0;
    m_hidden_count = 0;
    m_total_size = 0;

    QStringList uris;
    uris<<uri;
    //FIXME: replace BLOCKING api in ui thread.
    auto info = FileInfo::fromUri(uri);
    m_count_op = new FileCountOperation(uris, !info->isDir());
    connect(m_count_op, &FileOperation::operationStarted, this, &FilePreviewPage::resetCount, Qt::BlockingQueuedConnection);
    connect(m_count_op, &FileOperation::operationPreparedOne, this, &FilePreviewPage::onPreparedOne, Qt::BlockingQueuedConnection);
    connect(m_count_op, &FileCountOperation::countDone, this, &FilePreviewPage::onCountDone, Qt::BlockingQueuedConnection);
    QThreadPool::globalInstance()->start(m_count_op);
}

void FilePreviewPage::updateCount()
{
    wrapData(m_file_count_label, tr("%1 total, %2 hidden").arg(m_file_count).arg(m_hidden_count));

    auto format = g_format_size_full(m_total_size,G_FORMAT_SIZE_IEC_UNITS);
    QString fileSize(format);
    if (fileSize.contains("KiB")) {
        fileSize.replace("KiB", "KB");
    } else if (fileSize.contains("MiB")) {
        fileSize.replace("MiB", "MB");
    } else if (fileSize.contains("GiB")) {
        fileSize.replace("GiB", "GB");
    }
    m_total_size_label->setText(fileSize);
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
