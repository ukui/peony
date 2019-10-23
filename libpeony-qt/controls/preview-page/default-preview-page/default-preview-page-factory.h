#ifndef DEFAULTPREVIEWPAGEFACTORY_H
#define DEFAULTPREVIEWPAGEFACTORY_H

#include "peony-core_global.h"
#include "preview-page-plugin-iface.h"
#include <QObject>

namespace Peony {

/*!
 * \brief The DefaultPreviewPageFactory class
 * \details
 * This class is used to create the preview page of peony-qt.
 * \note
 * This is a interanl interface implement, so it is not a real plugin.
 * The interface implement is not so strict than a plugin one.
 */
class PEONYCORESHARED_EXPORT DefaultPreviewPageFactory : public QObject, public PreviewPagePluginIface
{
    Q_OBJECT
public:
    static DefaultPreviewPageFactory *getInstance();

    PluginType pluginType() override {return PluginType::PreviewPagePlugin;}
    const QString name() override {return tr("Default Preview");}
    const QString description() override {return tr("This is the Default Preview of peony-qt");}
    const QIcon icon() override {return QIcon::fromTheme("edit-find-symbolic");}
    void setEnable(bool enable) override {m_enable = enable;}
    bool isEnable() override {return m_enable;}

    PreviewPageIface *createPreviewPage() override;

private:
    explicit DefaultPreviewPageFactory(QObject *parent = nullptr);
    ~DefaultPreviewPageFactory() override;

    bool m_enable = true;
};

}

#endif // DEFAULTPREVIEWPAGEFACTORY_H
