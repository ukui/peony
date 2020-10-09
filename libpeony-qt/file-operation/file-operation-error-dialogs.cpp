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
#include "file-operation-error-dialogs.h"

#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <file-info.h>
#include <file-info-job.h>
#include <QHBoxLayout>

static QPixmap drawSymbolicColoredPixmap (const QPixmap& source);

Peony::FileOperationErrorDialogConflict::FileOperationErrorDialogConflict(FileOperationErrorDialogBase *parent)
    : FileOperationErrorDialogBase(parent)
{
    setFixedSize(m_fix_width, m_fix_height);
    setContentsMargins(9, 9, 9, 9);

    m_tip = new QLabel(this);
    m_tip->setTextFormat(Qt::RichText);
    m_tip->setBackgroundRole(QPalette::Link);
    m_tip->setText(QString("<p>%1</p><br/><small>%2</small>")
                   .arg(tr("This location already contains a file with the same name."))
                   .arg(tr("Please select the file to keep")));
    m_tip->setGeometry(m_margin_lr, m_tip_y, width() - 2 * m_margin_lr, m_tip_height);

    m_file_label1 = new FileInformationLabel(this);
    m_file_label1->setOpName(tr("Replace"));
    m_file_label1->setPixmap(QIcon::fromTheme("ukui-replace-doc").pixmap(m_file_label1->getIconSize(), m_file_label1->getIconSize()));


    m_file_label1->setGeometry(m_margin_lr, m_file_info1_top, width() - 2 * m_margin_lr, m_file_info_height);

    m_file_label2 = new FileInformationLabel(this);
    m_file_label2->setOpName(tr("Ignore"));
    m_file_label2->setPixmap(QIcon::fromTheme("ukui-ellipsis-doc").pixmap(m_file_label1->getIconSize(), m_file_label1->getIconSize()));

    m_file_label2->setGeometry(m_margin_lr, m_file_info2_top, width() - 2 * m_margin_lr, m_file_info_height);

    m_is_replace = false;
    m_file_label2->setActive(true);
    m_file_label1->setActive(false);

    m_ck_box = new QCheckBox(this);
    m_ck_box->setChecked(m_do_same_operation);
    m_ck_box->setGeometry(m_margin_lr, m_ck_btn_top, m_ck_btn_heigth, m_ck_btn_heigth);

    m_ck_label = new QLabel(this);
    m_ck_label->setText(tr("Then do the same thing in a similar situation"));
    m_ck_label->setGeometry(m_margin_lr + m_ck_btn_heigth + 6, m_ck_btn_top, width() - m_margin_lr * 2 - m_ck_btn_heigth, m_ck_btn_heigth);

    m_rename = new QPushButton(this);
    m_rename->setText(tr("Backup"));
    m_rename->setGeometry(m_margin_lr, m_btn_top, m_btn_width, m_btn_heigth);

    m_cancel = new QPushButton(this);
    m_cancel->setText(tr("Cancel"));
    m_cancel->setGeometry(m_btn_cancel_margin_left, m_btn_top, m_btn_width, m_btn_heigth);

    m_ok = new QPushButton(this);
    m_ok->setText(tr("OK"));
    m_ok->setGeometry(m_btn_ok_margin_left, m_btn_top, m_btn_width, m_btn_heigth);

    // rename dialog
    m_rename_dialog = new FileRenameDialog();

    connect(m_file_label1, &FileInformationLabel::active, [=]() {
        m_is_replace = true;
        m_file_label1->setActive(true);
        m_file_label2->setActive(false);
    });
    connect(m_file_label2, &FileInformationLabel::active, [=]() {
        m_is_replace = false;
        m_file_label2->setActive(true);
        m_file_label1->setActive(false);
    });
    connect(m_file_label1, &FileInformationLabel::choosed, [=]() {
        done (QDialog::Accepted);
    });
    connect(m_file_label2, &FileInformationLabel::choosed, [=]() {
        done (QDialog::Accepted);
    });
    connect(m_ck_box, &QCheckBox::clicked, [=](bool chose) {
        m_do_same_operation = chose;
    });
    connect(m_cancel, &QPushButton::clicked, [=] (bool) {
        done(QDialog::Rejected);
    });
    connect(m_ok, &QPushButton::clicked, [=] (bool) {
        done(QDialog::Accepted);
    });

    connect(m_rename, &QPushButton::clicked, [=] (bool) {
        if (QDialog::Accepted == m_rename_dialog->exec()) {
            if (m_is_backup) {
                if (!m_error->respValue.contains("name")) {
                    m_is_backup_all = true;
                }
            }
            done (QDialog::Accepted);
        } else {
            m_is_backup = false;
            m_is_backup_all = false;
        }
    });

    connect(m_rename_dialog, &FileRenameDialog::customRename, [=](FileRenameDialog::RenameType nameType, QString name) {
        m_is_backup = true;
        switch (nameType) {
        case FileRenameDialog::USER_INPUT:
            m_error->respValue["name"] = QVariant(name);
            break;
        case FileRenameDialog::AUTO_INSCREASE:
            if (m_error->respValue.contains("name")) {
                m_error->respValue.remove("name");
            }
            break;
        }
    });
}

Peony::FileOperationErrorDialogConflict::~FileOperationErrorDialogConflict()
{
    delete m_ok;
    delete m_tip;
    delete m_ck_box;
    delete m_rename;
    delete m_cancel;
    delete m_file_label1;
    delete m_file_label2;
    delete m_rename_dialog;
}

void Peony::FileOperationErrorDialogConflict::handle (FileOperationError& error)
{
    m_error = &error;
    FileInfoJob file(error.srcUri, nullptr);
    file.querySync();

    m_file_label1->setFileSize(file.getInfo()->fileSize());
    m_file_label1->setFileName(file.getInfo()->displayName());
    m_file_label1->setFileLocation(file.getInfo()->filePath());
    m_file_label1->setFileModifyTime (file.getInfo()->modifiedDate());

    m_file_label2->setFileSize(file.getInfo()->fileSize());
    m_file_label2->setFileName(file.getInfo()->displayName());
    m_file_label2->setFileLocation(file.getInfo()->filePath());
    m_file_label2->setFileModifyTime (file.getInfo()->modifiedDate());

    error.respCode = Retry;
    int ret = exec();
    if (QDialog::Accepted == ret) {
        if (m_do_same_operation) {
            if (m_is_replace) {
                error.respCode = OverWriteAll;
            } else {
                error.respCode = IgnoreAll;
            }
        } else {
            if (m_is_replace) {
                error.respCode = OverWriteOne;
            } else {
                error.respCode = IgnoreOne;
            }
        }

        if (m_is_backup) {
            if (m_is_backup_all) {
                error.respCode = BackupAll;
            } else {
                error.respCode = BackupOne;
            }
        }
    } else if (QDialog::Rejected == ret) {
        error.respCode = Cancel;
    }
}

Peony::FileInformationLabel::FileInformationLabel(QWidget *parent) : QFrame(parent)
{
    setObjectName("peony-frame");
    setFixedSize(m_fix_width, m_fix_heigth);
    setContentsMargins(0, 0, 0, 0);
    setFrameShadow(QFrame::Plain);
    setFrameShape(QFrame::StyledPanel);
    setStyleSheet("QWidget#peony-frame:hover{border:1px solid gray;}");

    m_file_information = new QLabel (this);
    m_file_information->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_file_information->setTextFormat(Qt::RichText);
    m_file_information->setGeometry(m_file_name_x, m_file_name_y, m_file_name_w, m_file_name_h);
}

Peony::FileInformationLabel::~FileInformationLabel()
{
    delete m_file_information;
}

float Peony::FileInformationLabel::getIconSize()
{
    return m_pic_size;
}

void Peony::FileInformationLabel::setActive(bool active)
{
    m_active = active;
    update();
}

void Peony::FileInformationLabel::setOpName(QString name)
{
    m_op_name = name;
    update();
}

void Peony::FileInformationLabel::setPixmap(QPixmap pixmap)
{
    m_icon = pixmap;
}

void Peony::FileInformationLabel::setFileSize(QString fileSize)
{
    m_file_size = fileSize;
}

void Peony::FileInformationLabel::setFileName(QString fileName)
{
    m_file_name = fileName;
    update();
}

void Peony::FileInformationLabel::setFileLocation(QString path)
{
    m_file_location = path;
    update();
}

void Peony::FileInformationLabel::setFileModifyTime(QString modify)
{
    m_modify_time = modify;
    update();
}


void Peony::FileInformationLabel::paintEvent(QPaintEvent *event)
{
    QPushButton btn;
    QPainter painter (this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // paint frame
    painter.setPen(Qt::NoPen);
    if (m_active) {
        painter.setBrush(QBrush(btn.palette().color(QPalette::Highlight).light(150)));
    } else {
        painter.setBrush(Qt::NoBrush);
    }
    painter.drawRect(0, 0, width(), height());
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::NoBrush);

    // paint picture
    painter.save();
    QRect pictureArea (m_pic_x, m_pic_y, m_pic_size, m_pic_size);
    painter.drawPixmap(pictureArea, m_icon);

    QRect picNameArea (m_pic_name_x, m_pic_name_y, m_pic_name_w, m_pic_name_h);
    QFont font = painter.font();
    font.setPixelSize(10);
    painter.setFont(font);
    painter.setPen(QPen(btn.palette().color(QPalette::WindowText)));
    painter.drawText(picNameArea, Qt::AlignVCenter | Qt::AlignHCenter, m_op_name);

    painter.restore();

    // draw info
    QString htmlString = QString("<style>"
                                 "  p{font-size:10px;line-height:60%;}"
                                 "  .bold{text-align: left;font-size:13px;font-wight:500;}"
                                 "</style>"
                                 "<p class='bold'>%1</p>"
                                 "<p>%2 %3</p>"
                                 "<p>%4 %5</p>"
                                 "<p>%6 %7</p>")
            .arg(m_file_name).arg(tr("File location:")).arg(m_file_location)
            .arg(tr("File size:")).arg(m_file_size).arg(tr("Modify time:")).arg(m_modify_time);
    m_file_information->setText(htmlString);

    Q_UNUSED(event);
}

void Peony::FileInformationLabel::mousePressEvent(QMouseEvent *event)
{
    Q_EMIT active();
    Q_UNUSED(event);
}

void Peony::FileInformationLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_EMIT active();
    Q_EMIT choosed();

    Q_UNUSED(event);
}

Peony::FileOperationErrorHandler *Peony::FileOperationErrorDialogFactory::getDialog(Peony::FileOperationError &errInfo)
{
    switch (errInfo.dlgType) {
    case ED_CONFLICT:
        return new FileOperationErrorDialogConflict();
    case ED_WARNING:
        return new FileOperationErrorDialogWarning();
    }
    return nullptr;
}

Peony::FileOperationErrorDialogWarning::FileOperationErrorDialogWarning(Peony::FileOperationErrorDialogBase *parent)
    : FileOperationErrorDialogBase(parent)
{
    setFixedSize(m_fix_width, m_fix_height);
    setContentsMargins(9, 9, 9, 9);

    m_icon = new QLabel(this);
    m_icon->setGeometry(m_margin_lr, m_pic_top, m_pic_size, m_pic_size);
    m_icon->setPixmap(QIcon::fromTheme("dialog-error").pixmap(m_pic_size, m_pic_size));

    m_text_scroll = new QScrollArea(this);
    m_text_scroll->setFrameShape(QFrame::NoFrame);
    m_text_scroll->setGeometry(m_margin + m_margin_lr + m_pic_size, m_text_y,
                               width() - m_margin - m_margin_lr * 2 - m_pic_size, m_text_heigth);

    m_text = new QLabel(m_text_scroll);

    m_text->setText("");
    m_text->setWordWrap(true);
    m_text->setMinimumWidth(width() - m_margin - m_margin_lr * 2 - m_pic_size);

    m_text_scroll->setWidget(m_text);
    m_text_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_text_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_ok = new QPushButton(this);
    m_ok->setText(tr("OK"));
    m_ok->setGeometry(m_ok_x, m_ok_y, m_ok_w, m_ok_h);

    connect(m_ok, &QPushButton::pressed, [=](){
        done(QDialog::Accepted);
    });
}

Peony::FileOperationErrorDialogWarning::~FileOperationErrorDialogWarning()
{
    delete m_ok;
    delete m_icon;
    delete m_text;
}

void Peony::FileOperationErrorDialogWarning::handle(Peony::FileOperationError &error)
{
    m_error = &error;

    if (nullptr != m_error->errorStr) {
        QString htmlString = QString("<style>"
                                     "  p{font-size:14px;line-height:100%;}"
                                     "  .bold{text-align: left;font-size:13px;font-wight:500;}"
                                     "</style>"
                                     "<p class='bold'>%1</p>"
                                     "<p>%2</p>")
                .arg(m_error->errorStr)
                .arg(tr("Make sure the disk is not full or write protected and that the file is not protected"));
        m_text->setText(htmlString);
    } else {
        QString htmlString = QString("<style>"
                                     "  p{font-size:14px;line-height:100%;}"
                                     "  .bold{text-align: left;font-size:13px;font-wight:500;}"
                                     "</style>"
                                     "<p>%1</p>")
                .arg(tr("Make sure the disk is not full or write protected and that the file is not protected"));
        m_text->setText(htmlString);
    }

    m_text->adjustSize();
    m_text->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    exec();

    switch (m_error->errorCode) {
        case G_IO_ERROR_BUSY:
        case G_IO_ERROR_PENDING:
        case G_IO_ERROR_NO_SPACE:
        case G_IO_ERROR_CANCELLED:
        case G_IO_ERROR_INVALID_DATA:
        case G_IO_ERROR_NOT_SUPPORTED:
        case G_IO_ERROR_PERMISSION_DENIED:
        case G_IO_ERROR_CANT_CREATE_BACKUP:
        case G_IO_ERROR_TOO_MANY_OPEN_FILES:
            error.respCode = Cancel;
            break;
        default:
            error.respCode = IgnoreOne;
            break;
    }
}

Peony::FileRenameDialog::FileRenameDialog(QWidget *parent) : QDialog(parent)
{
    setMouseTracking(true);
    setContentsMargins(0, 0, 0, 0);
    setWindowFlags(Qt::FramelessWindowHint);

    m_tip = new QLabel(this);
    m_name = new QLineEdit(this);
    m_ok = new QPushButton(this);
    m_name_label = new QLabel(this);
    m_cancel = new QPushButton(this);
    m_if_custom = new QCheckBox(this);

    setFixedSize(m_fix_width, m_fix_heigth);

    m_if_custom->setChecked(true);
    m_if_custom->setGeometry(m_margin + 16, 65, 24, 24);

    m_tip->setText(tr("Names automatically add serial Numbers (e.g., 1,2,3...)"));
    m_tip->setGeometry(m_margin * 2 + 16 + 24, 65, width() - m_margin * 3 - 16 - 24, 26);

    m_cancel->setText(tr("Cancel"));
    m_cancel->setGeometry(274, 132, 120, 36);

    m_name_label->setHidden(true);
    m_name_label->setGeometry(26, 113, 98, 20);
    m_name_label->setText(tr("New file name"));

    m_name->setHidden(true);
    m_name->setGeometry(130, 107, 400, 32);
    m_name->setPlaceholderText(tr("Please enter the file name"));

    m_ok->setText(tr("OK"));
    m_ok->setGeometry(410, 132, 120, 36);

    connect(m_if_custom, &QCheckBox::toggled, [=](bool status) {
        if (!status) {
            m_name->setHidden(false);
            m_name_label->setHidden(false);
            setFixedSize(m_fix_width, m_fix_heigth_2);
            m_ok->setGeometry(410, 132 + 40, 120, 36);
            m_cancel->setGeometry(274, 132 + 40, 120, 36);
        } else {
            m_name->setHidden(true);
            m_name_label->setHidden(true);
            setFixedSize(m_fix_width, m_fix_heigth);
            m_ok->setGeometry(410, 132, 120, 36);
            m_cancel->setGeometry(274, 132, 120, 36);
        }
        update();
    });

    connect(m_cancel, &QPushButton::clicked, [=](bool) {
        done(QDialog::Rejected);
    });
    connect(m_ok, &QPushButton::clicked, [=](bool) {
        if (true == m_if_custom->isChecked() || "" == m_name->text()) {
            Q_EMIT customRename (AUTO_INSCREASE, "");
        } else {
            Q_EMIT customRename (USER_INPUT, m_name->text());
        }
        done(QDialog::Accepted);
    });
}

Peony::FileRenameDialog::~FileRenameDialog()
{
    delete m_ok;
    delete m_tip;
    delete m_name;
    delete m_cancel;
    delete m_if_custom;
}

void Peony::FileRenameDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.save();

    QPushButton btn;
    // paint title
//    QRect textArea (m_margin_lr, 0, width() - m_margin_lr * 2 - 2 * m_btn_size, m_header_height);
//    QFont font = painter.font();
//    font.setPixelSize(12);
//    painter.setFont(font);
//    painter.setBrush(QBrush(btn.palette().color(QPalette::Highlight).lighter(150)));
//    if (nullptr != m_error && nullptr != m_error->title) {
//        painter.drawText(textArea, Qt::AlignVCenter | Qt::AlignHCenter, m_error->title);
//    }

    // paint close button
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(pen);
    painter.drawPixmap(width() - m_margin - m_header_btn_size, m_margin, m_header_btn_size, m_header_btn_size,
                           drawSymbolicColoredPixmap(QIcon::fromTheme("window-close-symbolic").pixmap(m_header_btn_size, m_header_btn_size)));

    painter.restore();
    Q_UNUSED(event);
}

void Peony::FileRenameDialog::mouseMoveEvent(QMouseEvent *event)
{
    // minilize button
    QPoint pos = event->pos();
    if ((pos.x() >= width() - m_margin - m_header_btn_size)
               && (pos.x() <= width() - m_margin)
               && (pos.y() >= m_margin)
               && (pos.y() <= m_margin + m_header_btn_size)) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
        QWidget::mouseMoveEvent(event);
    }
}

void Peony::FileRenameDialog::mousePressEvent(QMouseEvent *event)
{
    // minilize button
    QPoint pos = event->pos();
    if ((pos.x() >= width() - m_margin - m_header_btn_size)
               && (pos.x() <= width() - m_margin)
               && (pos.y() >= m_margin)
               && (pos.y() <= m_margin + m_header_btn_size)) {
        Q_EMIT customRename (AUTO_INSCREASE, "");
        done(QDialog::Rejected);
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
