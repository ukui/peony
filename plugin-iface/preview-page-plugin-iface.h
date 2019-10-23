#ifndef PREVIEWPAGEPLUGINIFACE_H
#define PREVIEWPAGEPLUGINIFACE_H

#include <QPluginLoader>
#include <QtPlugin>
#include <QString>
#include "plugin-iface.h"

#define PreviewPagePluginIface_iid "org.ukui.peony-qt.plugin-iface.PreviewPagePluginInterface"

namespace Peony {

class PreviewPageIface;

class PreviewPagePluginIface : public PluginInterface
{
public:
    virtual ~PreviewPagePluginIface() {}

    virtual PreviewPageIface *createPreviewPage() = 0;
};

class PreviewPageIface
{
public:
    enum PreviewType {
        Attribute,
        Text,
        Pictrue,
        PDF,
        OfficeDoc,
        Other
    };

    virtual ~PreviewPageIface() {}

    virtual void prepare(const QString &uri, PreviewType type) = 0;
    virtual void prepare(const QString &uri) = 0;
    virtual void startPreview() = 0;
    virtual void cancel() = 0;
    virtual void closePreviewPage() = 0;
};

}

Q_DECLARE_INTERFACE(Peony::PreviewPagePluginIface, PreviewPagePluginIface_iid)

#endif // PREVIEWPAGEPLUGINIFACE_H
