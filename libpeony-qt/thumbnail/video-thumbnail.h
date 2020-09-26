#ifndef VIDEOTHUMBNAIL_H
#define VIDEOTHUMBNAIL_H

#include "file-info.h"
#include <QHash>
#include <QIcon>
#include <QMutex>
#include <QUrl>

using namespace Peony;

class VideoThumbnail{
public:
    explicit VideoThumbnail(const QString &uri);
    ~VideoThumbnail();
    QIcon generateThumbnail();

private:
    QMap<QString, QString> videoInfo();
    QUrl m_url;
};

#endif // VIDEOTHUMBNAIL_H
