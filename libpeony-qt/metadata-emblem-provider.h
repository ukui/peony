#ifndef METADATAEMBLEMPROVIDER_H
#define METADATAEMBLEMPROVIDER_H

#include <QObject>
#include "peony-core_global.h"
#include "emblem-provider.h"

namespace Peony {

class PEONYCORESHARED_EXPORT MetadataEmblemProvider : public EmblemProvider
{
    Q_OBJECT
public:
    static MetadataEmblemProvider *getInstance();

    const QString emblemKey() override;

    QStringList getFileEmblemIcons(const QString &uri) override;

private:
    explicit MetadataEmblemProvider(QObject *parent = nullptr);
};

}

#endif // METADATAEMBLEMPROVIDER_H
