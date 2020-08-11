#include "file-operation-error-dialogs.h"

#include <QPainter>
#include <QPushButton>

Peony::FileOperationErrorDialogConflict::FileOperationErrorDialogConflict(Peony::FileOperationError *error, FileOperationErrorDialogBase *parent)
    : FileOperationErrorDialogBase(error, parent)
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
    m_file_label1->setGeometry(m_margin_lr, m_file_info1_top, width() - 2 * m_margin_lr, m_file_info_height);

    m_file_label2 = new FileInformationLabel(this);
    m_file_label2->setOpName(tr("Ignore"));
    m_file_label2->setGeometry(m_margin_lr, m_file_info2_top, width() - 2 * m_margin_lr, m_file_info_height);

    m_ck_box = new QCheckBox(this);
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

    connect(m_file_label1, &FileInformationLabel::active, [=](){
        m_file_label1->setActive(true);
        m_file_label2->setActive(false);
    });
    connect(m_file_label2, &FileInformationLabel::active, [=](){
        m_file_label2->setActive(true);
        m_file_label1->setActive(false);
    });
}

Peony::FileOperationErrorDialogConflict::~FileOperationErrorDialogConflict()
{

}

//bool Peony::FileOperationErrorDialogConflict::handle()
//{

//}


#if HANDLE_ERR_NEW
bool Peony::FileOperationErrorDialogConflict::handle()
{

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

    // file info
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
    painter.drawRect(pictureArea);

    QRect picNameArea (m_pic_name_x, m_pic_name_y, m_pic_name_w, m_pic_name_h);
    QFont font = painter.font();
    font.setPixelSize(10);
    painter.setFont(font);
    painter.setPen(QPalette::Base);
    painter.drawText(picNameArea, Qt::AlignVCenter | Qt::AlignHCenter, m_op_name);

    painter.restore();

    Q_UNUSED(event);
}

void Peony::FileInformationLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_EMIT active();
    Q_UNUSED(event);
}

Peony::FileOperationErrorHandler *Peony::FileOperationErrorDialogFactory::getFileOperationErrorDialog(Peony::FileOperationError& errInfo, Peony::ERROR_DIALOG errType)
{
    switch (errType) {
    case ED_CONFLICT:
        return new FileOperationErrorDialogConflict(&errInfo);
    }
}
