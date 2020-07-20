#include "file-operation-progress-bar.h"

#include <gio/gio.h>
#include "complementary-style.h"

#include <QDebug>
#include <QtMath>
#include <QStyle>
#include <QPainter>
#include <QMessageBox>
#include <QCloseEvent>

FileOperationProgressBar* FileOperationProgressBar::getInstance()
{
    static FileOperationProgressBar fp;

    return &fp;
}

FileOperationProgress* FileOperationProgressBar::addFileOperation()
{
    FileOperationProgress* proc = nullptr;
    if (m_inuse_progress < m_max_progressbar) {
        proc = new FileOperationProgress();
        QListWidgetItem* li = new QListWidgetItem(m_progress_widgets);
        m_progress_widgets->addItem(li);
        m_progress_widgets->setItemWidget(li, proc);
        ++ m_inuse_progress;
        (*m_progress)[proc] = li;

        li->setSizeHint(QSize(100, 100));

        connect(proc, &FileOperationProgress::finished, this, &FileOperationProgressBar::removeFileOperation);
    }

    return proc;
}

void FileOperationProgressBar::showProcess(FileOperationProgress &proc)
{
    if (m_inuse_progress > 0) {
        proc.show();
        show();
    }
}

void FileOperationProgressBar::removeFileOperation(FileOperationProgress* fileOperation)
{
    QListWidgetItem* item = (*m_progress)[fileOperation];

    item->setHidden(true);
    m_progress_widgets->removeItemWidget(item);
    m_progress->remove(fileOperation);
    delete fileOperation;
    delete item;

    -- m_inuse_progress;

    if (m_inuse_progress <= 0) {
        m_inuse_progress = 0;
        hide();
    }

}

FileOperationProgressBar::FileOperationProgressBar(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("*{"
                  "     margin:0;"
                  "     padding:0;"
                  "}"
                  "QListWidget::Item {"
                  "     minHeight:200;"
                  "}"
                  "QListWidget::Item:hover, "
                  "QListWidget::Item:selected {"
                  "     background-color:transparent;"
                  "}"
                  );

    setStyle(Peony::ComplementaryStyle::getStyle());

    setFixedSize(m_width, m_height);
    setContentsMargins(0, 0, 0, 0);

    m_detail_label = new QLabel(nullptr);
    m_detail_widget = new QWidget(nullptr);
    m_main_layout = new QVBoxLayout(nullptr);
    m_show_detail = new DetailButton(nullptr);
    m_detail_layout = new QHBoxLayout(nullptr);
    m_progress_widgets = new QListWidget(nullptr);
    m_progress_widgets->setFrameShape(QListWidget::NoFrame);
    m_progress = new QMap<FileOperationProgress*, QListWidgetItem*>;

    m_main_layout->setContentsMargins(8, 8, 8, 8);
    m_detail_widget->setContentsMargins(0, 0, 0, 0);

    detailInfo(true);

    // add detail button
    m_spline = new QFrame(this);
    m_spline->setFrameShape(QFrame::HLine);
    m_spline->setStyleSheet("color: #C5C5C5;");
    m_spline->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_detail_widget->setMaximumHeight(20);

    m_detail_label->setMargin(0);
    m_detail_label->setBaseSize(QSize(20, 100));
    m_detail_label->setContentsMargins(0, 0, 0, 0);
    m_detail_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_detail_layout->setAlignment(Qt::AlignLeft);
    m_detail_layout->setContentsMargins(0, 0, 0, 0);
    m_detail_layout->addWidget(m_show_detail);
    m_detail_layout->addWidget(m_detail_label);
    m_detail_widget->setLayout(m_detail_layout);

    // main layout
    m_main_layout->addWidget(m_progress_widgets);
    m_main_layout->addSpacing(1);
    m_main_layout->addWidget(m_spline);
    m_main_layout->addWidget(m_detail_widget);

    setLayout(m_main_layout);

    connect (m_show_detail, &DetailButton::valueChanged, this, &FileOperationProgressBar::detailInfo);
    connect (this, &FileOperationProgressBar::cancelAll, [=] () {
        m_status = CANCEL_ALL;

        for (auto it = m_progress->constBegin(); it != m_progress->constEnd(); ++it) {
            Q_EMIT it.key()->cancelled();
        }

        m_status = READY;
    });
}

FileOperationProgressBar::~FileOperationProgressBar()
{
    for (auto it = m_progress->begin(); it != m_progress->end(); ++ it) {
        delete it.key();
        delete it.value();
    }
    delete m_progress;

    delete m_spline;
    delete m_show_detail;
    delete m_detail_label;
    delete m_detail_widget;
    delete m_detail_layout;
}

void FileOperationProgressBar::detailInfo(bool open)
{
    for (auto it = m_progress->constBegin(); it != m_progress->constEnd(); ++it) {
        if (open) {
            it.key()->m_process_file->hide();
            it.key()->m_process_left_item->hide();
        } else {
            it.key()->m_process_file->show();
            it.key()->m_process_left_item->show();
        }
    }
    if (open) {
        m_detail_label->setText(tr("display details"));
    } else {
        m_detail_label->setText(tr("display brief"));
    }
}

void FileOperationProgressBar::closeEvent(QCloseEvent *e)
{
    QMessageBox::information(nullptr, "title", "text", QMessageBox::Ok);
    Q_EMIT cancelAll();
    e->ignore();
}

DetailButton::DetailButton(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(QSize(m_size, m_size));
    setMaximumSize(QSize(m_size, m_size));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void DetailButton::paintEvent(QPaintEvent *event)
{
    QPoint p1, p2, p3;
    float r = m_size;
    float r2 = r / 20;

    QFont f;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // draw border
    QPen pen(QColor(0, 0, 0), 1);
    pen.setStyle(Qt::SolidLine);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.save();
    painter.drawEllipse(0, 0, r, r);
    painter.restore();

    // draw arch
    painter.save();
    if (!m_open) {
        p1.setX(r2 * 6);
        p1.setY(r2 * 13);

        p2.setX(r2 * 10);
        p2.setY(r2 * 8);
        painter.drawLine(p1, p2);

        p3.setX(r2 * 14);
        p3.setY(r2 * 13);
        painter.drawLine(p2, p3);
    } else {
        p1.setX(r2 * 6);
        p1.setY(r2 * 7);

        p2.setX(r2 * 10);
        p2.setY(r2 * 12);
        painter.drawLine(p1, p2);

        p3.setX(r2 * 14);
        p3.setY(r2 * 7);
        painter.drawLine(p2, p3);
    }
    painter.restore();

    Q_UNUSED(event);
}

void DetailButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_open = !m_open;

    Q_EMIT valueChanged(m_open);

    update();

    Q_UNUSED(event);
}

StartStopButton::StartStopButton(QWidget *parent) : QWidget(parent)
{
    setFixedSize(QSize(m_size, m_size));
}

void StartStopButton::paintEvent(QPaintEvent *)
{
    float margin = m_size / 10;
    QPainter painter(this);
    QPen pen(QColor(0, 0, 0), 2);

    painter.setPen(pen);

    if (m_start) {
        QLine line1(QPoint(width() / 2 - 2 * margin, margin), QPoint(width() / 2 - 2 * margin, height() - 2 * margin));
        QLine line2(QPoint(width() / 2 + 2 * margin, margin), QPoint(width() / 2 + 2 * margin, height() - 2 * margin));
        painter.save();
        painter.drawLine(line1);
        painter.restore();
        painter.save();
        painter.drawLine(line2);
        painter.restore();
    } else {
        QLine line1(QPoint(margin, margin), QPoint(margin, width() - margin));
        QLine line2(QPoint(margin, margin), QPoint(width() - 3 * margin, height() / 2));
        QLine line3(QPoint(margin, height() - margin), QPoint(width() - 3 * margin, height() / 2));
        painter.save();
        painter.drawLine(line1);
        painter.restore();
        painter.save();
        painter.drawLine(line2);
        painter.restore();
        painter.save();
        painter.drawLine(line3);
        painter.restore();
    }
}

void StartStopButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_start = !m_start;
    Q_EMIT startStopClicked(!m_start);
    update();

    Q_UNUSED(event);
}

CloseButton::CloseButton(QWidget *parent) : QWidget(parent)
{
    setFixedSize(QSize(m_size, m_size));
}

void CloseButton::paintEvent(QPaintEvent *)
{
    float margin = 2;

    QPainter painter(this);
    QPen pen(QColor(0, 0, 0), 2);
    painter.setPen(pen);
    QLine line1(QPoint(margin, margin), QPoint(this->width() - 2 * margin, this->height() - 2 * margin));
    QLine line2(QPoint(margin, this->height() - 2 * margin), QPoint(this->width() - 2 * margin, margin));
    painter.save();
    painter.drawLine(line1);
    painter.restore();
    painter.save();
    painter.drawLine(line2);
    painter.restore();
}

void CloseButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_EMIT clicked();

    Q_UNUSED(event);
}

FileOperationProgress::FileOperationProgress(QWidget *parent) : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setMinimumHeight(60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_close = new CloseButton(nullptr);
    m_process_name = new QLabel(nullptr);
    m_process_file = new QLabel(nullptr);
    m_progress = new ProgressBar(nullptr);
    m_process_percent = new QLabel(nullptr);
    m_vbox_layout = new QHBoxLayout(nullptr);
    m_main_layout = new QVBoxLayout(nullptr);
    m_process_left_item = new QLabel(nullptr);
    m_start_stop = new StartStopButton(nullptr);

    m_main_layout->setAlignment(this, Qt::AlignLeft | Qt::AlignTop);
    m_main_layout->setContentsMargins(0, 0, 5, 0);

    m_vbox_layout->addWidget(m_process_percent);
    m_vbox_layout->addWidget(m_start_stop);
    m_vbox_layout->addWidget(m_close);

    m_main_layout->addWidget(m_process_name);
    m_main_layout->addLayout(m_vbox_layout);
    m_main_layout->addWidget(m_progress);
    m_main_layout->addWidget(m_process_file);
    m_main_layout->addWidget(m_process_left_item);

    setLayout(m_main_layout);

    // close this process
    connect(m_close, &CloseButton::clicked, [=] () {
        Q_EMIT cancelled();
    });
}

FileOperationProgress::~FileOperationProgress()
{
    delete m_close;
    delete m_progress;
    delete m_start_stop;
    delete m_process_name;
    delete m_process_file;
    delete m_process_percent;
    delete m_process_left_item;

    delete m_vbox_layout;
    delete m_main_layout;
}

void FileOperationProgress::initParam()
{
    m_total_count = 0;
    m_current_count = 1;
    m_total_size = 0;
    m_current_size = 0;

    m_process_name->setText("");
    m_process_file->setText("");
    m_process_percent->setText("");
    m_process_left_item->setText("");
    m_progress->m_current_value = 0.0;
}

void FileOperationProgress::delayShow()
{

}

void FileOperationProgress::onElementFoundOne(const QString &uri, const qint64 &size)
{
    ++ m_total_count;
    m_total_size += size;
    m_src_uri = uri;
    char* format_size = g_format_size (quint64(m_total_size));
    QString text = QString("There will be %1 operations performed, total size %2.").arg(m_total_count).arg(format_size);

    m_process_name->setText(text);

    g_free(format_size);
}

void FileOperationProgress::onElementFoundAll()
{
    qDebug() << "--------------> onElementFoundAll";
}

void FileOperationProgress::switchToProgressPage()
{

}

void FileOperationProgress::onFileOperationProgressedOne(const QString &uri, const QString &destUri, const qint64 &size)
{
    ++ m_current_count;
}

void FileOperationProgress::onFileOperationProgressedAll()
{

}

void FileOperationProgress::switchToAfterProgressPage()
{

}

void FileOperationProgress::onElementClearOne(const QString &uri)
{
    qDebug() << "----------> onElementClearOne ...";
}

void FileOperationProgress::switchToRollbackPage()
{
    qDebug() << "----------> switchToRollbackPage ...";
}

void FileOperationProgress::onFileRollbacked(const QString &destUri, const QString &srcUri)
{
    qDebug() << "----------> onFileRollbacked ...";
}

void FileOperationProgress::updateProgress(const QString &srcUri, const QString &destUri, quint64 current, quint64 total)
{
    if (current >= m_total_size) {
        return;
    }

    // update value
    m_src_uri = srcUri;
    m_dest_uri = destUri;

    double currentPercent = current * 1.0 / total;
    int leftItem = m_total_count - m_current_count;
    /*
    m_process_name->setText("zheng zai yi dong wenjian ");
    m_process_file->setText("file name");
    m_process_left_item->setText("left item");
    m_process_percent->setText("process percentage");
*/

    m_process_percent->setText(QString("transferred %1 %").arg(QString::number(currentPercent * 100, 'f', 3)));
    m_progress->updateValue(currentPercent);
    if (leftItem > 1) {
        m_process_left_item->setText(QString("remain %1 items").arg(leftItem));
    } else {
        m_process_left_item->setText(QString("remain %1 item").arg(leftItem < 0 ? 0 : leftItem));
    }


    Q_UNUSED(srcUri);
    Q_UNUSED(destUri);
}

void FileOperationProgress::onStartSync()
{
    qDebug() << "----------> on start sync";
}

void FileOperationProgress::onFinished()
{
    hide();
    qDebug() << "----------> on finished";
    Q_EMIT finished(this);
}

ProgressBar::ProgressBar(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(20);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored));
    if (m_detail) {
        m_area = QRectF(0, 0, width(), 60);
    } else {
        m_area = QRectF(0, 0, width(), 15);
    }
}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

        // draw border
        painter.save();
        QPen pen(QColor(192, 192, 192), 1);
        pen.setStyle(Qt::SolidLine);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(pen);
        painter.drawRoundedRect(m_area.x(), m_area.y(), m_area.width(), m_area.height(), 1, 1);

        painter.restore();

        // draw background
        painter.save();
        QLinearGradient progressBarBgGradient (QPointF(0, 0), QPointF(0, height()));
        progressBarBgGradient.setColorAt(0.0, QColor(220, 220, 220));
        progressBarBgGradient.setColorAt(1.0, QColor(211, 211, 211));

        float margin = 1;

        painter.setPen(Qt::NoPen);
        painter.setBrush(progressBarBgGradient);
        painter.drawRoundedRect(m_area.x() + margin, m_area.y() + margin,
                                m_area.width() - 2 * margin,
                                m_area.height() - 2 * margin, 1, 1);
        painter.restore();

        // draw foreground
        painter.save();
        double value = m_current_value * (m_area.width() - m_area.x() - margin);
        QLinearGradient progressBarfgGradient (QPointF(0, 0), QPointF(0, height()));
        progressBarfgGradient.setColorAt(0.0, QColor(124, 252, 0));
        progressBarfgGradient.setColorAt(1.0, QColor(127, 255, 0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(progressBarfgGradient);
        painter.drawRoundedRect(m_area.x() + margin, m_area.y() + margin,
                                m_area.x() + qCeil(value),
                                m_area.height() - 2 * margin, 1, 1);
        painter.restore();

        // draw text
//        QRectF textArea = m_area;
//        textArea.setWidth(textArea.width() - 2 * margin);
//        QFont font = painter.font();
//        font.setPixelSize(10);
//        painter.setFont(font);
//        painter.drawText(textArea, Qt::AlignVCenter | Qt::AlignRight, tr("sudu 200MB/S"));

        Q_UNUSED(event);
}

void ProgressBar::updateValue(double value)
{
    if (value <= 1) {
        m_current_value = value;
    }
    update();
}

