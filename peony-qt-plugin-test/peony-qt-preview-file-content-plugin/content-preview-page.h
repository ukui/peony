#ifndef CONTENTPREVIEWPAGE_H
#define CONTENTPREVIEWPAGE_H

#include <QWidget>
#include "preview-page-plugin-iface.h"

class QLabel;

class ContentPreviewPage : public QWidget, public Peony::PreviewPageIface
{
    Q_OBJECT
public:
    explicit ContentPreviewPage(QWidget *parent = nullptr);
    ~ContentPreviewPage() override;

    void prepare(const QString &uri, PreviewType type) override {m_current_uri = uri; m_current_type = type;}
    void prepare(const QString &uri) override {m_current_uri = uri;}
    void startPreview() override;
    void cancel() override;
    void closePreviewPage() override {deleteLater();}

signals:

public slots:

private:
    QLabel *m_label = nullptr;

    QString m_current_uri = nullptr;
    Peony::PreviewPageIface::PreviewType m_current_type = Peony::PreviewPageIface::Other;
};

#endif // CONTENTPREVIEWPAGE_H
