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

DefaultPreviewPage::DefaultPreviewPage(const QString &uri, QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    QUrl url = uri;
    if (!url.isLocalFile()) {
        QLabel *label = new QLabel(tr("Can not preivew this file."));
        layout->addWidget(label);
        return;
    }

    QFile file(url.path());
    QFileInfo info(file);
    QIcon icon;
    QPixmap pixmap(url.path());
    if (pixmap.isNull()) {
        QMimeDatabase db;
        icon = QIcon::fromTheme(db.mimeTypeForUrl(url).iconName());
    } else {
        icon = pixmap;
    }

    QPushButton *button = new QPushButton(icon, url.fileName(), this);
    layout->addWidget(button);

    QMimeDatabase db;
    QLabel *typeLabel = new QLabel(db.mimeTypeForUrl(url).name(), this);
    layout->addWidget(typeLabel);
    QLabel *sizeLabel = new QLabel(QString::number(info.size()), this);
    layout->addWidget(sizeLabel);
}
