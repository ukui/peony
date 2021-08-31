#include "desktop-manager.h"
#include "desktop-mode-factory.h"
#include "tablet-mode-factory.h"
#include "study-center-factory.h"

#include <QDebug>

using namespace Peony;

static DesktopManager *g_desktopManager = nullptr;

DesktopManager *DesktopManager::getInstance(bool enablePreloading, QObject *parent)
{
    if (!g_desktopManager) {
        g_desktopManager = new DesktopManager(enablePreloading, parent);
    }

    return g_desktopManager;
}

DesktopManager::DesktopManager(bool enablePreloading, QObject *parent) : QObject(parent)
{
    m_enablePreloading = enablePreloading;

    //注册组件，添加新的组件后，需要手动添加到此处
    registerPlugin(DesktopModeFactory::getInstance());
    registerPlugin(TabletModeFactory::getInstance());
    registerPlugin(StudyCenterFactory::getInstance());

    //预加载全部桌面，加快切换桌面时的速度
    if (m_enablePreloading) {
        for (DesktopType desktopType : m_pluginMap.keys()) {
            getDesktopByType(desktopType);
        }
    }
}

bool DesktopManager::registerPlugin(DesktopFactoryIFace *factory)
{
    if (!factory) {
        qWarning() << "[DesktopManager::registerPlugin] factory is nullptr !";
        return false;
    }

    m_mutex.lock();

    if (m_pluginMap.value(factory->getDesktopType())) {
        m_mutex.unlock();
        qWarning() << "[DesktopManager::registerPlugin] failed," << factory->getFactoryName() << "is registered !";
        return false;
    }

    //工厂被禁用
    if (!factory->isEnable()) {
        qWarning() << "[DesktopManager::registerPlugin] failed," << factory->getFactoryName() << "factory disable !";
        m_mutex.unlock();
        return false;
    }

    m_pluginMap.insert(factory->getDesktopType(), factory);
    qWarning() << "[DesktopManager::registerPlugin]" << factory->getFactoryName() << "success !";

    //TODO 添加严格模式，在工厂添加完成后，对工厂进行检查，如果出现问题，再尝试添加一次
    m_mutex.unlock();
    return true;
}

DesktopFactoryIFace *DesktopManager::getFactoryByType(DesktopType desktopType)
{
    m_mutex.lock();
    //工厂未注册
    if (!m_pluginMap.keys().contains(desktopType)) {
        qWarning() << "[DesktopManager::getFactoryByType] failed, factory" << desktopType << " is unregistered !";
        m_mutex.unlock();
        return nullptr;
    }

    DesktopFactoryIFace* factory = m_pluginMap.value(desktopType);

    m_mutex.unlock();
    return factory;
}

DesktopWidgetBase *DesktopManager::getDesktopByType(DesktopType desktopType, QWidget *parent)
{
    DesktopFactoryIFace* factory = getFactoryByType(desktopType);
    //factory is nullptr
    if (!factory) {
        qWarning() << "[DesktopManager::getDesktopByType] error, factory is nullptr !";
        return nullptr;
    }

    DesktopWidgetBase *desktop = factory->createDesktop(parent);
    desktop->setParent(parent);

    //桌面未实例化
    if (!desktop) {
        qWarning() << "[DesktopManager::getDesktopByType] failed, " << factory->getFactoryName() << "desktop uncreated !";
        return nullptr;
    }

    //桌面处于激活状态（在顶层）
    if (desktop->isActivated()) {
        qWarning() << "[DesktopManager::getDesktopByType] failed, " << factory->getFactoryName() << "desktop isActivated !";
        return nullptr;
    }

    qWarning() << "[DesktopManager::getDesktopByType] success, " << factory->getFactoryName();
    return desktop;
}

DesktopManager::~DesktopManager()
{
    for(DesktopFactoryIFace *factory : m_pluginMap.values()) {
        factory->closeFactory();
    }

    m_pluginMap.clear();
}
