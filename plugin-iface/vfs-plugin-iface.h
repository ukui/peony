#ifndef VFSPLUGINIFACE_H
#define VFSPLUGINIFACE_H

#include "plugin-iface.h"

#define VFSPluginInterface_iid "org.ukui.peony-qt.plugin-iface.VFSPluginInterface"

namespace Peony {
class VFSPluginIface : public PluginInterface {
public:
    virtual ~VFSPluginIface() {}

    /*!
     * \brief uriScheme
     * \return
     * \details
     * indicate the uri scheme you want to register.
     */
    virtual QString uriScheme() = 0;

    /*!
     * \brief holdInSideBar
     * \return
     * \details
     * indicate if it should display as an parent item in sidebar.
     */
    virtual bool holdInSideBar() = 0;

    /*!
     * \brief initVFS
     * \details
     * peony will init your custom app vfs with this interface at early stage.
     * you should call g_vfs_register_uri_scheme() in your own implement to register
     * your vfs into peony.
     * \note
     * you should implement your vfs registion with your uri scheme.
     */
    virtual void initVFS() = 0;

    /*!
     * \brief parseUriToVFSFile
     * \param uri
     * \return a GFile Handler represent crosponding uri.
     * \details
     * backup function for while register costom uri scheme is not supported.
     * \note
     * in old glib version, there is no g_vfs_register_uri_scheme(), and in new
     * glib, register a vfs into system is asynchronous, those means it might not be
     * registered and can not use GVfsFileLookupFunc to get file handler.
     */
    virtual void* parseUriToVFSFile(const QString &uri) = 0;
};
}

Q_DECLARE_INTERFACE(Peony::VFSPluginIface, VFSPluginInterface_iid)

#endif // VFSPLUGINIFACE_H
