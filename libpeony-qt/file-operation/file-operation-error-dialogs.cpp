#include "file-operation-error-dialogs.h"

#include <QPainter>
#include <QPushButton>
#include <file-info.h>

Peony::FileOperationErrorDialogConflict::FileOperationErrorDialogConflict(FileOperationErrorDialogBase *parent)
    : FileOperationErrorDialogBase(parent)
{
    setFixedSize(m_fix_width, m_fix_height);
    setContentsMargins(9, 9, 9, 9);

    m_tip = new QLabel(this);
    m_tip->setTextFormat(Qt::RichText);
    m_tip->setBackgroundRole(QPalette::Link);
    m_tip->setText("<p>This location already contains a file with the same name.<br/><small>Please select the file to keep</small></p>");
    m_tip->setGeometry(m_margin_lr, m_tip_y, width() - 2 * m_margin_lr, m_tip_height);

    m_file_label1 = new FileInformationLabel(this);
    m_file_label1->setOpName(tr("Replace"));
    m_file_label1->setPixmap(":/data/file-replace.png");


    m_file_label1->setGeometry(m_margin_lr, m_file_info1_top, width() - 2 * m_margin_lr, m_file_info_height);

    m_file_label2 = new FileInformationLabel(this);
    m_file_label2->setOpName(tr("Ignore"));
    m_file_label2->setPixmap(":/data/file-ignore.png");

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
    m_rename->setText(tr("Rename"));
    m_rename->setHidden(true);
    m_rename->setGeometry(m_margin_lr, m_btn_top, m_btn_width, m_btn_heigth);

    m_cancel = new QPushButton(this);
    m_cancel->setText(tr("Cancel"));
    m_cancel->setGeometry(m_btn_cancel_margin_left, m_btn_top, m_btn_width, m_btn_heigth);

    m_ok = new QPushButton(this);
    m_ok->setText("OK");
    m_ok->setGeometry(m_btn_ok_margin_left, m_btn_top, m_btn_width, m_btn_heigth);

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
    connect(m_ck_box, &QCheckBox::clicked, [=](bool chose) {
        m_do_same_operation = chose;
    });
    connect(m_cancel, &QPushButton::clicked, [=] () {
        done(QDialog::Rejected);
    });
    connect(m_ok, &QPushButton::clicked, [=] () {
        done(QDialog::Accepted);
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
}

#if HANDLE_ERR_NEW
void Peony::FileOperationErrorDialogConflict::handle (FileOperationError& error)
{
    m_error = &error;
    m_file_label1->setFileSize(FileInfo::fromUri(error.srcUri)->fileSize());
    m_file_label1->setFileName(FileInfo::fromUri(error.srcUri)->getFileName());
    m_file_label1->setFileLocation(FileInfo::fromUri(error.srcUri)->displayName());
    m_file_label1->setFileModifyTime (FileInfo::fromUri(error.srcUri)->modifiedDate());

    m_file_label2->setFileSize(FileInfo::fromUri(error.srcUri)->fileSize());
    m_file_label2->setFileName(FileInfo::fromUri(error.srcUri)->getFileName());
    m_file_label2->setFileLocation(FileInfo::fromUri(error.srcUri)->displayName());
    m_file_label2->setFileModifyTime (FileInfo::fromUri(error.srcUri)->modifiedDate());

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
    } else if (QDialog::Rejected == ret) {
        error.respCode = Cancel;
    }
}
#else
// FIXME://DELETE
int Peony::FileOperationErrorDialogConflict::handleError(const QString &srcUri, const QString &destDirUri, const Peony::GErrorWrapperPtr &err, bool isCritical)
{

}
#endif

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

void Peony::FileInformationLabel::setPixmap(QString pixmap)
{
    m_icon.load(pixmap);
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

Peony::FileOperationErrorHandler *Peony::FileOperationErrorDialogFactory::getDialog(Peony::FileOperationError &errInfo)
{
    switch (errInfo.dlgType) {
    case ED_CONFLICT:
        return new FileOperationErrorDialogConflict();
    }
    return nullptr;
}
