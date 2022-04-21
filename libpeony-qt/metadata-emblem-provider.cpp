#include "metadata-emblem-provider.h"
#include "file-meta-info.h"

using namespace Peony;

static MetadataEmblemProvider *global_instance = nullptr;

MetadataEmblemProvider *MetadataEmblemProvider::getInstance()
{
    if (!global_instance)
        global_instance = new MetadataEmblemProvider;
    return global_instance;
}

const QString MetadataEmblemProvider::emblemKey()
{
    return "metainfo-emblem";
}

QStringList MetadataEmblemProvider::getFileEmblemIcons(const QString &uri)
{
    auto metaInfo = FileMetaInfo::fromUri(uri);
    auto data = metaInfo->getMetaInfoStringListV1("emblems");
    if (!data.isEmpty()) {
        QStringList tmp = data;
    }
    return metaInfo->getMetaInfoStringListV1("emblems");
}

MetadataEmblemProvider::MetadataEmblemProvider(QObject *parent) : EmblemProvider(parent)
{

}
