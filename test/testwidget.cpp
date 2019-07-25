#include "testwidget.h"

#include "file-info.h"
#include "file-info-job.h"

#include "file-info-manager.h"

#include "file-enumerator.h"

#include "mount-operation.h"

#include "file-watcher.h"

#include "volume-manager.h"

#include <QDebug>

TestWidget::TestWidget(QWidget *parent) : QWidget(parent)
{
    auto info = Peony::FileInfo::fromUri("file:///");
    qDebug()<<info->uri();
    //get uri of info.
    QString uri = info->uri();

    connect(info.get(), &Peony::FileInfo::updated, [=](){
        qDebug()<<"this info was updated";
        //qDebug()<<info->iconName(); //this is uncorrect. be caleful that using info shared_ptr in lambda also causes ref count increased.
        auto file_info = Peony::FileInfoManager::getInstance()->findFileInfoByUri(uri); //this is correct.
        qDebug()<<file_info->iconName();
    });

    Peony::FileInfoJob *job = new Peony::FileInfoJob(info, nullptr);
    job->setAutoDelete(true);
    job->querySync();
    qDebug()<<info->iconName();

    for (int i = 0; i < 4; i++) {
        Peony::FileInfoJob *job2 = new Peony::FileInfoJob(info, nullptr);
        job2->setAutoDelete(true);
        job2->connect(job2, &Peony::FileInfoJob::queryAsyncFinished, [=](bool successed){
            qDebug()<<successed;
        });
        job2->queryAsync();
        //job2->cancel();//if we canceled the job, file info will not be modified send updated signal.
        qDebug()<<"queryAsync";
    }
    info.reset();

    //do not use shared_ptr with lambda.
    //auto enumerator = std::make_shared<Peony::FileEnumerator>();
    auto enumerator = new Peony::FileEnumerator;
    //enumerator->setEnumerateDirectory("computer:///KINGSTON%20RBU-SNS8152S3256GG5.drive");
    //enumerator->setEnumerateDirectory("sftp://112.124.201.32");
    //enumerator->setEnumerateDirectory("smb://lacie-5big.local/share/");
    //enumerator->setEnumerateDirectory("network:///");
    //enumerator->setEnumerateDirectory("network:///dnssd-domain-MC._smb._tcp");
    //enumerator->setEnumerateDirectory("network:///dnssd-domain-LaCie-5big._smb._tcp");
    //enumerator->setEnumerateDirectory("file:///home/lanyue");
    //enumerator->setEnumerateDirectory("file:///root");
    enumerator->setEnumerateDirectory("file:///");
    enumerator->prepare();
    connect(enumerator/*.get()*/, &Peony::FileEnumerator::prepared, [=](GError *prepared_err){
        qDebug()<<"prepared";
        if (prepared_err) {
            qDebug()<<prepared_err->code<<prepared_err->message;
        }

        //sync enumerate
        /*
        enumerator->enumerateSync();
        auto list = enumerator->getChildren();
        for (auto child : list) {
            qDebug()<<child->uri()<<child.use_count();
        }
        list.clear();
        enumerator->disconnect();
        delete enumerator;
        */

        //async enumerate
        this->connect(enumerator, &Peony::FileEnumerator::childrenUpdated, [=](const QStringList &uriList){
            if (uriList.isEmpty())
                return;
            qDebug()<<"update async:"<<uriList;
        });
        this->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, [=](bool successed){
            if (!successed) {
                qDebug()<<"failed enumerate children";
                return;
            }
            auto list = enumerator->getChildren();
            for (auto child : list) {
                qDebug()<<child->uri()<<child.use_count();
            }
            list.clear();
            enumerator->disconnect();
            delete enumerator;
        });
        enumerator->enumerateAsync();
    });

    //Peony::FileWatcher *watcher = new Peony::FileWatcher("file:///home/lanyue/gvfs-test");
    Peony::FileWatcher *watcher = new Peony::FileWatcher("computer:///");
    connect(watcher, &Peony::FileWatcher::locationChanged, [=](QString old, QString newly){
        qDebug()<<"monitor location changed"<<old<<"to"<<newly;
    });
    connect(watcher, &Peony::FileWatcher::directoryDeleted, [=](QString uri){
        qDebug()<<uri<<"was deleted";
        //the watcher instance will be deleted automaticly later.
    });
    connect(watcher, &Peony::FileWatcher::fileCreated, [=](QString uri){
        qDebug()<<uri<<"was created";
    });
    connect(watcher, &Peony::FileWatcher::fileDeleted, [=](QString uri){
        qDebug()<<uri<<"was deleted";
    });
    watcher->startMonitor();

    Peony::VolumeManager *volumeManager = Peony::VolumeManager::getInstance();

    connect(volumeManager, &Peony::VolumeManager::driveConnected, [=](std::shared_ptr<Peony::Drive> drive){
        auto name = drive->name();
        qDebug()<<"drive conneceted"<<name;
    });
    connect(volumeManager, &Peony::VolumeManager::driveDisconnected, [=](std::shared_ptr<Peony::Drive> drive){
        auto name = drive->name();
        qDebug()<<"drive disconneceted"<<name;
    });
    connect(volumeManager, &Peony::VolumeManager::volumeAdded, [=](std::shared_ptr<Peony::Volume> volume){
        auto name = volume->name();
        qDebug()<<"volume added"<<name;
    });
    connect(volumeManager, &Peony::VolumeManager::volumeRemoved, [=](std::shared_ptr<Peony::Volume> volume){
        auto name = volume->name();
        qDebug()<<"volume removed"<<name;
    });
    connect(volumeManager, &Peony::VolumeManager::mountAdded, [=](std::shared_ptr<Peony::Mount> mount){
        auto name = mount->name();
        qDebug()<<"mount added"<<name;
    });
    connect(volumeManager, &Peony::VolumeManager::mountRemoved, [=](std::shared_ptr<Peony::Mount> mount){
        auto name = mount->name();
        qDebug()<<"mount removed"<<name;
    });

}
