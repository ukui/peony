#ifndef FILECOUNTOPERATION_H
#define FILECOUNTOPERATION_H

#include "file-operation.h"

namespace Peony {

class FileNodeReporter;

class FileCountOperation : public FileOperation
{
    Q_OBJECT
public:
    explicit FileCountOperation(const QStringList &uris, bool countRoot = true, QObject *parent = nullptr);
    ~FileCountOperation() override;
    void run() override;
    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return nullptr;}
    void getInfo(quint64 &file_count, quint64 &hidden_file_count, quint64 &total_size) {
        file_count = m_file_count;
        hidden_file_count = m_hidden_file_count;
        total_size = m_total_size;
    }

Q_SIGNALS:
    void countDone(quint64 file_count, quint64 hidden_file_count, quint64 total_size);

public Q_SLOTS:
    void cancel() override;

private:
    FileNodeReporter *m_reporter = nullptr;
    QStringList m_uris;

    quint64 m_file_count = 0;
    quint64 m_hidden_file_count = 0;
    quint64 m_total_size = 0;

    bool m_count_root = true;
};

}

#endif // FILECOUNTOPERATION_H
