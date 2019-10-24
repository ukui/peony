#include "default-preview-page.h"

#include <QLabel>

#include <QFile>
#include <QFileInfo>
#include <QUrl>

#include <QPixmap>
#include <QIcon>
#include <QMimeDatabase>

#include <QPushButton>

using namespace Peony;

DefaultPreviewPage::DefaultPreviewPage(QWidget *parent) : QStackedWidget (parent)
{
    auto label = new QLabel(tr("Select the file you want to preview..."), this);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignCenter);
    m_empty_tab_widget = label;

    label = new QLabel(this);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignCenter);
    m_preview_tab_widget = label;

    addWidget(m_preview_tab_widget);
    addWidget(m_empty_tab_widget);

    setCurrentWidget(m_empty_tab_widget);
}

DefaultPreviewPage::~DefaultPreviewPage()
{

}

void DefaultPreviewPage::prepare(const QString &uri, PreviewType type)
{
    m_current_uri = uri;
    m_current_type = type;
}

void DefaultPreviewPage::prepare(const QString &uri)
{
    setCurrentWidget(m_preview_tab_widget);
    m_current_uri = uri;
    m_current_type = Other;
}

void DefaultPreviewPage::startPreview()
{
    QLabel *label = qobject_cast<QLabel*>(m_preview_tab_widget);
    label->setText("start loading: "+m_current_uri);
}

void DefaultPreviewPage::cancel()
{
    setCurrentWidget(m_empty_tab_widget);
}

void DefaultPreviewPage::closePreviewPage()
{
    cancel();
    deleteLater();
}
