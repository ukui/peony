#ifndef OFFICETHUMBNAIL_H
#define OFFICETHUMBNAIL_H

#include "file-info.h"
#include <QHash>
#include <QIcon>
#include <QMutex>
#include <QUrl>

using namespace Peony;

class OfficeThumbnail{
public:
    explicit OfficeThumbnail(const QString &uri);
    ~OfficeThumbnail();
    QIcon generateThumbnail();

private:
    /*
    * 提供office文件首页转换为图片的存储路径
    */
    void thumbnaileCachDir();

    QUrl m_url;
    /*
    * 获取文件的修改时间，如果被修改，将重新生成缩略图，
    * 主要是为了处理修改文件首页的情况
    */
    quint64 m_modifyTime = 0;
};

#endif // OFFICETHUMBNAIL_H
