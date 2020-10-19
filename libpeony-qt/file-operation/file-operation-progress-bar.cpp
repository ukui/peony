/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: Jing Ding <dingjing@kylinos.cn>
 *
 */

#include "file-operation-progress-bar.h"

#include <gio/gio.h>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QMessageBox>

#include <QUrl>
#include <QTimer>

QPushButton* btn;

static QPixmap drawSymbolicColoredPixmap (const QPixmap&);

FileOperationProgressBar *FileOperationProgressBar::instance = nullptr;

FileOperationProgressBar *FileOperationProgressBar::getInstance()
{
    if (nullptr == FileOperationProgressBar::instance) {
        instance = new FileOperationProgressBar;
    }

    return instance;
}

void FileOperationProgressBar::removeAllProgressbar()
{
    if (nullptr != m_main_progressbar && nullptr != m_current_main) {
        m_main_progressbar->disconnect(m_current_main, &ProgressBar::sendValue, 0, 0);
        m_current_main = nullptr;
    }

    for (auto pg = m_widget_list->constBegin(); pg != m_widget_list->constEnd(); ++pg) {
        if (pg.value()->m_current_size > 0) continue;
        if (nullptr != pg.value()) pg.value()->deleteLater();
        if (nullptr != pg.key()) delete pg.key();
    }

    m_widget_list->clear();
    m_list_widget->clear();
    m_progress_list->clear();
    m_progress_size = 0;
    hide();
}

ProgressBar *FileOperationProgressBar::addFileOperation()
{
    ProgressBar* proc = new ProgressBar;
    QListWidgetItem* li = new QListWidgetItem(m_list_widget);
    m_list_widget->addItem(li);
    m_list_widget->setItemWidget(li, proc);
    (*m_progress_list)[proc] = li;
    (*m_widget_list)[li] = proc;
    li->setSizeHint(QSize(m_main_progressbar->width(), m_progress_item_height));

    li->setFlags(Qt::NoItemFlags);

    connect(proc, &ProgressBar::finished, this, &FileOperationProgressBar::removeFileOperation);
    ++m_progress_size;

    if (nullptr == m_current_main) {
        mainProgressChange(li);
    }

    showMore();

    return proc;
}

void FileOperationProgressBar::showProgress(ProgressBar &progress)
{
    if (m_progress_size > 0) {
        progress.show();
        show();
    }
}

void FileOperationProgressBar::removeFileOperation(ProgressBar *progress)
{
    progress->hide();
    QListWidgetItem* li = (*m_progress_list)[progress];

    m_list_widget->removeItemWidget(li);
    m_progress_list->remove(progress);
    m_widget_list->remove(li);

    --m_progress_size;

    // check main progress
    if (m_current_main == progress) {
        // check other progress
        if (m_progress_size > 0) {
            QListWidgetItem* pg = m_progress_list->first();
            m_current_main = (*m_widget_list)[pg];
            mainProgressChange(pg);
        }
    }

    // free progress
    delete progress;
    delete li;

    if (m_progress_size <= 0) {
        m_progress_size = 0;
        m_current_main = nullptr;
        hide();
    }

    showMore();
}

FileOperationProgressBar::FileOperationProgressBar(QWidget *parent) : QWidget(parent)
{
    m_current_main = nullptr;
    setWindowFlags(Qt::FramelessWindowHint);
    setContentsMargins(0, 0, 0, 0);

    btn = new QPushButton(nullptr);
    m_main_layout = new QVBoxLayout(this);
    m_main_layout->setContentsMargins(0, 0, 0, 0);

    m_main_progressbar = new MainProgressBar(nullptr);
    m_other_progressbar = new OtherButton(nullptr);
    m_list_widget = new QListWidget(nullptr);

    m_list_widget->setFrameShape(QListWidget::NoFrame);

    m_main_layout->addWidget(m_main_progressbar);
    m_main_layout->addWidget(m_other_progressbar);
    m_main_layout->addWidget(m_list_widget);

    m_progress_list = new QMap<ProgressBar*, QListWidgetItem*>;
    m_widget_list = new QMap<QListWidgetItem*, ProgressBar*>;

    showWidgetList(false);

    connect(m_main_progressbar, &MainProgressBar::minimized, [=]() {
        this->showMinimized();
    });
    connect(m_main_progressbar, &MainProgressBar::closeWindow, [=](){
        for (auto pg = m_widget_list->constBegin(); pg != m_widget_list->constEnd(); ++pg) {
            Q_EMIT pg.value()->cancelled();
        }
        Q_EMIT canceled();
    });

    connect(m_other_progressbar, &OtherButton::clicked, this, &FileOperationProgressBar::showWidgetList);
    connect(m_list_widget, &QListWidget::itemClicked, this, &FileOperationProgressBar::mainProgressChange);

    showMore();
}

FileOperationProgressBar::~FileOperationProgressBar()
{
    delete btn;
}

void FileOperationProgressBar::showMore()
{
    if (m_progress_size > 1) {
        m_other_progressbar->show();
        if (m_progress_size > 1 && m_progress_size <= m_show_items) {
            m_list_widget->setFixedHeight(m_progress_size * m_progress_item_height);
        } else if (m_progress_size > m_show_items) {
            m_list_widget->setFixedHeight(m_show_items * m_progress_item_height);
        }

        if (m_show_more) {
            m_list_widget->show();
            m_other_progressbar->show();
            setFixedSize(m_main_progressbar->width(), m_main_progressbar->height() + m_other_progressbar->height() + m_list_widget->height());
        } else {
            m_list_widget->hide();
            setFixedSize(m_main_progressbar->width(), m_main_progressbar->height() + m_other_progressbar->height());
        }
    } else {
        m_list_widget->hide();
        m_other_progressbar->hide();
        setFixedSize(m_main_progressbar->width(), m_main_progressbar->height());
    }
    update();
}

void FileOperationProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_is_press) {
        move(event->globalPos() - m_position);
        event->accept();
    }
}

void FileOperationProgressBar::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button()) {
        m_is_press = true;
        m_position = event->globalPos() - frameGeometry().topLeft();
    }
}

void FileOperationProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_is_press = false;
    }
}

void FileOperationProgressBar::showWidgetList(bool show)
{
    m_show_more = show;
    if (show) {
        m_list_widget->show();
    } else {
        m_list_widget->hide();
    }
    showMore();
}

void FileOperationProgressBar::mainProgressChange(QListWidgetItem *item)
{
    if (nullptr != m_main_progressbar && nullptr != m_current_main) {
        m_main_progressbar->disconnect(m_current_main, &ProgressBar::sendValue, 0, 0);
    }

    ProgressBar* pb = (*m_widget_list)[item];
    m_current_main = pb;
    m_main_progressbar->initPrarm();
    if (m_current_main->getStatus()) {
        m_main_progressbar->cancelld();
    }
    m_main_progressbar->setFileIcon(m_current_main->getIcon());
    m_main_progressbar->connect(m_current_main, &ProgressBar::cancelled, m_main_progressbar, &MainProgressBar::cancelld);
    m_main_progressbar->connect(m_current_main, &ProgressBar::sendValue, m_main_progressbar, &MainProgressBar::updateValue);
    update();
}

void FileOperationProgressBar::showDelay(int msec)
{
    QTimer::singleShot(msec, this, [=](){
        if (m_list_widget->count() > 0) {
            show();
        }
    });
}

MainProgressBar::MainProgressBar(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    m_title = tr("File operation");

    setFixedSize(m_fix_width, m_fix_height);
}

void MainProgressBar::initPrarm()
{
    m_stopping = false;
    m_current_value = 0.0;
    m_file_name = tr("starting ...");
}

void MainProgressBar::setFileIcon(QIcon& icon)
{
    m_icon = icon;
}

void MainProgressBar::setTitle(QString title)
{
    m_title = title;
}

void MainProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter (this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    paintProgress (painter);
    paintHeader (painter);
    paintFoot (painter);
    paintContent (painter);

    Q_UNUSED(event);
}

void MainProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    // minilize button
    QPoint pos = event->pos();
    if ((pos.x() >= m_fix_width - m_btn_margin * 2 - m_btn_size * 2)
            && (pos.x() <= (m_fix_width - m_btn_margin * 2 - m_btn_size))
            && (pos.y() >= m_btn_margin_top)
            && (pos.y() <= m_btn_margin_top + m_btn_size)) {
        setCursor(Qt::PointingHandCursor);
    } else if ((pos.x() >= m_fix_width - m_btn_margin - m_btn_size)
               && (pos.x() <= (m_fix_width - m_btn_margin))
               && (pos.y() >= m_btn_margin_top)
               && (pos.y() <= m_btn_margin_top + m_btn_size)) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
        QWidget::mouseMoveEvent(event);
    }
}

void MainProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    // minilize button
    QPoint pos = event->pos();
    if ((pos.x() >= m_fix_width - m_btn_margin * 2 - m_btn_size * 2)
            && (pos.x() <= (m_fix_width - m_btn_margin * 2 - m_btn_size))
            && (pos.y() >= m_btn_margin_top)
            && (pos.y() <= m_btn_margin_top + m_btn_size)) {
        Q_EMIT minimized();
    } else if ((pos.x() >= m_fix_width - m_btn_margin - m_btn_size)
               && (pos.x() <= (m_fix_width - m_btn_margin))
               && (pos.y() >= m_btn_margin_top)
               && (pos.y() <= m_btn_margin_top + m_btn_size)) {
        QMessageBox msgBox(QMessageBox::Warning, tr("cancel all file operations"),
                           tr("Are you sure want to cancel all file operations"),
                           QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Ok)->setText(tr("OK"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("Cancel"));
        if (QMessageBox::Ok == msgBox.exec()) {
            Q_EMIT closeWindow();
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void MainProgressBar::paintFoot(QPainter &painter)
{
    painter.save();

    double value = m_current_value * m_fix_width;
    QPushButton btn;

//    QLinearGradient progressBarBgGradient (QPointF(0, 0), QPointF(0, height()));
//    progressBarBgGradient.setColorAt(0.0, QColor(75,0,130));
//    progressBarBgGradient.setColorAt(1.0, QColor(75,0,130));
//    painter.setBrush(progressBarBgGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(btn.palette().color(QPalette::Button)));
    painter.drawRoundedRect(0, m_fix_height - m_foot_margin, m_fix_width, m_foot_margin, 1, 1);
    painter.setBrush(QBrush(btn.palette().color(QPalette::Highlight)));
    painter.drawRoundedRect(0, m_fix_height - m_foot_margin, value, m_foot_margin, 1, 1);

    painter.restore();
}

void MainProgressBar::paintHeader(QPainter &painter)
{
    painter.save();

    // paint title
    QRect textArea (width() / 2 - m_title_width / 2, 0, m_title_width, m_header_height);
    QFont font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
    painter.drawText(textArea, Qt::AlignVCenter | Qt::AlignHCenter, m_title);

    // paint minilize button
    painter.drawPixmap(m_fix_width - m_btn_margin * 2 - m_btn_size * 2, m_btn_margin_top, m_btn_size, m_btn_size,
                       drawSymbolicColoredPixmap(QIcon::fromTheme("window-minimize-symbolic").pixmap(m_btn_size, m_btn_size)));

    // paint close button
    painter.drawPixmap(m_fix_width - m_btn_margin - m_btn_size, m_btn_margin_top, m_btn_size, m_btn_size,
                       drawSymbolicColoredPixmap(QIcon::fromTheme("window-close-symbolic").pixmap(m_btn_size, m_btn_size)));

    painter.restore();
}

void MainProgressBar::paintContent(QPainter &painter)
{
    double x = 0;
    double y = 0;
    double w = 0;

    painter.save();

    // paint icon
    y = m_fix_height / 2 - m_icon_size / 2;
    if (m_icon.isNull()) {
        painter.drawPixmap(m_icon_margin, y, m_icon_size, m_icon_size,
                           QIcon::fromTheme("text").pixmap(m_icon_size, m_icon_size));
    } else {
        painter.drawPixmap(m_icon_margin, y, m_icon_size, m_icon_size,
                           m_icon.pixmap(m_icon_size, m_icon_size));
    }

    // paint file name
    x = m_icon_margin + m_file_name_margin + m_icon_size;
    y = m_fix_height / 2 - m_file_name_height / 2;
    w = m_fix_width - m_icon_size - m_icon_margin - m_file_name_margin * 3;
    QFont font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
    if (m_stopping) {
        painter.drawText(x, y, w, m_file_name_height, Qt::AlignLeft | Qt::AlignVCenter, tr("canceling ..."));
    } else {
        painter.drawText(x, y, w, m_file_name_height, Qt::AlignLeft | Qt::AlignVCenter, m_file_name);
    }

    // paint percentage
    x = m_fix_width - m_percent_margin - m_percent_height;
    y = m_fix_height - m_foot_margin - m_percent_height - m_percent_margin / 5;
    font.setPixelSize(12);
    painter.setFont(font);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Highlight)));
    painter.drawText(x, y, m_percent_height, m_percent_height, Qt::AlignRight | Qt::AlignBottom,
                     QString(" %1 %").arg(QString::number(m_current_value * 100, 'f', 1)));

    painter.restore();
}

void MainProgressBar::paintProgress(QPainter &painter)
{
    painter.save();

    double value = m_current_value * m_fix_width;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Highlight).lighter(150)));
    painter.drawRoundedRect(0, 0, value, m_fix_height, 1, 1);

//    QLinearGradient progressBarBgGradient (QPointF(0, 0), QPointF(0, height()));
//    progressBarBgGradient.setColorAt(0.0, QColor(175, 238, 238));
//    progressBarBgGradient.setColorAt(1.0, QColor(175, 238, 238));
//    painter.setBrush(progressBarBgGradient);


    painter.restore();
}

void MainProgressBar::cancelld()
{
    m_stopping = true;
    update();
}

void MainProgressBar::updateValue(QString& name, QIcon& icon, double value)
{
    if (value >= 0 && value <= 1) {
        m_current_value = value;
    }

    m_file_name = name;
    m_icon = icon;

    update();
}

OtherButton::OtherButton(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);
    setMinimumWidth(180);
    setFixedHeight(m_button_heigth);
    setContentsMargins(0, 0, 0, 0);
}

void OtherButton::paintEvent(QPaintEvent *event)
{
    double x = 0;
    double y = 0;

    QPainter painter(this);
    painter.save();

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    // paint icon
    x = width() / 2 - m_icon_size - m_icon_margin -20;
    y = (height() - m_icon_size) / 2;
    QRect iconArea (x, y, m_icon_size, m_icon_size);
    if (m_show) {
        painter.drawPixmap(iconArea, drawSymbolicColoredPixmap(QIcon::fromTheme("kylin-fold").pixmap(m_icon_size, m_icon_size)));
    } else {
        painter.drawPixmap(iconArea, drawSymbolicColoredPixmap(QIcon::fromTheme("kylin-open").pixmap(m_icon_size, m_icon_size)));
    }

    // paint text
    x = x + m_icon_size + 10;
    QRect textArea (x, 0, m_text_length, m_button_heigth);
    QFont font = painter.font();
    font.setPixelSize(10);
    painter.setFont(font);
    pen.setBrush(QBrush(btn->palette().color(QPalette::WindowText)));
    painter.setPen(pen);
    painter.drawText(textArea, Qt::AlignLeft | Qt::AlignVCenter, m_text);

    painter.restore();

    Q_UNUSED(event);
}

void OtherButton::mousePressEvent(QMouseEvent *event)
{
    if (!m_is_press) {
        m_is_press = true;
    }
    Q_UNUSED(event);
}

void OtherButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_is_press) {
        m_show = !m_show;
        Q_EMIT clicked(m_show);
    }

    m_is_press = false;

    Q_UNUSED(event);
}

ProgressBar::ProgressBar(QWidget *parent) : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setFixedHeight(m_fix_height);
    setMouseTracking(true);
    m_is_stopping = false;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, &ProgressBar::cancelled, this, &ProgressBar::onCancelled);
}

void ProgressBar::setIcon(const QString& icon)
{
    if (nullptr != icon && "" != icon) {
        m_icon = QIcon::fromTheme(icon);
    } else {
        m_icon = QIcon::fromTheme("text");
    }
}

QIcon& ProgressBar::getIcon()
{
    return m_icon;
}

bool ProgressBar::getStatus()
{
    return m_is_stopping;
}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    double x = 0;
    double y = 0;
    double w = 0;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.save();

    // paint icon
    x = m_margin_lr;
    y = (height() - m_margin_ud * 2 - m_icon_size) / 2 + m_margin_ud;
    if (m_icon.isNull()) {
        painter.drawPixmap(x, y, m_icon_size, m_icon_size,
                           QIcon::fromTheme("text").pixmap(m_icon_size, m_icon_size));
    } else {
        painter.drawPixmap(x, y, m_icon_size, m_icon_size,
                           m_icon.pixmap(m_icon_size, m_icon_size));
    }

    // paint text
    x = m_margin_lr * 2 + m_icon_size;
    y = (height() - m_margin_ud * 2 - m_text_height) / 2 + m_margin_ud;
    w = width() - m_margin_lr * 5 - m_icon_size - m_btn_size - m_progress_width - m_percent_width;
    QPen pen;
    pen.setBrush(QBrush(btn->palette().color(QPalette::WindowText)));
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setPixelSize(12);
    painter.setFont(font);
    if (m_is_stopping) {
        painter.drawText(x, y, w, m_text_height, Qt::AlignLeft | Qt::AlignVCenter, tr("canceling ..."));
    } else {
        painter.drawText(x, y, w, m_text_height, Qt::AlignLeft | Qt::AlignVCenter, m_dest_uri);
    }

    // paint progress
    x = m_margin_lr * 3 + m_icon_size + w;
    y = (height() - m_margin_ud * 2 - m_progress_height) / 2 + m_margin_ud;
    double value = m_current_value * m_progress_width;

    pen.setStyle(Qt::SolidLine);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);

//    QLinearGradient progressBarBgGradient (QPointF(0, 0), QPointF(0, height()));
//    progressBarBgGradient.setColorAt(0.0, QColor(175,238,238));
//    progressBarBgGradient.setColorAt(1.0, QColor(175,238,238));
//    painter.setPen(Qt::NoPen);
//    painter.setBrush(progressBarBgGradient);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Button)));
    painter.drawRect(x, y, m_progress_width, m_progress_height);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Highlight)));
    painter.drawRoundedRect(x, y, value, m_progress_height, 1, 1);

    // paint close
    x =  m_margin_lr * 5 + m_icon_size + w + m_progress_width;
    y = (height() - m_margin_ud * 2 - m_btn_size) / 2 + m_margin_ud;
    painter.drawPixmap(x, y, m_btn_size, m_btn_size,
                       drawSymbolicColoredPixmap(QIcon::fromTheme("window-close-symbolic").pixmap(m_btn_size, m_btn_size)));

    // paint stopping

    painter.restore();

    Q_UNUSED(event);
}

void ProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    double w = width() - m_margin_lr * 5 - m_icon_size - m_btn_size - m_progress_width - m_percent_width;
    double x =  m_margin_lr * 5 + m_icon_size + w + m_progress_width;
    double y = (height() - m_margin_ud * 2 - m_btn_size) / 2 + m_margin_ud;

    QPoint pos = event->pos();
    if ((pos.x() >= x) && (pos.x() <= (x + m_btn_size))
            && (pos.y() >= y) && (pos.y() <= y + m_btn_size)) {
        QMessageBox msgBox(QMessageBox::Warning, tr("cancel file operation"),
                           tr("Are you sure want to cancel the current selected file operation"),
                           QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Ok)->setText(tr("OK"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("Cancel"));
        if (QMessageBox::Ok == msgBox.exec() && ! m_is_stopping) {
            m_is_stopping = true;
            Q_EMIT cancelled();
            if (m_current_value <= 0) {
                Q_EMIT finished(this);
            }
        }
    }

    QWidget::mouseReleaseEvent(event);
    update();
}

void ProgressBar::onCancelled()
{
    m_is_stopping = true;
    update();
}

void ProgressBar::updateValue(double value)
{
    if (value >= 0 && value <= 1) {
        m_current_value = value;
    }

    Q_EMIT sendValue(m_dest_uri, getIcon(), m_current_value);
    update();
}

void ProgressBar::onElementFoundOne(const QString &uri, const qint64 &size)
{
    ++m_total_count;
    m_total_size += size;
    QUrl url = uri;
    m_src_uri = url.toDisplayString();
    char* format_size = g_format_size (quint64(m_total_size));

    g_free(format_size);
}

void ProgressBar::onElementFoundAll()
{

}

void ProgressBar::onFileOperationProgressedOne(const QString &uri, const QString &destUri, const qint64 &size)
{
    ++m_current_count;

    Q_UNUSED(uri);
    Q_UNUSED(size);
    Q_UNUSED(destUri);
}

void ProgressBar::updateProgress(const QString &srcUri, const QString &destUri, const QString& fIcon, const quint64& current, const quint64& total)
{
    if (current >= m_total_size) {
        return;
    }

    QUrl srcUrl = srcUri;
    m_src_uri = srcUrl.toDisplayString();
    if (nullptr != destUri) {
        QUrl destUrl = destUri;
        m_dest_uri = destUrl.toDisplayString();
    }

    if (fIcon != getIcon().name()) {
        setIcon(fIcon);
    }

    double currentPercent = current * 1.0 / total;
    updateValue(currentPercent);

    Q_UNUSED(srcUri);
    Q_UNUSED(destUri);
}

void ProgressBar::onFileOperationProgressedAll()
{

}

void ProgressBar::onElementClearOne(const QString &uri)
{

    Q_UNUSED(uri);
}

void ProgressBar::switchToRollbackPage()
{

}

void ProgressBar::onStartSync()
{

}

void ProgressBar::onFinished()
{
    hide();
    Q_EMIT finished(this);
}

void ProgressBar::onFileRollbacked(const QString &destUri, const QString &srcUri)
{
    Q_UNUSED(srcUri);
    Q_UNUSED(destUri);
}

QPixmap drawSymbolicColoredPixmap (const QPixmap& source)
{
    // 18, 32, 69
    QColor baseColor = btn->palette().color(QPalette::Text).light(150);
    QImage img = source.toImage();

    for (int x = 0; x < img.width(); ++x) {
        for (int y = 0; y < img.height(); ++y) {
            auto color = img.pixelColor(x, y);
            color.setRed(baseColor.red());
            color.setGreen(baseColor.green());
            color.setBlue(baseColor.blue());
            img.setPixelColor(x, y, color);
        }
    }

    return QPixmap::fromImage(img);
}
