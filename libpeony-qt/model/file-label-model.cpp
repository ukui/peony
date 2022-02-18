/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "file-label-model.h"

#include "file-meta-info.h"
#include "file-info.h"
#include "audio-play-manager.h"

#include <QMessageBox>

static FileLabelModel *global_instance = nullptr;

FileLabelModel::FileLabelModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_label_settings = new QSettings(QSettings::UserScope, "org.ukui", "peony-qt", this);
    if (m_label_settings->value("lastid").isNull()) {
        //adjsut color value to design instead of Qt define color,task#25507
        QColor Red(0xFA6056);
        QColor Orange(0xF8A34C);
        QColor Yellow(0xF7CE52);
        QColor Green(0x5FD065);
        QColor Blue(0x478EF8);
        QColor Purple(0xB470D5);
        QColor Gray(0x9D9DA0);
        //init settings
        addLabel(tr("Red"), Red);
        addLabel(tr("Orange"), Orange);
        addLabel(tr("Yellow"), Yellow);
        addLabel(tr("Green"), Green);
        addLabel(tr("Blue"), Blue);
        addLabel(tr("Purple"), Purple);
        addLabel(tr("Gray"), Gray);
        //addLabel(tr("Transparent"), Qt::transparent);
    } else {
        initLabelItems();
    }
}

FileLabelModel::~FileLabelModel()
{

}

FileLabelModel *FileLabelModel::getGlobalModel()
{
    if (!global_instance) {
        global_instance = new FileLabelModel;
    }
    return global_instance;
}

const QStringList FileLabelModel::getLabels()
{
    QStringList l;

    int size = m_label_settings->beginReadArray("labels");
    for (int i = 0; i < size; i++) {
        m_label_settings->setArrayIndex(i);
        if (m_label_settings->value("visible").toBool()) {
            l<<m_label_settings->value("label").toString();
        }
    }
    m_label_settings->endArray();

    return l;
}

const QList<QColor> FileLabelModel::getColors()
{
    QList<QColor> l;

    int size = m_label_settings->beginReadArray("labels");
    for (int i = 0; i < size; i++) {
        m_label_settings->setArrayIndex(i);
        if (m_label_settings->value("visible").toBool()) {
            l<<qvariant_cast<QColor>(m_label_settings->value("color"));
        }
    }
    m_label_settings->endArray();

    return l;
}

int FileLabelModel::lastLabelId()
{
    m_label_settings = new QSettings(QSettings::UserScope, "org.ukui", "peony-qt", this);
    if (m_label_settings->value("lastid").isNull()) {
        return 0;
    } else {
        return m_label_settings->value("lastid").toInt();
    }
}

void FileLabelModel::addLabel(const QString &label, const QColor &color)
{
    beginResetModel();

    if (getLabels().contains(label) || getColors().contains(color)) {
        Peony::AudioPlayManager::getInstance()->playWarningAudio();
        QMessageBox::critical(nullptr, tr("Error"), tr("Label or color is duplicated."));
        return;
    }

    int lastid = lastLabelId();
    m_label_settings->beginWriteArray("labels");
    m_label_settings->setArrayIndex(lastid + 1);
    m_label_settings->setValue("label", label);
    m_label_settings->setValue("color", color);
    m_label_settings->setValue("visible", true);
    m_label_settings->endArray();

    auto item = new FileLabelItem(this);
    item->m_id = lastid + 1;
    item->m_name = label;
    item->m_color = color;

    m_labels.append(item);

    addId();

    connect(item, &FileLabelItem::nameChanged, this, [=](const QString &name) {
        m_label_settings->beginWriteArray("labels");
        m_label_settings->setArrayIndex(item->id());
        m_label_settings->setValue("label", name);
        m_label_settings->endArray();
        m_label_settings->sync();
    });

    connect(item, &FileLabelItem::colorChanged, this, [=](const QColor &color) {
        m_label_settings->beginWriteArray("labels");
        m_label_settings->setArrayIndex(item->id());
        m_label_settings->setValue("color", color);
        m_label_settings->endArray();
        m_label_settings->sync();
    });

    endResetModel();
}

void FileLabelModel::removeLabel(int id)
{
    beginResetModel();

    for (auto item : m_labels) {
        if (item->id() == id) {
            m_labels.removeOne(item);
            item->deleteLater();
            break;
        }
    }

    m_label_settings->beginWriteArray("labels", lastLabelId() + 1);
    m_label_settings->setArrayIndex(id);
    m_label_settings->setValue("visible", false);
    m_label_settings->endArray();
    m_label_settings->sync();

    Q_EMIT dataChanged(QModelIndex(), QModelIndex());

    endResetModel();
}

void FileLabelModel::setLabelName(int id, const QString &name)
{
    for (auto item : m_labels) {
        if (item->id() == id) {
            item->setName(name);
            int row = m_labels.indexOf(item);
            Q_EMIT dataChanged(index(row), index(row));
            break;
        }
    }
}

void FileLabelModel::setLabelColor(int id, const QColor &color)
{
    for (auto item : m_labels) {
        if (item->id() == id) {
            item->setColor(color);
            int row = m_labels.indexOf(item);
            Q_EMIT dataChanged(index(row), index(row));
            break;
        }
    }
}

const QList<int> FileLabelModel::getFileLabelIds(const QString &uri)
{
    QList<int> l;
    auto metaInfo = Peony::FileMetaInfo::fromUri(uri);
    if (! metaInfo || metaInfo->getMetaInfoVariant(PEONY_FILE_LABEL_IDS).isNull())
        return l;
    auto labels = metaInfo->getMetaInfoStringList(PEONY_FILE_LABEL_IDS);
    for (auto label : labels) {
        l<<label.toInt();
    }
    return l;
}

const QStringList FileLabelModel::getFileLabels(const QString &uri)
{
    QStringList l;
    auto metaInfo = Peony::FileMetaInfo::fromUri(uri);
    if (! metaInfo || metaInfo->getMetaInfoVariant(PEONY_FILE_LABEL_IDS).isNull())
        return l;
    auto labels = metaInfo->getMetaInfoStringList(PEONY_FILE_LABEL_IDS);
    for (auto label : labels) {
        auto id = label.toInt();
        auto item = itemFromId(id);
        if (item) {
            l<<item->name();
        }
    }
    return l;
}

const QList<QColor> FileLabelModel::getFileColors(const QString &uri)
{
    QList<QColor> l;
    auto metaInfo = Peony::FileMetaInfo::fromUri(uri);
    if (! metaInfo || metaInfo->getMetaInfoVariant(PEONY_FILE_LABEL_IDS).isNull())
        return l;
    auto labels = metaInfo->getMetaInfoStringList(PEONY_FILE_LABEL_IDS);
    for (auto label : labels) {
        auto id = label.toInt();
        auto item = itemFromId(id);
        if (item) {
            l<<item->color();
        }
    }
    return l;
}

FileLabelItem *FileLabelModel::itemFromId(int id)
{
    for (auto item : this->m_labels) {
        if (id == item->id()) {
            return item;
        }
    }
    return nullptr;
}

FileLabelItem *FileLabelModel::itemFormIndex(const QModelIndex &index)
{
    if (index.isValid()) {
        return m_labels.at(index.row());
    }
    return nullptr;
}

QList<FileLabelItem *> FileLabelModel::getAllFileLabelItems()
{
    return m_labels;
}

void FileLabelModel::addLabelToFile(const QString &uri, int labelId)
{
    auto metaInfo = Peony::FileMetaInfo::fromUri(uri);
    QStringList labelIds;
    if (metaInfo && !metaInfo->getMetaInfoVariant(PEONY_FILE_LABEL_IDS).isNull())
        labelIds = metaInfo->getMetaInfoStringList(PEONY_FILE_LABEL_IDS);
    labelIds<<QString::number(labelId);
    labelIds.removeDuplicates();
    metaInfo->setMetaInfoStringList(PEONY_FILE_LABEL_IDS, labelIds);
    Q_EMIT fileLabelChanged(uri);
}

void FileLabelModel::removeFileLabel(const QString &uri, int labelId)
{
    auto metaInfo = Peony::FileMetaInfo::fromUri(uri);
    if (! metaInfo)
        return;
    if (labelId <= 0) {
        metaInfo->removeMetaInfo(PEONY_FILE_LABEL_IDS);
    } else {
        if (metaInfo->getMetaInfoVariant(PEONY_FILE_LABEL_IDS).isNull())
            return;
        QStringList labelIds = metaInfo->getMetaInfoStringList(PEONY_FILE_LABEL_IDS);
        labelIds.removeOne(QString::number(labelId));
        metaInfo->setMetaInfoStringList(PEONY_FILE_LABEL_IDS, labelIds);
    }
    Q_EMIT fileLabelChanged(uri);
}

int FileLabelModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return m_labels.size();
}

QVariant FileLabelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch (role) {
    case Qt::DisplayRole: {
        return m_labels.at(index.row())->name();
    }
    case Qt::DecorationRole: {
        return m_labels.at(index.row())->color();
    }
    default:
        return QVariant();
    }
}

bool FileLabelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        auto name = value.toString();
        if (name.isEmpty()) {
            return false;
        }
        if (getLabels().contains(name)) {
            Peony::AudioPlayManager::getInstance()->playWarningAudio();
            QMessageBox::critical(nullptr, tr("Error"), tr("Label or color is duplicated."));
            return false;
        }
        this->setLabelName(m_labels.at(index.row())->id(), name);
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags FileLabelModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable; // FIXME: Implement me!
}

bool FileLabelModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool FileLabelModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

void FileLabelModel::setName(FileLabelItem *item, const QString &name)
{
    m_label_settings->beginWriteArray("labels", lastLabelId() + 1);
    m_label_settings->setArrayIndex(item->id());
    m_label_settings->setValue("label", name);
    m_label_settings->endArray();
    m_label_settings->sync();
}

void FileLabelModel::setColor(FileLabelItem *item, const QColor &color)
{
    m_label_settings->beginWriteArray("labels", lastLabelId() + 1);
    m_label_settings->setArrayIndex(item->id());
    m_label_settings->setValue("color", color);
    m_label_settings->endArray();
    m_label_settings->sync();
}

void FileLabelModel::initLabelItems()
{
    beginResetModel();
    auto size = m_label_settings->beginReadArray("labels");
    for (int i = 0; i < size; i++) {
        m_label_settings->setArrayIndex(i);
        bool visible = m_label_settings->value("visible").toBool();
        if (visible) {
            auto name = m_label_settings->value("label").toString();
            auto color = qvariant_cast<QColor>(m_label_settings->value("color"));

            auto item = new FileLabelItem(this);
            item->m_id = i;
            item->m_name = name;
            item->m_color = color;

            m_labels.append(item);
        }
    }
    m_label_settings->endArray();
    endResetModel();
}

void FileLabelModel::addId()
{
    int lastid = lastLabelId();
    m_label_settings->setValue("lastid", lastid + 1);
    m_label_settings->sync();
}

//FileLabelItem
FileLabelItem::FileLabelItem(QObject *parent)
{
    //should be initialized in model.
}

int FileLabelItem::id()
{
    return m_id;
}

const QString FileLabelItem::name()
{
    return m_name;
}

const QColor FileLabelItem::color()
{
    return m_color;
}

void FileLabelItem::setName(const QString &name)
{
    m_name = name;
    if (m_id >= 0) {
        if (global_instance)
            global_instance->setName(this, name);
    }
}

void FileLabelItem::setColor(const QColor &color)
{
    if (color.blackF() == 1) {
        auto black = color;
        black.setRgbF(0.01, 0.01, 0.01);
        setColor(black);
        return;
    }
    m_color = color;
    if (m_id >= 0) {
        if (global_instance)
            global_instance->setColor(this, color);
    }
}
