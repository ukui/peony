#include "desktop-mode.h"

using namespace Peony;

#include "desktop-menu.h"

#include <QBoxLayout>
#include <QPushButton>

DesktopMode::DesktopMode(QWidget *parent) : DesktopWidgetBase(parent)
{
    //创建桌面时，默认未激活
    m_isActivated = false;

    //设置ui
    this->initUI();

}

DesktopMode::~DesktopMode()
{

}

DesktopWidgetBase *DesktopMode::initDesktop(const QRect &rect)
{
    DesktopWidgetBase::initDesktop(rect);

//    m_view->refresh();
    qDebug() << "DesktopMode::initDesktop:" << this->geometry() << m_view->geometry() << m_view->pos() << m_boxLayout->geometry();

    return this;
}

void DesktopMode::initUI()
{
    this->setContentsMargins(0, 0, 0, 0);

    m_boxLayout = new QVBoxLayout(this);

    m_boxLayout->setContentsMargins(0, 0, 0, 0);
    m_boxLayout->setAlignment(Qt::AlignCenter);

    this->setLayout(m_boxLayout);

    m_view = new DesktopIconView(this);

    m_boxLayout->addWidget(m_view);

    //右键菜单
    this->initMenu();
}

void DesktopMode::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);

    //桌面被激活就显示,取消激活就隐藏
    if (m_view) {
        m_view->setHidden(!activated);
    }
}

void DesktopMode::initMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    // menu
    connect(this, &QWidget::customContextMenuRequested, [=](const QPoint &pos) {
        // FIXME: use other menu
        qDebug() << "menu request in desktop window";
        auto contentMargins = contentsMargins();
//        auto fixedPos = pos - QPoint(contentMargins.left(), contentMargins.top());
        auto index = m_view->indexAt(QCursor::pos());
//        auto selectcount = m_view->getSelections().count();
        if (!index.isValid()) {
            m_view->clearSelection();
        } else {
            if (!m_view->selectionModel()->selection().indexes().contains(index)) {
                m_view->clearSelection();
                m_view->selectionModel()->select(index, QItemSelectionModel::Select);
            }
        }

//        if (index.isValid()) {
//            //! \note 针对mdm禁用后的快捷方式不弹出右键
//            auto model = static_cast<DesktopItemModel*>(m_view->model());
//            auto info = FileInfo::fromUri(model->data(index, DesktopItemModel::Role::UriRole).toString());
//            if (info->isExecDisable())
//                return;
//        }

        QTimer::singleShot(1, [=]() {
            DesktopMenu menu(m_view);
            if (m_view->getSelections().isEmpty()) {
                auto action = menu.addAction(tr("set background"));
                connect(action, &QAction::triggered, [=]() {
                    //go to control center set background
                    Q_EMIT gotoSetBackground();
                });
                //测试代码，删除即可
                auto action1 = menu.addAction(tr("go to other desktop"));
                connect(action1, &QAction::triggered, [=]() {
                    m_exitAnimationType = AnimationType::OpacityLess;
                    Q_EMIT moveToOtherDesktop(DesktopType::Tablet, AnimationType::OpacityFull);
                });
            }
            menu.exec(QCursor::pos());
            auto urisToEdit = menu.urisToEdit();
            if (urisToEdit.count() == 1) {
                QTimer::singleShot(
                        100, this, [=]() {
                            m_view->editUri(urisToEdit.first());
                        });
            }
        });
    });
}
