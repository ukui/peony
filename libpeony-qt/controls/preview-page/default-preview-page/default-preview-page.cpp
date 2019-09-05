#include "default-preview-page.h"

#include <QLabel>

#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QGridLayout>
#include <QPixmap>
#include <QIcon>
#include <QMimeDatabase>

#include <QPushButton>

using namespace Peony;

DefaultPreviewPage::DefaultPreviewPage(QWidget *parent) : QWidget(parent)
{
    m_layout = new QGridLayout(this);
    setLayout(m_layout);
}

DefaultPreviewPage::~DefaultPreviewPage()
{

}

void DefaultPreviewPage::prepare(const QString &uri, PreviewType type)
{
    m_current_uri = uri;
    m_current_type = type;
}

void DefaultPreviewPage::startPreview()
{
    QLabel *l = new QLabel(tr("start preview..."), this);
    m_layout->addWidget(l);
    //TODO:
    //cancellable async preview
}

void DefaultPreviewPage::cancel()
{
    QLabel *l = new QLabel(tr("cancel..."), this);
    m_layout->addWidget(l);
}

void DefaultPreviewPage::closePreviewPage()
{
    cancel();
    deleteLater();
}
