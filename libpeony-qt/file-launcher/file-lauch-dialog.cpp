#include "file-lauch-dialog.h"

#include "file-launch-action.h"
#include "file-launch-manager.h"

#include "file-info.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QDialogButtonBox>

using namespace Peony;

FileLauchDialog::FileLauchDialog(const QString &uri, QWidget *parent) : QDialog(parent)
{
    m_layout = new QVBoxLayout(this);
    setLayout(m_layout);

    m_layout->addWidget(new QLabel(tr("Choose an Application to open this file"), this));
    m_view = new QListWidget(this);
    m_view->setIconSize(QSize(48, 48));
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_layout->addWidget(m_view, 1);
    m_check_box = new QCheckBox(tr("Set as Default"), this);
    m_layout->addWidget(m_check_box);
    m_button_box = new QDialogButtonBox(QDialogButtonBox::Ok
                                        | QDialogButtonBox::Cancel,
                                        this);
    m_layout->addWidget(m_button_box);

    m_uri = uri;
    auto actions = FileLaunchManager::getAllActions(uri);
    for (auto action : actions) {
        action->setParent(this);
        auto item = new QListWidgetItem(!action->icon().isNull()? action->icon(): QIcon::fromTheme("application-x-desktop"),
                                        action->text(),
                                        m_view);
        m_view->addItem(item);
        m_hash.insert(item, action);
    }

    connect(this, &QDialog::accepted, [=](){
        if (m_view->currentItem()) {
            auto action = m_hash.value(m_view->currentItem());
            if (m_check_box->isChecked()) {
                FileLaunchManager::setDefaultLauchAction(m_uri, action);
            }
            action->lauchFileAsync(true);
        } else {
            FileLaunchManager::openAsync(m_uri);
        }
    });

    connect(m_button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
