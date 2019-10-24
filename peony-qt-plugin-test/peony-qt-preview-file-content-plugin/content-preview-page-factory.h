#ifndef CONTENTPREVIEWPAGEFACTORY_H
#define CONTENTPREVIEWPAGEFACTORY_H

#include "peony-qt-preview-file-content-plugin_global.h"
#include "preview-page-plugin-iface.h"

/*!
 * \brief The ContentPreviewPageFactory class
 *
 * \details This example is showing you how to
 * implement a preview page plugin for peony-qt.
 * We should start at implement the PreviewPagePluginIface
 * with your own factory.
 *
 * You have to declare your factory
 * as PreviewPagePluginIface derived class
 * and override all virtual method both
 * Peony::PreviewPagePluginIface and Peony::PluginIface provided.
 *
 * \note
 * You don't need declare the PreviewPageIface as a plugin interface,
 * But you should aslo implement your own PreviewPage
 * by deriving Peony::PreviewPageIface for the return value
 * as this class's createPreviewPage() method.
 */
class PEONYQTPREVIEWFILECONTENTPLUGINSHARED_EXPORT ContentPreviewPageFactory : public QObject, public Peony::PreviewPagePluginIface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PreviewPagePluginIface_iid)
    Q_INTERFACES(Peony::PreviewPagePluginIface)
public:
    explicit ContentPreviewPageFactory(QObject *parent = nullptr);
    ~ContentPreviewPageFactory() override;

    PluginInterface::PluginType pluginType() override {return PluginInterface::PreviewPagePlugin;}
    const QString name() override {return "Preview Page Plugin";}
    const QString description() override {return "This plugin is a factory plugin providing the content preview page for FMWindow";}
    const QIcon icon() override {return QIcon::fromTheme("edit-find-symbolic");}
    void setEnable(bool enable) override {m_enable = enable;}
    bool isEnable() override {return m_enable;}

    Peony::PreviewPageIface *createPreviewPage() override;

private:
    bool m_enable = true;

};

#endif // CONTENTPREVIEWPAGEFACTORY_H
