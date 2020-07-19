#include "file-operation-progress-bar.h"

#include <QDebug>
#include <QtMath>
#include <QStyle>
#include <QPainter>

FileOperationProgressBar & FileOperationProgressBar::getInstance()
{
    static FileOperationProgressBar fp;

    return fp;
}

FileOperationProgress* FileOperationProgressBar::addFileOperation()
{
    FileOperationProgress* proc = nullptr;
    if (m_inuse_process < m_max_progressbar) {
        for (auto it = m_process->begin(); it != m_process->end(); ++ it) {
            if (FREE == it.value()) {
                it.value() = PREPARE;
                ++ m_inuse_process;
                it.key()->initParam();
                proc = it.key();
            }
        }
    }

    return proc;
}

void FileOperationProgressBar::showProcess(FileOperationProgress &proc)
{
    if (m_inuse_process > 0) {
        proc.show();
        show();
    }
}

void FileOperationProgressBar::removeFileOperation(FileOperationProgress **fileOperation)
{
    (*fileOperation)->hide();
    (*m_process)[*fileOperation] = FREE;

    -- m_inuse_process;

    if (m_inuse_process <= 0) {
        m_inuse_process = 0;
        hide();
    }
}

FileOperationProgressBar::FileOperationProgressBar(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("*{margin:0;padding:0;}");
    setFixedSize(m_width, m_height);

    setContentsMargins(0, 0, 0, 0);

    m_detail_label = new QLabel(nullptr);
    m_detail_widget = new QWidget(nullptr);
    m_scroll_widget = new QWidget(nullptr);
    m_main_layout = new QVBoxLayout(nullptr);
    m_scroll_area = new QScrollArea(nullptr);
    m_show_detail = new DetailButton(nullptr);
    m_detail_layout = new QHBoxLayout(nullptr);
    m_process_layout = new QVBoxLayout(nullptr);
    m_process_widget = new QMainWindow(nullptr);
    m_process = new QMap<FileOperationProgress*, Status> ();

    m_main_layout->setContentsMargins(0, 0, 0, 0);
    m_detail_widget->setContentsMargins(0, 0, 0, 0);
    m_process_widget->setContentsMargins(0, 0, 0, 0);

    m_scroll_widget->setLayout(m_process_layout);

    for (int i = 0; i < m_max_progressbar; ++i) {
        FileOperationProgress* fpb = new FileOperationProgress(m_width - m_margin, m_height - m_margin, nullptr);
        m_scroll_widget->layout()->addWidget(fpb);
        fpb->hide();
        m_process->insert(fpb, FREE);
    }

    detailInfo(true);

    m_scroll_widget->setFixedSize(m_width - m_margin, m_height - 60);
    m_scroll_area->setWidget(m_scroll_widget);
    m_scroll_area->setFrameShape(QFrame::NoFrame);
    m_process_widget->setCentralWidget(m_scroll_area);
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
    m_main_layout->addWidget(m_process_widget);
    m_main_layout->addSpacing(1);
    m_main_layout->addWidget(m_spline);
    m_main_layout->addWidget(m_detail_widget);

    setLayout(m_main_layout);

    connect (m_show_detail, &DetailButton::valueChanged, this, &FileOperationProgressBar::detailInfo);
}

FileOperationProgressBar::~FileOperationProgressBar()
{
    for (auto it = m_process->begin(); it != m_process->end(); ++ it) {
        delete it.key();
    }
    delete m_process;

    delete m_spline;
    delete m_show_detail;
    delete m_detail_label;
    delete m_detail_widget;
    delete m_detail_layout;

    delete m_scroll_area;
    delete m_scroll_widget;
    delete m_process_layout;
    delete m_process_widget;
}

void FileOperationProgressBar::detailInfo(bool open)
{
    for (auto it = m_process->constBegin(); it != m_process->constEnd(); ++it) {
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
    qDebug() << "click";

    Q_UNUSED(event);
}

FileOperationProgress::FileOperationProgress(float width, float height, QWidget *parent) : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
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

    m_process_name->setText("zheng zai yi dong wenjian ");
    m_process_file->setText("file name");
    m_process_left_item->setText("left item");
    m_process_percent->setText("process percentage");

    m_vbox_layout->addWidget(m_process_percent);
    m_vbox_layout->addWidget(m_start_stop);
    m_vbox_layout->addWidget(m_close);

    m_main_layout->addWidget(m_process_name);
    m_main_layout->addLayout(m_vbox_layout);
    m_main_layout->addWidget(m_progress);
    m_main_layout->addWidget(m_process_file);
    m_main_layout->addWidget(m_process_left_item);

    setLayout(m_main_layout);
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
    m_process_name->setText("");
    m_process_file->setText("");
    m_process_percent->setText("");
    m_process_left_item->setText("");
    m_progress->m_current_value = 0.0;
}

ProgressBar::ProgressBar(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(10);
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
        QPen pen(QColor(192,192,192), 1);
        pen.setStyle(Qt::SolidLine);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(pen);
        painter.drawRoundedRect(m_area.x(), m_area.y(), m_area.width(), m_area.height(), 1, 1);

        painter.restore();

        // draw background
        painter.save();
        QLinearGradient progressBarBgGradient (QPointF(0, 0), QPointF(0, height()));
        progressBarBgGradient.setColorAt(0.0, QColor(220,220,220));
        progressBarBgGradient.setColorAt(1.0, QColor(211,211,211));

        float margin = 1;

        painter.setPen(Qt::NoPen);
        painter.setBrush(progressBarBgGradient);
        painter.drawRoundedRect(m_area.x() + margin, m_area.y() + margin,
                                m_area.width() - 2 * margin,
                                m_area.height() - 2 * margin, 1, 1);
        painter.restore();

        // draw foreground
        painter.save();
        double value = m_current_value * (m_area.width() - m_area.x() - 2 * margin);
        QLinearGradient progressBarfgGradient (QPointF(0, 0), QPointF(0, height()));
        progressBarfgGradient.setColorAt(0.0, QColor(124,252,0));
        progressBarfgGradient.setColorAt(1.0, QColor(127,255,0));
        painter.setPen(Qt::NoPen);
        painter.setBrush(progressBarfgGradient);
        painter.drawRoundedRect(m_area.x() + margin, m_area.y() + margin,
                                m_area.x() + qCeil(value),
                                m_area.height() - 2 * margin, 1, 1);
        painter.restore();

        // draw text
        QRectF textArea = m_area;
        textArea.setWidth(textArea.width() - 2 * margin);
        QFont font = painter.font();
        font.setPixelSize(10);
        painter.setFont(font);
        painter.drawText(textArea, Qt::AlignVCenter | Qt::AlignRight, tr("sudu 200MB/S"));

        Q_UNUSED(event);
}

void ProgressBar::updateValue(double value)
{
    if (value <= 1) {
        m_current_value = value;
    }
}

