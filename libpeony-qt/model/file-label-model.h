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

#ifndef FILELABELMODEL_H
#define FILELABELMODEL_H

#include <QAbstractListModel>
#include <QSettings>

#include <QColor>

#define PEONY_FILE_LABEL_IDS "peony-file-label-ids"

class FileLabelItem;

class FileLabelModel : public QAbstractListModel
{
    Q_OBJECT

public:
    static FileLabelModel *getGlobalModel();

    const QStringList getLabels();
    const QList<QColor> getColors();

    int lastLabelId();

    void addLabel(const QString &label, const QColor &color);
    void removeLabel(int id);
    void setLabelName(int id, const QString &name);
    void setLabelColor(int id, const QColor &color);

    void addLabelToFile(const QString &uri, int labelId);
    void removeFileLabel(const QString &uri, int labelId = -1);

    const QList<int> getFileLabelIds(const QString &uri);
    const QStringList getFileLabels(const QString &uri);
    FileLabelItem *itemFromId(int id);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

protected:
    void initLabelItems();
    void addId();

private:
    explicit FileLabelModel(QObject *parent = nullptr);
    ~FileLabelModel();

    QSettings *m_label_settings;

    QList<FileLabelItem *> m_labels;
};

class FileLabelItem : public QObject
{
    friend class FileLabelModel;
    Q_OBJECT
public:
    explicit FileLabelItem(QObject *parent = nullptr);

    int id();
    const QString name();
    const QColor color();

    void setName(const QString &name);
    void setColor(const QColor &color);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void colorChanged(const QColor &color);

private:
    int m_id = -1; //invalid
    QString m_name = nullptr;
    QColor m_color = Qt::transparent;
};

#endif // FILELABELMODEL_H
