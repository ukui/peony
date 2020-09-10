/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: Jing Ding <dingjing@kylinos.cn>
 *
 */

#ifndef FILEOPERATIONPROGRESS_H
#define FILEOPERATIONPROGRESS_H
#include <QWidget>
#include <QHBoxLayout>
#include <QListWidget>

class ProgressBar;
class OtherButton;
class MainProgressBar;

class FileOperationProgressBar : public QWidget
{
    Q_OBJECT
public:
    static FileOperationProgressBar* getInstance();

    void removeAllProgressbar ();
    ProgressBar* addFileOperation();
    void showProgress (ProgressBar& progress);
    void removeFileOperation(ProgressBar* progress);

private:
    explicit FileOperationProgressBar(QWidget *parent = nullptr);
    ~FileOperationProgressBar();
    void showMore ();

protected:
    void showWidgetList(bool show);
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void canceled();

public Q_SLOTS:
    void mainProgressChange(QListWidgetItem *item);
    void showDelay(int msec = 2000);

private:
    // layout
    QVBoxLayout* m_main_layout = nullptr;

    // widget
    QListWidget* m_list_widget = nullptr;
    ProgressBar* m_current_main = nullptr;
    OtherButton* m_other_progressbar = nullptr;
    MainProgressBar* m_main_progressbar = nullptr;

    QMap<QListWidgetItem*, ProgressBar*>* m_widget_list = nullptr;
    QMap<ProgressBar*, QListWidgetItem*>* m_progress_list = nullptr;

    int m_show_items = 2;
    bool m_show_more = false;
    int m_progress_item_height = 62;
    int m_progress_list_heigth = 200;

    // ui
    QPoint m_position;
    int m_progress_size = 0;
    bool m_is_press = false;

    bool m_more = false;
    static FileOperationProgressBar* instance;
};

class ProgressBar : public QWidget
{
    friend FileOperationProgressBar;
    Q_OBJECT
public:
    explicit ProgressBar (QWidget* parent = nullptr);
    void setIcon (QIcon icon);
    QIcon getIcon();
    bool getStatus();

private:
    ~ProgressBar();

Q_SIGNALS:
    void cancelled();
    void finished(ProgressBar* fop);
    void sendValue(QString&, double);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public Q_SLOTS:
    void onCancelled();
    void updateValue(double);
    void onElementFoundOne (const QString &uri, const qint64 &size);
    void onElementFoundAll ();
    void onFileOperationProgressedOne(const QString &uri, const QString &destUri, const qint64 &size);
    void updateProgress(const QString &srcUri, const QString &destUri, quint64 current, quint64 total);
    void onFileOperationProgressedAll();
    void onElementClearOne(const QString &uri);
    void switchToRollbackPage();
    void onStartSync();
    void onFinished();
    void onFileRollbacked(const QString &destUri, const QString &srcUri);

private:
    int m_min_width = 400;
    int m_fix_height = 62;

    int m_btn_size = 18;
    int m_margin_ud = 2;
    int m_margin_lr = 8;
    int m_icon_size = 32;
    int m_text_height = 20;

    int m_progress_width = 80;
    int m_progress_height = 6;

    int m_percent_width = 20;

    // value
    QIcon m_icon;
    double m_current_value = 0.0;

    QString m_src_uri;
    QString m_dest_uri;
    int m_total_count = 0;
    int m_current_count = 1;
    quint64 m_total_size = 0;
    qint32 m_current_size = 0;

    bool m_is_stopping = false;
};

class MainProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit MainProgressBar(QWidget *parent = nullptr);
    void initPrarm();
    void setFileIcon (QIcon icon);
    void setTitle (QString title);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void paintFoot (QPainter& painter);
    void paintHeader (QPainter& painter);
    void paintContent (QPainter& painter);
    void paintProgress (QPainter& painter);

Q_SIGNALS:
    void minimized();
    void closeWindow();

public Q_SLOTS:
    void cancelld();
    void updateValue (QString&, double);

private:
    // header
    QString m_title;
    int m_fix_width = 550;
    int m_fix_height = 200;
    int m_title_width = 480;
    int m_header_height = 30;

    // btn
    int m_btn_margin_top = 8;
    int m_btn_margin = 10;
    int m_btn_size = 16;

    // icon
    int m_icon_margin = 20;
    int m_icon_size = 64;

    // file name
    int m_file_name_height = 20;
    int m_file_name_margin = 10;

    // percent
    int m_percent_margin = 15;
    int m_percent_height = 50;

    // foot
    float m_foot_margin = 3;

    // progress
    bool m_show = false;
    float m_move_x = 0.5;
    bool m_stopping = false;
    float m_current_value = 0.0;
    QString m_file_name = tr("starting ...");
    QIcon m_icon = QIcon::fromTheme("window-close-symbolic");
};

class OtherButton : public QWidget
{
    Q_OBJECT
public:
    explicit OtherButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void clicked (bool show);

private:
    // button height
    int m_icon_margin = 6;
    int m_icon_size = 10;
    int m_button_heigth = 24;

    // text width
    int m_text_length = 100;
    QString m_text = tr("Other queue");

    // show
    bool m_show = false;

    bool m_is_press = true;
};

#endif // FILEOPERATIONPROGRESS_H
