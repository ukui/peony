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

#include <QTimer>
#include "file-utils.h"
#include "xatom-helper.h"

#include <QVector4D>

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

    uninhibit();

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
    li->setSizeHint(QSize(m_list_widget->width(), m_progress_item_height));
    li->setFlags(Qt::NoItemFlags);

    proc->connect(proc, &ProgressBar::finished, this, &FileOperationProgressBar::removeFileOperation);
    ++m_progress_size;

    if (nullptr == m_current_main) {
        mainProgressChange(li);
    }

    if (!isInhibit()) {
        inhibit();
    }

    showMore();

    if (m_progress_size > 0 && !isHidden()) {
        setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    }

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
    progress->deleteLater();
    delete li;

    showMore();

    if (m_progress_size <= 0) {
        m_progress_size = 0;
        m_current_main = nullptr;
        setWindowState(Qt::WindowNoState);
        hide();
        uninhibit();
    }
}

bool FileOperationProgressBar::isInhibit()
{
    return m_fds != nullptr;
}

FileOperationProgressBar::FileOperationProgressBar(QWidget *parent) : QWidget(parent)
{
    m_current_main = nullptr;
    setContentsMargins(0, 0, 0, 0);

    setAutoFillBackground (true);
    setBackgroundRole (QPalette::Base);

    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowOpacity(0.9999);

    setProperty("useCustomShadow", true);
    setProperty("customShadowDarkness", 0.5);
    setProperty("customShadowWidth", 30);
    setProperty("customShadowRadius", QVector4D(1, 1, 1, 1));
    setProperty("customShadowMargins", QVector4D(30, 30, 30, 30));

    btn = new QPushButton(nullptr);
    m_main_layout = new QVBoxLayout(this);
    m_main_layout->setContentsMargins(0, 0, 0, 0);

    m_main_progressbar = new MainProgressBar(nullptr);
    m_other_progressbar = new OtherButton(nullptr);
    m_list_widget = new QListWidget(nullptr);

    m_list_widget->setFrameShape(QListWidget::NoFrame);
    m_list_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_list_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_main_layout->addWidget(m_main_progressbar);
    m_main_layout->addWidget(m_other_progressbar);
    m_main_layout->addWidget(m_list_widget);

    m_progress_list = new QMap<ProgressBar*, QListWidgetItem*>;
    m_widget_list = new QMap<QListWidgetItem*, ProgressBar*>;

    showWidgetList(false);

    connect(m_main_progressbar, &MainProgressBar::minimized, this, [=]() {
        this->showMinimized();
    });
    connect(m_main_progressbar, &MainProgressBar::closeWindow, this, [=](){
        for (auto pg = m_widget_list->constBegin(); pg != m_widget_list->constEnd(); ++pg) {
            Q_EMIT pg.value()->cancelled();
        }
        Q_EMIT canceled();
    });

    connect(m_other_progressbar, &OtherButton::clicked, this, &FileOperationProgressBar::showWidgetList);
    connect(m_list_widget, &QListWidget::itemClicked, this, &FileOperationProgressBar::mainProgressChange);

    connect(this, &FileOperationProgressBar::pause, this, [=] () {
        m_main_progressbar->setPause();
        for (auto pb : m_progress_list->keys()) {
            pb->setPause();
        }
    });

    showMore();
}

FileOperationProgressBar::~FileOperationProgressBar()
{
    delete btn;
    if (m_dbus_connection) g_object_unref(m_dbus_connection);
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

bool FileOperationProgressBar::inhibit()
{
    g_autoptr(GError) error = NULL;
    g_autoptr(GDBusConnection) pconnection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error) {
        printf("error:%s\n", error->message);
        return false;
    }

    uninhibit();

    if (pconnection) {
        g_autoptr(GVariantType) rtype = g_variant_type_new("(h)");

        g_autoptr(GVariant) ret = g_dbus_connection_call_with_unix_fd_list_sync(pconnection, "org.freedesktop.login1", "/org/freedesktop/login1",
                                                                      "org.freedesktop.login1.Manager", "Inhibit",
                                                                      g_variant_new("(ssss)", "sleep", "peony", "file operation", "block"),
                                                                      rtype, G_DBUS_CALL_FLAGS_NONE, G_MAXINT, NULL, &m_fds, NULL, &error);
        if (error) {
            printf("cannot block s4: %s\n", error->message);
        }

        Q_UNUSED(ret);
    }

    return true;
}

void FileOperationProgressBar::uninhibit()
{
    if (m_fds) {
        g_object_unref(m_fds);
        m_fds = nullptr;
    }
}

#if 0
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
#endif

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

        m_current_main->disconnect(m_main_progressbar, &MainProgressBar::start, 0, 0);
        m_current_main->disconnect(m_main_progressbar, &MainProgressBar::pause, 0, 0);
    }

    ProgressBar* pb = (*m_widget_list)[item];
    m_current_main = pb;
    m_main_progressbar->initPrarm();
    if (m_current_main->getStatus()) {
        m_main_progressbar->cancelld();
    }
    m_main_progressbar->setFileName(pb->getFileName());
    m_main_progressbar->setProgress(pb->getProgress());
    m_main_progressbar->setFileIcon(m_current_main->getIcon());
    m_main_progressbar->connect(m_current_main, &ProgressBar::cancelled, m_main_progressbar, &MainProgressBar::cancelld);
    m_main_progressbar->connect(m_current_main, &ProgressBar::sendValue, m_main_progressbar, &MainProgressBar::updateValue);

    if (m_current_main->isPause()) {
        m_main_progressbar->setPause();
    } else {
        m_main_progressbar->setResume();
    }

    m_main_progressbar->setIsSync(pb->m_sync);
    m_current_main->connect(m_main_progressbar, &MainProgressBar::pause, this, [=] () {
        m_current_main->setPause();
        m_main_progressbar->setPause();
    });
    m_current_main->connect(m_main_progressbar, &MainProgressBar::start, this, [=] () {
        m_current_main->setResume();
        m_main_progressbar->setResume();
    });

    update();
}

void FileOperationProgressBar::showDelay(int msec)
{
    QTimer::singleShot(msec, this, [=] () {
        if (m_list_widget->count() > 0 && !m_error) {
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
    m_sync = false;
    m_show = false;
    m_pause = false;
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

void MainProgressBar::setPause()
{
    m_pause = true;
    update();
}

void MainProgressBar::setResume()
{
    m_pause = false;
    update();
}

void MainProgressBar::setIsSync(bool sync)
{
    m_sync = sync;
}

void MainProgressBar::setProgress(float val)
{
    m_current_value = val;
}

void MainProgressBar::setFileName(QString name)
{
    m_file_name = name;
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
    if ((pos.x() >= m_minilize_button_x_l)
            && (pos.x() <= m_minilize_button_x_r)
            && (pos.y() >= m_minilize_button_y_t)
            && (pos.y() <= m_minilize_button_y_b)) {
        setCursor(Qt::PointingHandCursor);
    } else if ((pos.x() >= m_close_button_x_l)
               && (pos.x() <= m_close_button_x_r)
               && (pos.y() >= m_close_button_y_t)
               && (pos.y() <= m_close_button_y_b)) {
        setCursor(Qt::PointingHandCursor);
    } else if ((pos.x() >= m_progress_pause_x)
               && (pos.x() <= m_progress_pause_x_r)
               && (pos.y() >= m_progress_pause_y)
               && (pos.y() <= m_progress_pause_y_b)) {
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
    if ((pos.x() >= m_minilize_button_x_l)
            && (pos.x() <= m_minilize_button_x_r)
            && (pos.y() >= m_minilize_button_y_t)
            && (pos.y() <= m_minilize_button_y_b)) {
        Q_EMIT minimized();
    } else if ((pos.x() >= m_close_button_x_l)
               && (pos.x() <= m_close_button_x_r)
               && (pos.y() >= m_close_button_y_t)
               && (pos.y() <= m_close_button_y_b)) {
        QMessageBox msgBox(QMessageBox::Warning, tr("cancel all file operations"),
                           tr("Are you sure want to cancel all file operations"),
                           QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Ok)->setText(tr("OK"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("Cancel"));
        if (QMessageBox::Ok == msgBox.exec()) {
            Q_EMIT closeWindow();
        }
    } else if ((pos.x() >= m_progress_pause_x)
               && (pos.x() <= m_progress_pause_x_r)
               && (pos.y() >= m_progress_pause_y)
               && (pos.y() <= m_progress_pause_y_b)) {
        if (m_pause) {
            Q_EMIT start();
        } else {
            Q_EMIT pause();
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
    painter.drawRoundedRect(0, m_foot_progress_back_y, m_fix_width, m_foot_margin, 1, 1);
    painter.setBrush(QBrush(btn.palette().color(QPalette::Highlight)));
    painter.drawRoundedRect(0, m_foot_progress_back_y, value, m_foot_margin, 1, 1);

    painter.restore();
}

void MainProgressBar::paintHeader(QPainter &painter)
{
    painter.save();

    // paint title
    QRect textArea (m_text_area_x, 0, m_title_width, m_header_height);
    QFont font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
    painter.drawText(textArea, Qt::AlignVCenter | Qt::AlignHCenter, m_title);

    // paint minilize button
    painter.drawPixmap(m_minilize_button_x_l, m_btn_margin_top, m_btn_size, m_btn_size,
                       drawSymbolicColoredPixmap(QIcon::fromTheme("window-minimize-symbolic").pixmap(m_btn_size, m_btn_size)));

    // paint close button
    painter.drawPixmap(m_close_button_x_l, m_btn_margin_top, m_btn_size, m_btn_size,
                       drawSymbolicColoredPixmap(QIcon::fromTheme("window-close-symbolic").pixmap(m_btn_size, m_btn_size)));

    painter.restore();
}

void MainProgressBar::paintContent(QPainter &painter)
{
    painter.save();

    // paint icon
    if (m_icon.isNull()) {
        painter.drawPixmap(m_icon_margin, m_icon_area_y, m_icon_size, m_icon_size,
                           QIcon::fromTheme("text").pixmap(m_icon_size, m_icon_size));
    } else {
        painter.drawPixmap(m_icon_margin, m_icon_area_y, m_icon_size, m_icon_size,
                           m_icon.pixmap(m_icon_size, m_icon_size));
    }

    // paint file name
    QFont font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
    if (m_stopping) {
        painter.drawText(m_file_name_x, m_file_name_y, m_file_name_w, m_file_name_height, Qt::AlignLeft | Qt::AlignVCenter, tr("canceling ..."));
    } else {
        if (m_sync) {
            painter.drawText(m_file_name_x, m_file_name_y, m_file_name_w, m_file_name_height, Qt::AlignLeft | Qt::AlignVCenter, tr("sync ..."));
            painter.drawPixmap(m_progress_pause_x, m_progress_pause_y, drawSymbolicColoredPixmap(QIcon::fromTheme("media-playback-pause-symbolic").pixmap(m_pause_btn_height, m_pause_btn_height)));
        } else {
            painter.drawText(m_file_name_x, m_file_name_y, m_file_name_w, m_file_name_height, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap | Qt::TextWrapAnywhere, m_file_name);
            if (m_pause) {
                painter.drawPixmap(m_progress_pause_x, m_progress_pause_y, drawSymbolicColoredPixmap(QIcon::fromTheme("media-playback-start-symbolic").pixmap(m_pause_btn_height, m_pause_btn_height)));
            } else {
                painter.drawPixmap(m_progress_pause_x, m_progress_pause_y, drawSymbolicColoredPixmap(QIcon::fromTheme("media-playback-pause-symbolic").pixmap(m_pause_btn_height, m_pause_btn_height)));
            }
        }
    }

    // paint percentage
    font.setPixelSize(12);
    painter.setFont(font);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Highlight)));
    painter.drawText(m_percent_x, m_percent_y, m_percent_height, m_percent_height, Qt::AlignRight | Qt::AlignBottom,
                     QString(" %1 %").arg(QString::number(m_current_value * 100, 'f', 1)));

    painter.restore();
}

void MainProgressBar::paintProgress(QPainter &painter)
{
    painter.save();

    double value = m_current_value * m_fix_width;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Highlight).lighter(150)));

    if (value < 1) {
        painter.drawRoundedRect(0, 0, value, m_fix_height, 1, 1);
    }

    painter.restore();
}

void MainProgressBar::cancelld()
{
    m_stopping = true;
    update();
}

void MainProgressBar::updateValue(QString& name, QIcon& icon, double value)
{
    if (value >= 0 && value < 1) {
        m_current_value = value;
    }

    m_file_name = Peony::FileUtils::urlDecode(name);
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
    m_dest_uri = tr("starting ...");
    connect(this, &ProgressBar::cancelled, this, &ProgressBar::onCancelled);
    connect(this, &ProgressBar::destroyed, this, [=] () {m_has_finished = true;});
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

float ProgressBar::getProgress()
{
    return m_current_value;
}

float ProgressBar::getTotalSize()
{
    return m_total_size;
}

QString ProgressBar::getFileName()
{
    return m_dest_uri;
}

bool ProgressBar::isPause()
{
    return m_pause;
}

void ProgressBar::setPause()
{
    m_pause = true;
    Q_EMIT pause();
    update();
}

void ProgressBar::setResume()
{
    m_pause = false;
    Q_EMIT resume();
    update();
}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.save();

    // paint icon
    if (m_icon.isNull()) {
        painter.drawPixmap(m_icon_x, m_icon_y, m_icon_size, m_icon_size,
                           QIcon::fromTheme("text").pixmap(m_icon_size, m_icon_size));
    } else {
        painter.drawPixmap(m_icon_x, m_icon_y, m_icon_size, m_icon_size,
                           m_icon.pixmap(m_icon_size, m_icon_size));
    }

    // paint text
    QPen pen;
    pen.setBrush(QBrush(btn->palette().color(QPalette::WindowText)));
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setPixelSize(12);
    painter.setFont(font);
    if (m_is_stopping) {
        painter.drawText(m_text_x, m_text_y, m_text_w, m_text_height, Qt::AlignLeft | Qt::AlignVCenter, tr("canceling ..."));
    } else if (m_sync) {
        painter.drawText(m_text_x, m_text_y, m_text_w, m_text_height, Qt::AlignLeft | Qt::AlignVCenter, tr("sync ..."));
        painter.drawPixmap(m_pause_x, m_pause_y, drawSymbolicColoredPixmap(QIcon::fromTheme("media-playback-pause-symbolic").pixmap(m_btn_size, m_btn_size)));
    } else {
        painter.drawText(m_text_x, m_text_y, m_text_w, m_text_height, Qt::AlignLeft | Qt::AlignVCenter, m_dest_uri);
    }

    // paint progress
    double value = m_current_value * m_progress_width;

    pen.setStyle(Qt::SolidLine);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Button)));
    painter.drawRect(m_progress_x, m_progress_y, m_progress_width, m_progress_height);
    painter.setBrush(QBrush(btn->palette().color(QPalette::Highlight)));
    painter.drawRoundedRect(m_progress_x, m_progress_y, value, m_progress_height, 1, 1);

    if (!m_sync) {
        if (m_pause) {
            painter.drawPixmap(m_pause_x, m_pause_y, drawSymbolicColoredPixmap(QIcon::fromTheme("media-playback-start-symbolic").pixmap(m_btn_size, m_btn_size)));
        } else {
            painter.drawPixmap(m_pause_x, m_pause_y, drawSymbolicColoredPixmap(QIcon::fromTheme("media-playback-pause-symbolic").pixmap(m_btn_size, m_btn_size)));
        }
    }


    // paint close
    painter.drawPixmap(m_close_x, m_close_y, m_btn_size, m_btn_size,
                       drawSymbolicColoredPixmap(QIcon::fromTheme("window-close-symbolic").pixmap(m_btn_size, m_btn_size)));

    painter.restore();

    Q_UNUSED(event);
}

void ProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if ((pos.x() >= m_pause_x) && (pos.x() <= m_pause_x_r)
            && (pos.y() >= m_pause_y) && (pos.y() <= m_pause_y_b)) {
        if (m_pause) {
            m_pause = false;
            Q_EMIT resume();
        } else {
            m_pause = true;
            Q_EMIT pause();
        }
    } else if ((pos.x() >= m_close_x) && (pos.x() <= m_close_x_r)
               && (pos.y() >= m_close_y) && (pos.y() <= m_close_y_b)) {
        /**
         * @note
         * fix: During the process of copying a file,
         * click the Cancel button in the small progress bar,
         * wait until the copy is finished and then click "OK" in the pop-up box,
         * which will cause the file manager to crash.
         *
         * Therefore, remove the uncheck pop-up
         *
         * @todo
         * Pause the file operation before confirming the popbox
         */
        /*QMessageBox msgBox(QMessageBox::Warning, tr("cancel file operation"),
                           tr("Are you sure want to cancel the current selected file operation"),
                           QMessageBox::Ok | QMessageBox::Cancel, this);
        msgBox.button(QMessageBox::Ok)->setText(tr("OK"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("Cancel"));
        if (QMessageBox::Ok == msgBox.exec() && */
        //
        if(!m_is_stopping) {
            if (m_has_finished) return;
            m_is_stopping = true;
            Q_EMIT cancelled();
            if (m_current_value <= 0) {
                Q_EMIT finished(this);
            }
        }
    }

    QWidget::mouseReleaseEvent(event);
    if (! m_is_stopping)
       update();
}

void ProgressBar::onCancelled()
{
    m_is_stopping = true;
    update();
}

void ProgressBar::updateValue(double value)
{
    if (value >= 0 && value < 1) {
        m_sync = false;
        m_current_value = value;
    }

    Q_EMIT sendValue(m_dest_uri, getIcon(), m_current_value);
    update();
}

void ProgressBar::onElementFoundOne(const QString &uri, const qint64 &size)
{
    ++m_total_count;
    m_total_size += size;
    m_src_uri = Peony::FileUtils::urlDecode(uri);
    //char* format_size = g_format_size (quint64(m_total_size));
    //Calculated by 1024 bytes
    char* format_size = strtok(g_format_size_full(quint64(m_total_size),G_FORMAT_SIZE_IEC_UNITS),"iB");

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
        qDebug() << "progress bar value error!";
        return;
    }

    m_src_uri = Peony::FileUtils::urlDecode(srcUri);
    if (nullptr != destUri) {
        m_dest_uri = Peony::FileUtils::urlDecode(destUri);
    }

    if (fIcon != getIcon().name()) {
        setIcon(fIcon);
    }

    double currentPercent = current * 1.0 / total;

//    qDebug() << "progress bar: " << currentPercent;

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
    m_sync = true;
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
