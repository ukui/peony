#ifndef BASICPROPERTIESPAGE_H
#define BASICPROPERTIESPAGE_H

#include <QWidget>
#include "peony-core_global.h"

#include <memory>

class QVBoxLayout;
class QFormLayout;
class QPushButton;
class QLabel;
class QLineEdit;

namespace Peony {

class FileInfo;
class FileWatcher;
class FileCountOperation;

/*!
 * \brief The BasicPropertiesPage class
 * \todo
 * handle special files, such as divice or remote server.
 */
class BasicPropertiesPage : public QWidget
{
    Q_OBJECT
public:
    explicit BasicPropertiesPage(const QStringList &uris, QWidget *parent = nullptr);
    ~BasicPropertiesPage();

protected:
    void addSeparator();

protected Q_SLOTS:
    void onSingleFileChanged(const QString &oldUri, const QString &newUri);
    void countFilesAsync(const QStringList &uris);
    void onFileCountOne(const QString &uri, quint64 size);
    void cancelCount();

    void updateInfo(const QString &uri);

private:
    QVBoxLayout *m_layout = nullptr;
    std::shared_ptr<FileInfo> m_info;
    std::shared_ptr<FileWatcher> m_watcher;

    void updateCountInfo();

    //floor1
    QPushButton *m_icon;
    QLabel *m_type;
    QLineEdit *m_display_name;
    QLabel *m_location;

    //floor2
    QLabel *m_total_size_label;
    QLabel *m_file_count_label;

    quint64 m_file_count = 0;
    quint64 m_hidden_file_count = 0;
    quint64 m_total_size = 0;

    //floor3
    QFormLayout *m_form3;
    QLabel *m_time_created_label;
    QLabel *m_time_modified_label;
    QLabel *m_time_access_label;

    quint64 m_time_created = 0;
    quint64 m_time_modified = 0;
    quint64 m_time_access = 0;

    //floor4

    //


    FileCountOperation *m_count_op = nullptr;
};

}

#endif // BASICPROPERTIESPAGE_H
