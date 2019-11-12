#ifndef DEFAULTPREVIEWPAGE_H
#define DEFAULTPREVIEWPAGE_H

#include "peony-core_global.h"
#include "preview-page-plugin-iface.h"
#include <QStackedWidget>

#include <memory>

class QGridLayout;
class QPushButton;
class QFormLayout;
class QLabel;

namespace Peony {

class FileInfo;
class FileWatcher;
class IconContainer;

/*!
 * \brief The DefaultPreviewPage class
 * \todo
 * Implement a preview page containing file thumbnail and descriptions.
 */
class PEONYCORESHARED_EXPORT DefaultPreviewPage : public QStackedWidget, public PreviewPageIface
{
    Q_OBJECT
public:
    explicit DefaultPreviewPage(QWidget *parent = nullptr);
    ~DefaultPreviewPage() override;

    void prepare(const QString &uri, PreviewType type) override;
    void prepare(const QString &uri) override;
    void startPreview() override;
    void cancel() override;
    void closePreviewPage() override;

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QString m_current_uri;
    PreviewType m_current_type;

    QWidget *m_empty_tab_widget;
    QWidget *m_preview_tab_widget;

    std::shared_ptr<FileInfo> m_info;
    std::shared_ptr<FileWatcher> m_watcher;

    bool m_support = true;
};

class FileCountOperation;
class FilePreviewPage : public QFrame
{
    friend class DefaultPreviewPage;
    Q_OBJECT
private:
    explicit FilePreviewPage(QWidget *parent = nullptr);
    ~FilePreviewPage();

private Q_SLOTS:
    void updateInfo(FileInfo *info);
    void countAsync(const QString &uri);
    void updateCount();
    void cancel();

    void resizeIcon(QSize size);

protected Q_SLOTS:
    void resetCount();
    void onPreparedOne(const QString &uri, quint64 size) {
        m_file_count++;
        if (uri.contains("/.")) {
            m_hidden_count++;
        }
        m_total_size += size;
        this->updateCount();
    }
    void onCountDone();

private:
    FileCountOperation *m_count_op = nullptr;
    quint64 m_file_count = 0;
    quint64 m_hidden_count = 0;
    quint64 m_total_size = 0;

    QGridLayout *m_layout;
    IconContainer *m_icon;
    QFormLayout *m_form;
    QLabel *m_display_name_label;
    QLabel *m_type_label;
    QLabel *m_file_count_label;
    QLabel *m_total_size_label;
    QLabel *m_time_modified_label;
    QLabel *m_time_access_label;

    //image
    QLabel *m_image_size;
    QLabel *m_image_format;
};

}

#endif // DEFAULTPREVIEWPAGE_H
