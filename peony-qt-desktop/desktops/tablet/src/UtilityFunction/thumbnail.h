#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "src/Interface/ukuimenuinterface.h"
#define TEXT_LENGTH 100       //文字长度

class ThumbNail : public QWidget
{
    Q_OBJECT
public:
    ThumbNail(QWidget *parent = nullptr);
    ~ThumbNail();
    void setupthumbnail(QString desktopfp);
    QPixmap getIcon(QString desktopfp,int width,int height);
    bool appDisable(QString desktopfp);

private:
    QLabel *iconLabel=nullptr;
    QVBoxLayout *layout=nullptr;
    QLabel *m_icon=nullptr;
    QLabel *textLabel = nullptr;
    UkuiMenuInterface *pUkuiMenuInterface = nullptr;
    QString appIcon=nullptr;
    QSettings *setting=nullptr;
    QSettings *disableSetting=nullptr;

private:
    void initUi();
};

#endif // THUMBNAIL_H
