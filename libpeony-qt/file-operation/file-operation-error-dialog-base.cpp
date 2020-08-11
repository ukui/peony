#include "file-operation-error-dialog-base.h"

#include <QIcon>
#include <QPainter>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>

static QPixmap drawSymbolicColoredPixmap (const QPixmap& source);

Peony::FileOperationErrorDialogBase::FileOperationErrorDialogBase(Peony::FileOperationError* error, QDialog *parent) : QDialog(parent), m_error(error)
{
    setMouseTracking(true);
    setContentsMargins(0, 0, 0, 0);
    setWindowFlags(Qt::FramelessWindowHint);
}

Peony::FileOperationErrorDialogBase::~FileOperationErrorDialogBase()
{

}

//#if HANDLE_ERR_NEW
//bool Peony::FileOperationErrorDialogBase::handle(FileOperationError& errorInfo)
//{

//}

//#else

//int Peony::FileOperationErrorDialogBase::handleError(const QString &srcUri, const QString &destDirUri, const Peony::GErrorWrapperPtr &err, bool isCritical)
//{

//}

//#endif

void Peony::FileOperationErrorDialogBase::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.save();

    // paint title
    QRect textArea (m_margin_lr, m_margin_tp, width() - m_margin_lr * 2 - 2 * m_btn_size, m_header_height);
    QFont font = painter.font();
    font.setPixelSize(12);
    painter.setFont(font);
    painter.drawText(textArea, Qt::AlignVCenter | Qt::AlignHCenter, m_error->title);

    // paint minilize button
    QPen pen(QColor(192,192,192), 1);
    pen.setStyle(Qt::SolidLine);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawPixmap(width() - m_margin_lr * 2 - m_btn_size * 2, m_margin_tp, m_btn_size, m_btn_size,
                           drawSymbolicColoredPixmap(QIcon::fromTheme("window-minimize-symbolic").pixmap(m_btn_size, m_btn_size)));

    // paint close button
    pen.setStyle(Qt::SolidLine);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawPixmap(width() - m_margin_lr - m_btn_size, m_margin_tp, m_btn_size, m_btn_size,
                           drawSymbolicColoredPixmap(QIcon::fromTheme("window-close-symbolic").pixmap(m_btn_size, m_btn_size)));

    painter.restore();
}

void Peony::FileOperationErrorDialogBase::mouseMoveEvent(QMouseEvent *event)
{
    // minilize button
    QPoint pos = event->pos();
    if ((pos.x() >= width() - m_margin_lr * 2 - m_btn_size * 2)
            && (pos.x() <= width() - m_margin_lr * 2 - m_btn_size)
            && (pos.y() >= m_margin_tp)
            && (pos.y() <= m_margin_tp + m_btn_size)) {
        setCursor(Qt::PointingHandCursor);
    } else if ((pos.x() >= width() - m_margin_lr - m_btn_size)
               && (pos.x() <= width() - m_margin_lr)
               && (pos.y() >= m_margin_tp)
               && (pos.y() <= m_margin_tp + m_btn_size)) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
        QWidget::mouseMoveEvent(event);
    }
}

void Peony::FileOperationErrorDialogBase::mouseReleaseEvent(QMouseEvent *event)
{
    // minilize button
    QPoint pos = event->pos();
    if ((pos.x() >= width() - m_margin_lr * 2 - m_btn_size * 2)
            && (pos.x() <= width() - m_margin_lr * 2 - m_btn_size)
            && (pos.y() >= m_margin_tp)
            && (pos.y() <= m_margin_tp + m_btn_size)) {
        Q_EMIT showMinimized();
    } else if ((pos.x() >= width() - m_margin_lr - m_btn_size)
               && (pos.x() <= width() - m_margin_lr)
               && (pos.y() >= m_margin_tp)
               && (pos.y() <= m_margin_tp + m_btn_size)) {
        close();
    }

    QWidget::mouseReleaseEvent(event);
}

static QPixmap drawSymbolicColoredPixmap (const QPixmap& source)
{
    // 18, 32, 69
    QPushButton      m_btn;
    QColor baseColor = m_btn.palette().color(QPalette::Text).light(150);
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
