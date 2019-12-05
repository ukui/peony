#include "bookmark-manager.h"

#include <QtConcurrent>
#include <glib.h>

#include <QDebug>

using namespace Peony;

static BookMarkManager *global_instance = nullptr;

BookMarkManager *BookMarkManager::getInstance()
{
    if (!global_instance) {
        global_instance = new BookMarkManager;
    }
    return global_instance;
}

BookMarkManager::BookMarkManager(QObject *parent) : QObject(parent)
{
    QtConcurrent::run([=](){
        m_book_mark = new QSettings("org.ukui", "peony-qt");
        m_uris = m_book_mark->value("uris").toStringList();
        m_is_loaded = true;
        m_uris<<"computer:///";
        //qDebug()<<"====================ok============\n\n\n\n";
        Q_EMIT this->urisLoaded();
    });
}

BookMarkManager::~BookMarkManager()
{
    if (m_book_mark) {
        m_book_mark->deleteLater();
    }
}

void BookMarkManager::addBookMark(const QString &uri)
{
    QtConcurrent::run([=](){
        while (!this->isLoaded()) {
            g_usleep(100);
        }
        if (m_mutex.tryLock(1000)) {
            bool successed = !m_uris.contains(uri);
            if (successed) {
                m_uris<<m_uris;
                m_uris.removeDuplicates();
                m_book_mark->setValue("uris", m_uris);
                Q_EMIT this->bookMarkAdded(uri, true);
            } else {
                Q_EMIT this->bookMarkAdded(uri, false);
            }
            m_mutex.unlock();
        } else {
            Q_EMIT this->bookMarkAdded(uri, false);
        }
    });
}

void BookMarkManager::removeBookMark(const QString &uri)
{
    QtConcurrent::run([=](){
        while (!this->isLoaded()) {
            g_usleep(100);
        }
        if (m_mutex.tryLock(1000)) {
            bool successed = m_uris.contains(uri);
            if (successed) {
                m_uris.removeOne(uri);
                m_uris.removeDuplicates();
                m_book_mark->setValue("uris", m_uris);
                Q_EMIT this->bookMarkRemoved(uri, true);
            } else {
                Q_EMIT this->bookMarkRemoved(uri, false);
            }
            m_mutex.unlock();
        } else {
            Q_EMIT this->bookMarkRemoved(uri, false);
        }
    });
}
