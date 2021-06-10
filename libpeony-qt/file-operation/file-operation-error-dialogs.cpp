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

#include <QUrl>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <file-info.h>
#include <QHBoxLayout>
#include <file-info-job.h>
#include <file-utils.h>
#include <QStyleOptionViewItem>

static QPixmap drawSymbolicColoredPixmap (const QPixmap& source);

Peony::FileOperationErrorDialogConflict::FileOperationErrorDialogConflict(FileOperationErrorDialogBase *parent)
    : FileOperationErrorDialogBase(parent)
{
    setFixedSize(m_fix_width, m_fix_height);
    setContentsMargins(9, 9, 9, 9);

    // file icon
    m_file_icon = new QLabel(this);
    m_file_icon->setGeometry(m_file_x, m_file_y, m_file_size, m_file_size);
    m_file_icon->setPixmap(QIcon::fromTheme(m_file_icon_name.isEmpty()?"text-x-plain":m_file_icon_name).pixmap(QSize(m_file_size, m_file_size)));


    m_tip = new QLabel(this);
    m_tip->setTextFormat(Qt::RichText);
    m_tip->setBackgroundRole(QPalette::Link);
    m_tip->setText(QString("<p>%1 %3</p>")
                   .arg(tr("This location already contains the file,"))
                   .arg(tr("Do you want to override it?")));
    m_tip->setWordWrap(true);
    m_tip->setGeometry(m_tip_x, m_tip_y, m_tip_width, m_tip_height);

    // replace
    m_rp_btn = new QPushButton(this);
    m_rp_btn->setText(tr("Replace"));
    m_rp_btn->setBackgroundRole(QPalette::Button);
    m_rp_btn->setGeometry(m_rp_btn_x, m_rp_btn_y, m_rp_btn_width, m_rp_btn_height);

    // ignore
    m_ig_btn = new QPushButton(this);
    m_ig_btn->setText(tr("Ignore"));
    m_ig_btn->setGeometry(m_ig_btn_x, m_ig_btn_y, m_ig_btn_width, m_ig_btn_height);

    // backup
    m_bk_btn = new QPushButton(this);
    m_bk_btn->setText(tr("Backup"));
    m_bk_btn->setGeometry(m_bk_btn_x, m_bk_btn_y, m_bk_btn_width, m_bk_btn_height);

    // Then do the same thing
    m_sm_ck = new QCheckBox(this);
    m_sm_ck->setText(tr("Do the same"));
    m_sm_ck->setGeometry(m_sm_btn_x, m_sm_btn_y, m_sm_btn_width / 2, m_sm_btn_height);

    connect(m_sm_ck, &QCheckBox::stateChanged, this, [=](int chose) {
        switch (chose) {
        case Qt::Checked:
            m_do_same = true;
            break;
        case Qt::Unchecked:
        default:
            m_do_same = false;
        }
    });

    connect(m_rp_btn, &QPushButton::pressed, this, [=] () {
        m_ignore = false;
        m_backup = false;
        m_replace = true;
        done(QDialog::Accepted);
    });

    connect(m_ig_btn, &QPushButton::pressed, this, [=] () {
        m_ignore = true;
        m_backup = false;
        m_replace = false;
        done(QDialog::Accepted);
    });

    connect(m_bk_btn, &QPushButton::pressed, this, [=] () {
        m_ignore = false;
        m_backup = true;
        m_replace = false;
        done(QDialog::Accepted);
    });
}

Peony::FileOperationErrorDialogConflict::~FileOperationErrorDialogConflict()
{

}

void Peony::FileOperationErrorDialogConflict::setTipFilename(QString name)
{
    if (!name.isEmpty()) {
        QStyleOptionViewItem opt;
        m_file_name = QUrl(name).toDisplayString();
        m_tip->setText(QString(tr("<p>This location already contains the file '%1', Do you want to override it?</p>"))
                       .arg(opt.fontMetrics.elidedText(m_file_name, Qt::ElideMiddle, 480).toHtmlEscaped()));
    }
}

void Peony::FileOperationErrorDialogConflict::setTipFileicon(QString icon)
{
    if (!icon.isEmpty()) {
        m_file_icon_name = icon;
        m_file_icon->setPixmap(QIcon::fromTheme(m_file_icon_name.isEmpty()?"text-x-plain":m_file_icon_name).pixmap(QSize(m_file_size, m_file_size)));
    }
}

void Peony::FileOperationErrorDialogConflict::handle (FileOperationError& error)
{
    m_error = &error;

    if (FileOpRename == m_error->op) {
        FileInfoJob file(error.destDirUri, nullptr);
        file.querySync();
        setTipFileicon(file.getInfo()->iconName());
        setTipFilename(file.getInfo()->displayName());
    } else {
        QString fileName = error.srcUri.split("/").back();
        QString url = error.destDirUri.contains(fileName) ? error.destDirUri : error.destDirUri + "/" + fileName;
        FileInfoJob file(url, nullptr);
        file.querySync();
        setTipFileicon(file.getInfo()->iconName());
        setTipFilename(file.getInfo()->displayName());
    }

    error.respCode = Retry;
    int ret = exec();
    if (QDialog::Accepted == ret) {
        if (m_do_same) {
            if (m_replace) {
                error.respCode = OverWriteAll;
            } else if (m_backup) {
                error.respCode = BackupAll;
            } else if (m_ignore) {
                error.respCode = IgnoreAll;
            } else {
                error.respCode = Cancel;
            }
        } else {
            if (m_replace) {
                error.respCode = OverWriteOne;
            } else if (m_backup) {
                error.respCode = BackupOne;
            } else if (m_ignore) {
                error.respCode = IgnoreOne;
            } else {
                error.respCode = Cancel;
            }
        }
    } else {
        error.respCode = Cancel;
    }
}


Peony::FileOperationErrorHandler *Peony::FileOperationErrorDialogFactory::getDialog(Peony::FileOperationError &errInfo)
{
    FileOperationErrorDialogBase* dlg = nullptr;

    switch (errInfo.dlgType) {
    case ED_CONFLICT:
        dlg = new FileOperationErrorDialogConflict();
        dlg->setHeaderIcon("dialog-warning");
        break;
    case ED_WARNING:
        dlg = new FileOperationErrorDialogWarning();
        dlg->setHeaderIcon("dialog-error");
        break;
    case ED_NOT_SUPPORTED: {
        dlg = new FileOperationErrorDialogNotSupported();
        dlg->setHeaderIcon("dialog-information");
        break;
    }
    }

    return dlg;
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

    m_cancel = new QPushButton(this);
    m_cancel->setText(tr("Cancel"));
    m_cancel->setGeometry(m_cancel_x, m_cancel_y, m_cancel_w, m_cancel_h);

    connect(m_ok, &QPushButton::pressed, this, [=](){
        done (QDialog::Accepted);
    });

    connect(m_cancel, &QPushButton::pressed, this, [=] () {
        done (QDialog::Rejected);
    });
}

Peony::FileOperationErrorDialogWarning::~FileOperationErrorDialogWarning()
{

}

void Peony::FileOperationErrorDialogWarning::handle(Peony::FileOperationError &error)
{
    m_error = &error;

    QStyleOptionViewItem opt;
    if (nullptr != m_error->errorStr) {
        QString htmlString = QString("<p>%1</p>")
                .arg(opt.fontMetrics.elidedText(m_error->errorStr.toHtmlEscaped(), Qt::ElideMiddle, 480).toHtmlEscaped());
        m_text->setText(htmlString);
    } else {
        QString htmlString = QString("<p>%1</p>")
                .arg(opt.fontMetrics.elidedText(tr("Make sure the disk is not full or write protected and that the file is not protected"), Qt::ElideMiddle, 480).toHtmlEscaped());
        m_text->setText(htmlString);
    }

    m_text->adjustSize();
    m_text->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    int ret = exec();

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

    // Delete file to the Recycle Bin error, prompt whether to force deletion
    if (QDialog::Accepted == ret && m_error->op == FileOpTrash && m_error->errorCode == G_IO_ERROR_FILENAME_TOO_LONG) {
        error.respCode = Force;
    }

    if (QDialog::Rejected == ret) {
        error.respCode = Cancel;
    }
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

Peony::FileOperationErrorDialogNotSupported::FileOperationErrorDialogNotSupported(Peony::FileOperationErrorDialogBase *parent) : FileOperationErrorDialogBase(parent)
{
    setFixedSize(m_fix_width, m_fix_height);
    setContentsMargins(9, 9, 9, 9);

    m_icon = new QLabel(this);
    m_icon->setGeometry(m_margin_lr, m_pic_top, m_pic_size, m_pic_size);
    m_icon->setPixmap(QIcon::fromTheme("dialog-infomation").pixmap(m_pic_size, m_pic_size));

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
    m_ok->setText(tr("Yes"));
    m_ok->setGeometry(m_ok_x, m_ok_y, m_ok_w, m_ok_h);

    m_cancel = new QPushButton(this);
    m_cancel->setText(tr("Cancel"));
    m_cancel->setGeometry(m_cancel_x, m_cancel_y, m_cancel_w, m_cancel_h);

    m_sm_ck = new QCheckBox(this);
    m_sm_ck->setText(tr("Do the same"));
    m_sm_ck->setGeometry(m_sm_btn_x, m_sm_btn_y, m_sm_btn_width, m_sm_btn_height);

    connect(m_ok, &QPushButton::pressed, this, [=](){
        done (QDialog::Accepted);
    });

    connect(m_cancel, &QPushButton::pressed, this, [=] () {
        done (QDialog::Rejected);
    });
}

Peony::FileOperationErrorDialogNotSupported::~FileOperationErrorDialogNotSupported()
{

}

void Peony::FileOperationErrorDialogNotSupported::handle(Peony::FileOperationError &error)
{
    m_error = &error;

    QStyleOptionViewItem opt;
    if (nullptr != m_error->errorStr) {
        QString htmlString = QString("<p>%1</p>")
                .arg(opt.fontMetrics.elidedText(m_error->errorStr.toHtmlEscaped(), Qt::ElideMiddle, 480).toHtmlEscaped());
        m_text->setText(htmlString);
    } else {
        QString htmlString = QString("<p>%1</p>")
                .arg(opt.fontMetrics.elidedText(tr("Make sure the disk is not full or write protected and that the file is not protected"), Qt::ElideMiddle, 480).toHtmlEscaped());
        m_text->setText(htmlString);
    }

    m_text->adjustSize();
    m_text->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    int ret = exec();

    switch (m_error->errorCode) {
        case G_IO_ERROR_NOT_SUPPORTED: {
            if (!m_sm_ck->isChecked())
                error.respCode = OverWriteOne;
            else
                error.respCode = OverWriteAll;
            break;
        }
        case G_IO_ERROR_BUSY:
        case G_IO_ERROR_PENDING:
        case G_IO_ERROR_NO_SPACE:
        case G_IO_ERROR_CANCELLED:
        case G_IO_ERROR_INVALID_DATA:
        case G_IO_ERROR_PERMISSION_DENIED:
        case G_IO_ERROR_CANT_CREATE_BACKUP:
        case G_IO_ERROR_TOO_MANY_OPEN_FILES:
            error.respCode = Cancel;
            break;
        default:
            error.respCode = IgnoreOne;
            break;
    }

    if (QDialog::Rejected == ret) {
        error.respCode = Cancel;
    }
}
