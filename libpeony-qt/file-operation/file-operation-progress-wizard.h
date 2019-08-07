#ifndef FILEOPERATIONPROGRESSWIZARD_H
#define FILEOPERATIONPROGRESSWIZARD_H

#include <QWizard>

class QLabel;
class QProgressBar;

namespace Peony {

class FileOperationPreparePage;
class FileOperationProgressPage;

class FileOperationProgressWizard : public QWizard
{
    friend class FileOperationPreparePage;
    friend class FileOperationProgressPage;
    Q_OBJECT
public:
    enum PageId {
        PreparePage,
        ProgressPage
    };

    explicit FileOperationProgressWizard(QWidget *parent = nullptr);
    ~FileOperationProgressWizard();

Q_SIGNALS:
    void cancelled();

public Q_SLOTS:
    void switchToPreparedPage();
    void onElementFound(const QString &uri, const qint64 &size);
    void switchToProgressPage();
    void onElementOperationFinished(const QString &uri, const qint64 &size);

private:
    qint64 m_total_size = 0;
    int m_current_size = 0;
    int m_total_count = 0;
};

class FileOperationPreparePage : public QWizardPage
{
    friend class FileOperationProgressWizard;
    Q_OBJECT
public:
    explicit FileOperationPreparePage(QWidget *parent = nullptr);
    ~FileOperationPreparePage();

Q_SIGNALS:
    void cancelled();

private:
    QLabel *m_src_line = nullptr;
    QLabel *m_count_line = nullptr;
    QLabel *m_size_line = nullptr;
};

class FileOperationProgressPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit FileOperationProgressPage(QWidget *parent = nullptr);
    ~FileOperationProgressPage();

Q_SIGNALS:
    void cancelled();

private:
    QLabel *m_src_line = nullptr;
    QLabel *m_dest_line = nullptr;
    QLabel *m_state_line = nullptr;
    QProgressBar *m_progress_bar = nullptr;
};

}

#endif // FILEOPERATIONPROGRESSWIZARD_H
