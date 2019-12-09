#include <QDebug>
#include <QImage>
#include <QStandardPaths>

#include "pdf-thumbnail.h"

PdfThumbnail::PdfThumbnail(const QString &url, unsigned int pageNum)
    : pageNum(pageNum), shortUrl(url) {
  shortUrl = shortUrl.remove("file://");
  documentPrivate = Poppler::Document::load(shortUrl);
  if (!documentPrivate || documentPrivate->isLocked()) {
    qDebug() << "load pdf documnet failed";
  }
}

PdfThumbnail::~PdfThumbnail() {
  delete documentPrivate;
  delete pagePrivate;
}

QPixmap PdfThumbnail::generateThumbnail(unsigned int pageNum) {
  try {
    if (this->documentPrivate == nullptr)
      throw "pdf document not existed";
    pagePrivate = documentPrivate->page(pageNum);
    if (pagePrivate == nullptr)
      throw "load pdf page failed";
    auto image = pagePrivate->renderToImage(
        72 * 2, 72 * 2, 0, 0, pagePrivate->pageSizeF().width() * 2,
        pagePrivate->pageSizeF().height() * 2);
    if (image.isNull())
      throw "load pdf page image failed";
    return QPixmap::fromImage(image);
  } catch (char *e) {
    qDebug() << e;
    return QPixmap();
  }
}
