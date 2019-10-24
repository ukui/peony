#include "content-preview-page.h"

#include <QVBoxLayout>
#include <QLabel>

ContentPreviewPage::ContentPreviewPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    m_label = new QLabel(tr("This page is a part of peony-qt extension"), this);
    layout->addWidget(m_label);
}

ContentPreviewPage::~ContentPreviewPage()
{

}

void ContentPreviewPage::startPreview()
{
    m_label->setText(m_current_uri);
}

void ContentPreviewPage::cancel()
{
    m_label->setText(tr("This page is a part of peony-qt extension"));
}
