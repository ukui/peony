#ifndef FILEOPERATIONPROGRESS_H
#define FILEOPERATIONPROGRESS_H

#include <QMap>
#include <QLabel>
#include <QMutex>
#include <QWidget>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMainWindow>

class CloseButton;
class ProgressBar;
class DetailButton;
class StartStopButton;
class FileOperationProgress;

class FileOperationProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum Status
    {
        STOP,
        PREPARE,
        RUNNING,
        FINISHED,
        CANCELED,
        FREE,
    };
public:
    static FileOperationProgressBar* getInstance();

    FileOperationProgress* addFileOperation ();
    void showProcess(FileOperationProgress& proc);
    void removeFileOperation(FileOperationProgress& fileOperation);

private:
    explicit FileOperationProgressBar(QWidget *parent = nullptr);
    ~FileOperationProgressBar();

public Q_SLOTS:
    void detailInfo (bool open);

protected:
    void closeEvent(QCloseEvent *e) override;

private:
    float m_width = 500;
    float m_height = 150;
    float m_margin = 20;
    float m_max_height = 300;

    QFrame* m_spline = nullptr;
    QLabel* m_detail_label = nullptr;
    QWidget* m_detail_widget = nullptr;
    QScrollArea* m_scroll_area = nullptr;
    QVBoxLayout* m_main_layout = nullptr;
    DetailButton* m_show_detail = nullptr;
    QHBoxLayout* m_detail_layout = nullptr;

    QWidget* m_scroll_widget = nullptr;
    QMainWindow* m_process_widget = nullptr;
    QVBoxLayout* m_process_layout = nullptr;

    int m_inuse_process = 0;
    const int m_max_progressbar = 20;                               // reserved one for undo or redo
    QMutex *m_process_locker = nullptr;                             // maybe not need
    QMap<FileOperationProgress*, Status>* m_process = nullptr;      // all of processbar, fixme
};

class FileOperationProgress : public QWidget
{
    friend FileOperationProgressBar;
    Q_OBJECT
public:
    explicit FileOperationProgress(float width, float heigth, QWidget *parent = nullptr);

private:
    ~FileOperationProgress();
    void initParam ();

Q_SIGNALS:
    void cancelled();
    void finished(FileOperationProgress& fop);

public Q_SLOTS:
    virtual void delayShow();

    virtual void switchToPreparedPage();
    virtual void onElementFoundOne(const QString &uri, const qint64 &size);
    virtual void onElementFoundAll();

    virtual void switchToProgressPage();
    virtual void onFileOperationProgressedOne(const QString &uri, const QString &destUri, const qint64 &size);
    virtual void onFileOperationProgressedAll();

    virtual void switchToAfterProgressPage();
    virtual void onElementClearOne(const QString &uri);

    virtual void switchToRollbackPage();
    virtual void onFileRollbacked(const QString &destUri, const QString &srcUri);

    virtual void updateProgress(const QString &srcUri, const QString &destUri, quint64 current, quint64 total);

    virtual void onStartSync();
    void onFinished ();

private:
    int m_total_count = 0;
    int m_current_count = 1;
    qint64 m_total_size = 0;
    qint32 m_current_size = 0;
    QString m_src_uri;
    QString m_dest_uri;

    CloseButton* m_close = nullptr;
    QLabel* m_process_name = nullptr;
    QLabel* m_process_file = nullptr;
    ProgressBar* m_progress = nullptr;
    QLabel* m_process_percent = nullptr;
    QHBoxLayout* m_vbox_layout = nullptr;
    QVBoxLayout* m_main_layout = nullptr;
    QLabel* m_process_left_item = nullptr;
    StartStopButton* m_start_stop = nullptr;
};

class ProgressBar : public QWidget
{
    friend FileOperationProgress;
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);
    ~ProgressBar();

protected:
    void paintEvent(QPaintEvent *event) override;

public Q_SLOTS:
    void updateValue(double value);

public:

private:
    QRectF m_area;
    bool m_detail = false;
    double m_current_value = 1.0;
};

class DetailButton : public QWidget
{
    Q_OBJECT
public:
    explicit DetailButton(QWidget *parent = nullptr);

Q_SIGNALS:
    void valueChanged (bool open);

public Q_SLOTS:

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event)override;

private:
    float m_size = 18;
    bool m_open = true;
};

/**
 * @brief
 *
 */
class StartStopButton : public QWidget
{
    Q_OBJECT
public:
    explicit StartStopButton(QWidget *parent = nullptr);

Q_SIGNALS:
    void startStopClicked (bool start);
protected:
    void paintEvent(QPaintEvent *) override;
    void mouseReleaseEvent(QMouseEvent *event)override;

private:
    float m_size = 12;
    bool m_start = true;
};

class CloseButton : public QWidget
{
    Q_OBJECT
public:
    explicit CloseButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseReleaseEvent(QMouseEvent *event)override;

Q_SIGNALS:

private:
    float m_size = 12;
};

#endif // FILEOPERATIONPROGRESS_H
