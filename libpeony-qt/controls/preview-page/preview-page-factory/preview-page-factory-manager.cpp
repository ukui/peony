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
    m_map = new QMap<QString, PreviewPagePluginIface*>();
    //load default and plugins.
    auto defaultFactory = DefaultPreviewPageFactory::getInstance();
    registerFactory(defaultFactory->name(), static_cast<PreviewPagePluginIface*>(defaultFactory));
    //registerFactory("test", static_cast<PreviewPagePluginIface*>(defaultFactory));
}

PreviewPageFactoryManager::~PreviewPageFactoryManager()
{
    if (m_map) {
        //FIXME: unload all module?
        delete m_map;
    }
}

const QStringList PreviewPageFactoryManager::getPluginNames()
{
    QStringList l;
    for (auto key : m_map->keys()) {
        l<<key;
    }
    return l;
}

bool PreviewPageFactoryManager::registerFactory(const QString &name, PreviewPagePluginIface *plugin)
{
    if (m_map->value(name)) {
        return false;
    }
    m_map->insert(name, plugin);
    return true;
}

PreviewPagePluginIface *PreviewPageFactoryManager::getPlugin(const QString &name)
{
    m_last_preview_page_id = name;
    return m_map->value(name);
}

const QString PreviewPageFactoryManager::getLastPreviewPageId()
{
    if (m_last_preview_page_id.isNull()) {
        return m_map->firstKey();
    }
    return m_last_preview_page_id;
}
