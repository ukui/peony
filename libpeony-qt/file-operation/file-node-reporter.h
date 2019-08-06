#ifndef FILENODEREPORTER_H
#define FILENODEREPORTER_H

#include <QObject>
#include <memory>

namespace Peony {

class FileNode;

/*!
 * \brief The FileNodeReporter class
 * <br>
 * This class is a signal proxy of FileNode instances.
 * Other object can connect the signals getting the current state of filenode.
 * </br>
 */
class FileNodeReporter : public QObject
{
    Q_OBJECT
public:
    explicit FileNodeReporter(QObject *parent = nullptr);
    ~FileNodeReporter();

    void sendNodeFound(const QString &uri, const quint64 &offset) {
        Q_EMIT nodeFound(uri, offset);
    }

Q_SIGNALS:
    void nodeFound(const QString &uri, const quint64 &offset);
    void enumerateNodeFinished();
};

}

#endif // FILENODEREPORTER_H
