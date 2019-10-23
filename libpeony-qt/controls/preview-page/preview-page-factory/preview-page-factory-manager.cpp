#include "preview-page-factory-manager.h"
#include "default-preview-page-factory.h"

using namespace Peony;

static PreviewPageFactoryManager *globalInstance = nullptr;

PreviewPageFactoryManager *PreviewPageFactoryManager::getInstance()
{
    if (!globalInstance) {
        globalInstance = new PreviewPageFactoryManager;
    }
    return globalInstance;
}

PreviewPageFactoryManager::PreviewPageFactoryManager(QObject *parent) : QObject(parent)
{
    m_hash = new QHash<QString, PreviewPagePluginIface*>();
    //load default and plugins.
    auto defaultFactory = DefaultPreviewPageFactory::getInstance();
    registerFactory(defaultFactory->name(), static_cast<PreviewPagePluginIface*>(defaultFactory));
    //registerFactory("test", static_cast<PreviewPagePluginIface*>(defaultFactory));
}

PreviewPageFactoryManager::~PreviewPageFactoryManager()
{
    if (m_hash) {
        //FIXME: unload all module?
        delete m_hash;
    }
}

const QStringList PreviewPageFactoryManager::getPluginNames()
{
    return m_hash->keys();
}

bool PreviewPageFactoryManager::registerFactory(const QString &name, PreviewPagePluginIface *plugin)
{
    if (m_hash->value(name)) {
        return false;
    }
    m_hash->insert(name, plugin);
    return true;
}

PreviewPagePluginIface *PreviewPageFactoryManager::getPlugin(const QString &name)
{
    return m_hash->value(name);
}
