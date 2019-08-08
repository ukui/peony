#include "file-operation-progress-wizard.h"

#include <QFormLayout>
#include <QGridLayout>

#include <QLabel>
#include <QProgressBar>

#include <QPushButton>

#include <QDebug>

using namespace Peony;

FileOperationProgressWizard::FileOperationProgressWizard(QWidget *parent) : QWizard(parent)
{
    //only show cancel button at bottom-right of wizard
    QList<WizardButton> layout;
    layout<<Stretch<<CancelButton;
    setButtonLayout(layout);

    //connect(this, &QDialog::rejected, this, &FileOperationProgressWizard::cancelled);

    m_first_page = new FileOperationPreparePage(this);
    m_first_page->setTitle(tr("Preparing to move..."));
    addPage(m_first_page);
    m_second_page = new FileOperationProgressPage(this);
    m_second_page->setTitle(tr("Moving..."));
    addPage(m_second_page);
}

FileOperationProgressWizard::~FileOperationProgressWizard()
{

}

void FileOperationProgressWizard::onElementFound(const QString &uri, const qint64 &size)
{
    qDebug()<<"onElementFound"<<uri<<size;
    m_total_count++;
    m_total_size += size;

    m_first_page->m_src_line->setText(uri);
    m_first_page->m_state_line->setText(tr("%1 files, %2 bytes").arg(m_total_count).arg(m_total_size));
}

void FileOperationProgressWizard::onElementOperationFinished()
{
    switchToProgressPage();
}

void FileOperationProgressWizard::switchToPreparedPage()
{
    restart();
}

void FileOperationProgressWizard::switchToProgressPage()
{
    next();
}

void FileOperationProgressWizard::onFileOperationFinishedOne(const QString &uri, const qint64 &size)
{
    qDebug()<<"onFileOperationFinishedOne"<<uri<<size;
    m_current_count++;
    m_current_size += size;
    m_second_page->m_state_line->setText(tr("%1 done, %2 total, %3 of %4.").
                                         arg(m_current_size).arg(m_total_size)
                                         .arg(m_current_count).arg(m_total_count));

    m_second_page->m_src_line->setText(uri);
    m_second_page->m_dest_line->setText("FIXME: add the current dest uri to this signal/slot");
    double test = (m_current_size*1.0/m_total_size)*100;
    m_second_page->m_progress_bar->setValue(int(test));

    if (m_current_count == m_total_count)
        onFileOperationFinished();
}

void FileOperationProgressWizard::onFileOperationFinished()
{
    accept();
}

//FileOperationPreparePage
FileOperationPreparePage::FileOperationPreparePage(QWidget *parent) : QWizardPage (parent)
{
    m_layout = new QFormLayout(this);

    m_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    m_layout->setLabelAlignment(Qt::AlignRight);
    m_layout->setFormAlignment(Qt::AlignLeft);

    m_src_line = new QLabel("null", this);
    m_state_line = new QLabel("0 files, 0 bytes", this);
    m_layout->addRow(tr("counting:"), m_src_line);
    m_layout->addRow(tr("state:"), m_state_line);

    setLayout(m_layout);
}

FileOperationPreparePage::~FileOperationPreparePage()
{

}

//FileOperationProgressPage
FileOperationProgressPage::FileOperationProgressPage(QWidget *parent) : QWizardPage (parent)
{
    m_layout = new QGridLayout(this);

    m_state_line = new QLabel("unknow", this);

    m_progress_bar = new QProgressBar(this);

    m_layout->addWidget(m_state_line, 0, 0);
    m_layout->addWidget(m_progress_bar, 1, 0);

    QPushButton *details_button = new QPushButton(tr("&More Details"), this);
    details_button->setCheckable(true);
    m_layout->addWidget(details_button, 2, 0, Qt::AlignRight);

    QFormLayout *moreDetailsLayout = new QFormLayout;
    moreDetailsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    moreDetailsLayout->setLabelAlignment(Qt::AlignRight);
    moreDetailsLayout->setFormAlignment(Qt::AlignLeft);

    m_src_line = new QLabel("null", this);
    m_dest_line = new QLabel("null", this);
    moreDetailsLayout->addRow(tr("From:"), m_src_line);
    moreDetailsLayout->addRow(tr("To:"), m_dest_line);

    QWidget *detailsWidget = new QWidget(this);
    detailsWidget->setLayout(moreDetailsLayout);
    m_layout->addWidget(detailsWidget, 3, 0);
    detailsWidget->hide();
    connect(details_button, &QAbstractButton::toggled, detailsWidget, &QWidget::setVisible);

    setLayout(m_layout);
}

FileOperationProgressPage::~FileOperationProgressPage()
{

}
