#ifndef LIBPEONYPREVIEW_PDFTHUMBNAIL_H
#define LIBPEONYPREVIEW_PDFTHUMBNAIL_H

#include <QPixmap>
#include <QString>
#include <poppler-qt5.h>

class PdfThumbnail {
public:
  unsigned int pageNum;

  explicit PdfThumbnail(const QString &url, unsigned int pageNum = 0);
  ~PdfThumbnail();
  QPixmap generateThumbnail(unsigned int pageNum = 0);

private:
  QString shortUrl;
  Poppler::Document *documentPrivate = nullptr;
  Poppler::Page *pagePrivate = nullptr;
};

#endif // LIBPEONYPREVIEW_PDFTHUMBNAIL_H
