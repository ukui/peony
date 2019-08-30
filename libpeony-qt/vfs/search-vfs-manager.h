#ifndef SEARCHVFSMANAGER_H
#define SEARCHVFSMANAGER_H

#include <QObject>
#include <QHash>
#include <QMutex>

namespace Peony {

class SearchVFSManager : public QObject
{
    Q_OBJECT
public:
    static SearchVFSManager *getInstance();

public Q_SLOTS:
    void clearHistory();
    /*!
     * \brief clearHistoryOne
     * \param searchUri
     * \details
     * if we refresh the directory, we should clean the history of the
     * directory and search again.
     */
    void clearHistoryOne(const QString &searchUri);
    void addHistory(const QString &searchUri, const QStringList &results);
    bool hasHistory(const QString &serachUri);
    QStringList getHistroyResults(const QString &searchUri) ;

private:
    explicit SearchVFSManager(QObject *parent = nullptr);
    ~SearchVFSManager();

    QMutex m_mutex;
    QHash<QString, QStringList> m_search_dir_results_hash;
};

}

#endif // SEARCHVFSMANAGER_H
