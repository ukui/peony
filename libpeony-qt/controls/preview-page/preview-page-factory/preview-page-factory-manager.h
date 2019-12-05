#ifndef PREVIEWPAGEFACTORYMANAGER_H
#define PREVIEWPAGEFACTORYMANAGER_H

#include "peony-core_global.h"
#include <QObject>
#include <QMap>

namespace Peony {

class PreviewPagePluginIface;

/*!
 * \brief The PreviewPageFactoryManager class
 * \details
 * In peony-qt, we have a optional extra preview view at
 * right of the window.
 * PreviewPage is provided by an inplement of PreviewPagePluginInterface.
 * Actually, the interface is represent a factory template for
 * create a preview page.
 * We use this class manage all the factories of preview page.
 * When the manger instance init, the factories will be loaded
 * and registered in manager. Then we can call the createPreviewPage()
 * function of them when we need. The registered factory can be
 * listed in a GUI, and we can disable/enable them thourgh the
 * setEnable() method.
 * \note
 * The manager is single and global, and factory too.
 * Do not try newing or deleting them.
 */
class PEONYCORESHARED_EXPORT PreviewPageFactoryManager : public QObject
{
    Q_OBJECT
public:
    static PreviewPageFactoryManager *getInstance();

    bool registerFactory(const QString &name, PreviewPagePluginIface* plugin);

    const QStringList getPluginNames();
    PreviewPagePluginIface *getPlugin(const QString &name);

    const QString getLastPreviewPageId();

private:
    explicit PreviewPageFactoryManager(QObject *parent = nullptr);
    ~PreviewPageFactoryManager();

    QMap<QString, PreviewPagePluginIface*> *m_map = nullptr;
    QString m_last_preview_page_id = nullptr;
};

}

#endif // PREVIEWPAGEFACTORYMANAGER_H
