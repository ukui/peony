/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

#include "mark-properties-page.h"
#include "file-operation-utils.h"

#include <QHeaderView>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QUrl>

using namespace Peony;

MarkPropertiesPage::MarkPropertiesPage(const QString &uri, QWidget *parent) : PropertiesWindowTabIface(parent)
{
    this->m_uri = uri;
    //note:请查看：BasicPropertiesPage::getFIleInfo(QString uri) - Look BasicPropertiesPage::getFIleInfo(QString uri)
    if (uri.startsWith("favorite://")) {
        QUrl url(uri);
        m_uri = "file://" + url.path();
    }

    this->m_layout = new QVBoxLayout(this);
    //表格自带一部分高度，所以手动删减一部分
    m_layout->setContentsMargins(16,16,16,20);

    this->initTableWidget();

    this->initTableData();

    this->setLayout(m_layout);
}

void MarkPropertiesPage::initTableWidget()
{
    this->m_tableWidget = new QTableWidget(this);
    m_tableWidget->setContentsMargins(0,0,0,0);

    m_tableWidget->setRowCount(4);
    m_tableWidget->setColumnCount(2);
    m_tableWidget->setSelectionMode(QTableWidget::NoSelection);
    m_tableWidget->setFrameShape(QFrame::NoFrame);
    m_tableWidget->horizontalHeader()->setVisible(false);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //显示线条
    m_tableWidget->setShowGrid(false);

    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->resizeRowsToContents();

    //隔行换色
    m_tableWidget->setAlternatingRowColors(false);
    m_layout->addWidget(m_tableWidget);
}

MarkPropertiesPage::~MarkPropertiesPage()
{

}

void MarkPropertiesPage::saveAllChange()
{
    if(!this->m_thisPageChanged)
        return;
    //1.清除旧的标记
    for (auto labelId : m_fileLabelModel->getFileLabelIds(m_uri)) {
        this->m_fileLabelModel->removeFileLabel(m_uri,labelId);
    }
    //2.写入新的标记
    for(auto labelId : this->m_thisFileLabelIds) {
        m_fileLabelModel->addLabelToFile(m_uri,labelId);
    }
}

void MarkPropertiesPage::initTableData()
{
    m_fileLabelModel = FileLabelModel::getGlobalModel();

    m_thisFileLabelIds = m_fileLabelModel->getFileLabelIds(m_uri);
    QList<FileLabelItem *> allLabels = m_fileLabelModel->getAllFileLabelItems();

    int rowCount = (allLabels.count()%2 == 0 ? allLabels.count()/2 : allLabels.count()/2 + 1);
    m_tableWidget->setRowCount(rowCount);
    int totalLabel = allLabels.count()%2 ? allLabels.count()+1 : allLabels.count();

    for (int i = 0; i < totalLabel; i++) {
        QWidget *widget = new QWidget(m_tableWidget);
        QHBoxLayout *boxLayout = new QHBoxLayout(m_tableWidget);
        boxLayout->setAlignment(Qt::AlignLeft);
        boxLayout->setContentsMargins(8,0,8,0);
        widget->setLayout(boxLayout);
        //fix last single box can input letters issue, bug#38757
        if (i >= allLabels.count())
        {
            QLabel *label = new QLabel(widget);
            label->setText("");
            boxLayout->addWidget(label);
            m_tableWidget->setCellWidget(i/2,i%2,widget);
            continue;
        }

        auto item = allLabels.at(i);

        QCheckBox *checkBox = new QCheckBox(widget);
        checkBox->setChecked(m_thisFileLabelIds.contains(item->id()));
        boxLayout->addWidget(checkBox);

        QPushButton *button = new QPushButton(widget);
        button->palette().window();
        button->setStyleSheet("QPushButton{"
                              "border-radius: 6px; "
                              "background-color: " + convertRGB16HexStr(item->color()) + ";"
                              " max-width:12px;"
                              " max-height:12px;"
                              " min-width:12px;"
                              " min-height:12px;"
                              "}");
        button->setEnabled(false);
        boxLayout->addWidget(button);

        QLabel *label = new QLabel(widget);
        label->setText(item->name());
        boxLayout->addWidget(label);
        connect(checkBox,&QCheckBox::clicked,this,[=](bool checked){
            this->changeLabel(item->id(),checked);
        });

        m_tableWidget->setCellWidget(i/2,i%2,widget);
    }

    m_layout->addWidget(m_tableWidget);
}

void MarkPropertiesPage::changeLabel(int labelId, bool checked)
{
    qDebug() << "id" << labelId << checked;
    //1.检查是否存在
    bool hasLabelId = this->m_thisFileLabelIds.contains(labelId);

    //2.如果选中并且不存在
    if(checked && !hasLabelId)
        this->m_thisFileLabelIds.append(labelId);

    //3.如果取消选中，并且存在
    if(!checked && hasLabelId)
        this->m_thisFileLabelIds.removeAt(this->m_thisFileLabelIds.indexOf(labelId));

    this->thisPageChanged();
}

QString MarkPropertiesPage::convertRGB16HexStr(const QColor color)
{
    if (color == Qt::transparent)
        return "transparent";
    QString redStr = QString("%1").arg(color.red(),2,16,QChar('0'));
    QString greenStr = QString("%1").arg(color.green(),2,16,QChar('0'));
    QString blueStr = QString("%1").arg(color.blue(),2,16,QChar('0'));
    QString hexStr = "#" + redStr + greenStr + blueStr;
    return hexStr;
}
