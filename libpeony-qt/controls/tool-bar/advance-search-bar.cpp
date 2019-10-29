#include "advance-search-bar.h"
#include "fm-window.h"
#include "search-vfs-uri-parser.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QStringListModel>
#include <QFileDialog>

#include <QDebug>

using namespace Peony;

AdvanceSearchBar::AdvanceSearchBar(FMWindow *window, QWidget *parent) : QWidget(parent)
{
    m_top_window = window;
    init(window? true: false);
}

void AdvanceSearchBar::init(bool hasTopWindow)
{
    //add mutiple filter page
    m_filter = new QWidget(this);
    //m_filter->setFixedWidth(160);
    m_filter->setContentsMargins(0, 0, 0, 0);
    QLabel *keyLabel = new QLabel(tr("Key Words"), m_filter);
    //keyLabel->setText("关键词");
    m_advanced_key = new QLineEdit(m_filter);
    m_advanced_key->setFixedWidth(120);
    keyLabel->setBuddy(m_advanced_key);
    m_advanced_key->setPlaceholderText(tr("input key words..."));
    QLabel *searchLocation = new QLabel(tr("Search Location"), m_filter);
    //searchLocation->setText("搜索位置");
    m_advance_bar = new AdvancedLocationBar(m_filter);
    m_advance_bar->setFixedWidth(120);
    QString uri = m_top_window->getCurrentUri();
    m_advance_bar->updateLocation(uri);
    m_advance_target_path = uri;

    QPushButton *m_browse_button = new QPushButton(tr("browse"), nullptr);
    //m_browse_button->setText("浏览");
    m_browse_button->setFixedWidth(80);

    QLabel *fileType = new QLabel(tr("File Type"), m_filter);
    //fileType->setText("搜索类型");
    fileType->setFixedWidth(120);
    typeViewCombox = new QComboBox(m_filter);
    typeViewCombox->setToolTip(tr("Choose File Type"));
    typeViewCombox->setFixedWidth(120);
    auto model = new QStringListModel(m_filter);
    model->setStringList(m_file_type_list);
    typeViewCombox->setModel(model);

    QLabel *modifyTime = new QLabel(tr("Modify Time"), m_filter);
    //modifyTime->setText("修改时间");
    modifyTime->setFixedWidth(120);
    timeViewCombox = new QComboBox(m_filter);
    timeViewCombox->setToolTip(tr("Choose Modify Time"));
    timeViewCombox->setFixedWidth(120);
    auto time_model = new QStringListModel(m_filter);
    time_model->setStringList(m_file_mtime_list);
    timeViewCombox->setModel(time_model);

    QLabel *fileSize = new QLabel(tr("File Size"), m_filter);
    //fileSize->setText("文件大小");
    fileSize->setFixedWidth(120);
    sizeViewCombox = new QComboBox(m_filter);
    sizeViewCombox->setToolTip(tr("Choose file size"));
    sizeViewCombox->setFixedWidth(120);
    auto size_model = new QStringListModel(m_filter);
    size_model->setStringList(m_file_size_list);
    sizeViewCombox->setModel(size_model);

    QPushButton *m_show_hidden_button = new QPushButton(tr("show hidden file"), nullptr);
    //m_show_hidden_button->setText("显示隐藏文件");
    m_show_hidden_button->setFixedWidth(120);

    QPushButton *m_filter_button = new QPushButton(tr("search"), nullptr);
    //m_filter_button->setText("搜索");
    m_filter_button->setFixedWidth(80);
    m_filter_button->setToolTip("start search");

    QFormLayout *topLayout = new QFormLayout();
    topLayout->setContentsMargins(10, 10, 10, 10);
    QWidget *b1 = new QWidget(m_filter);
    QHBoxLayout *middleLayout = new QHBoxLayout(b1);
    b1->setLayout(middleLayout);
    QWidget *b2 = new QWidget(m_filter);
    QHBoxLayout *bottomLayout = new QHBoxLayout(b2);
    b2->setLayout(bottomLayout);
    QVBoxLayout *mainLayout = new QVBoxLayout(m_filter);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(middleLayout);
    mainLayout->addLayout(bottomLayout);

    topLayout->addWidget(keyLabel);
    topLayout->addWidget(m_advanced_key);
    topLayout->addWidget(searchLocation);
    topLayout->addWidget(m_advance_bar);
    middleLayout->addWidget(m_browse_button, Qt::AlignCenter);
    middleLayout->setContentsMargins(10,10,10,10);
    topLayout->addWidget(b1);
    topLayout->addWidget(fileType);
    topLayout->addWidget(typeViewCombox);
    topLayout->addWidget(modifyTime);
    topLayout->addWidget(timeViewCombox);
    topLayout->addWidget(fileSize);
    topLayout->addWidget(sizeViewCombox);
    topLayout->addWidget(m_show_hidden_button);
    bottomLayout->setContentsMargins(10,30,10,10);
    bottomLayout->addWidget(m_filter_button, Qt::AlignCenter);
    topLayout->addWidget(b2);
    //end mutiple filter


    connect(m_browse_button, &QPushButton::clicked, this, &AdvanceSearchBar::browsePath);
    connect(m_filter_button, &QPushButton::clicked, this, &AdvanceSearchBar::searchFilter);
    connect(m_show_hidden_button, &QPushButton::clicked, this, &AdvanceSearchBar::setShowHidden);
    connect(typeViewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &AdvanceSearchBar::filterUpdate);
    connect(timeViewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &AdvanceSearchBar::filterUpdate);
    connect(sizeViewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &AdvanceSearchBar::filterUpdate);

    //advance search change Location
    connect(m_advance_bar, &AdvancedLocationBar::updateWindowLocationRequest, [=](const QString &uri){
        if (uri.contains("file://"))
            m_advance_target_path = uri;
        else
            m_advance_target_path = "file://" + uri;
    });
}

void AdvanceSearchBar::browsePath()
{
    QString target_path = QFileDialog::getExistingDirectory(this, "caption", m_top_window->getCurrentUri(), QFileDialog::ShowDirsOnly);
    //qDebug()<<"browsePath Opened:"<<target_path;
    //add root prefix
    if (target_path.contains("file://"))
        m_advance_target_path = target_path;
    else
        m_advance_target_path = "file://" + target_path;

    updateLocation(target_path);
}

void AdvanceSearchBar::searchFilter()
{
    //qDebug()<<"searchFilter clicked"<<m_advanced_key->text()<<"path:"<<m_advance_target_path;
    if (m_advanced_key->text() == nullptr || m_advance_target_path == nullptr) //must have key words and target path
        return;

    m_top_window->searchFilter(m_advance_target_path, m_advanced_key->text());
}

void AdvanceSearchBar::filterUpdate()
{
    m_top_window->filterUpdate(typeViewCombox->currentIndex(), timeViewCombox->currentIndex(), sizeViewCombox->currentIndex());
}

void AdvanceSearchBar::setShowHidden()
{
    m_top_window->setShowHidden();
}

void AdvanceSearchBar::updateLocation(const QString uri)
{
     //only update path
     //qDebug()<<"uri:"<<uri;
     m_advance_target_path = uri;
     m_advance_bar->updateLocation(uri);
}

void AdvanceSearchBar::clearData()
{
    //m_advanced_key->setText("");
    typeViewCombox->setCurrentIndex(0);
    timeViewCombox->setCurrentIndex(0);
    sizeViewCombox->setCurrentIndex(0);
}
