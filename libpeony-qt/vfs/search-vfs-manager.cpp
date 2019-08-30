#include "search-vfs-manager.h"

using namespace Peony;

static SearchVFSManager* global_manager = nullptr;

SearchVFSManager *SearchVFSManager::getInstance()
{
    if (!global_manager) {
        global_manager = new SearchVFSManager;
    }
    return global_manager;
}

SearchVFSManager::SearchVFSManager(QObject *parent) : QObject(parent)
{

}

SearchVFSManager::~SearchVFSManager()
{
    m_search_dir_results_hash.clear();
}

void SearchVFSManager::clearHistory()
{
    m_mutex.lock();
    m_search_dir_results_hash.clear();
    m_mutex.unlock();
}

void SearchVFSManager::clearHistoryOne(const QString &searchUri)
{
    m_mutex.lock();
    m_search_dir_results_hash.remove(searchUri);
    m_mutex.unlock();
}

bool SearchVFSManager::hasHistory(const QString &searchUri)
{
    return m_search_dir_results_hash.contains(searchUri);
}

void SearchVFSManager::addHistory(const QString &searchUri, const QStringList &results)
{
    m_mutex.lock();
    m_search_dir_results_hash.insert(searchUri, results);
    m_mutex.unlock();
}

QStringList SearchVFSManager::getHistroyResults(const QString &searchUri)
{
    return m_search_dir_results_hash.value(searchUri);
}
