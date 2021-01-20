#ifndef PROPERTIESWINDOWTABIFACE_H
#define PROPERTIESWINDOWTABIFACE_H

#include <QWidget>

#define PropertiesWindowTabIface_iid "org.ukui.peony-qt.plugin-iface.PropertiesWindowTabInterface"
namespace Peony {

class PropertiesWindowTabIface : public QWidget
{
    Q_OBJECT

public:
    PropertiesWindowTabIface(QWidget *parent = nullptr) : QWidget(parent){}
    virtual ~PropertiesWindowTabIface() {}
    PropertiesWindowTabIface();

    bool m_thisPageChanged = false;
    /*!
     * 作为当前页面是否改变的依据
     * \brief
     */
    virtual void thisPageChanged() {
        this->m_thisPageChanged = true;
    }
    virtual void saveAllChange() = 0;

Q_SIGNALS:
    /*!
     * 当前tab页面请求关闭窗口
     * \brief
     */
    void requestCloseMainWindow();

};

}

Q_DECLARE_INTERFACE(Peony::PropertiesWindowTabIface, PropertiesWindowTabIface_iid)

#endif // PROPERTIESWINDOWTABIFACE_H
