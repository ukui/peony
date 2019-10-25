#ifndef FILECOUNTOPERATION_H
#define FILECOUNTOPERATION_H

#include "file-operation.h"

namespace Peony {

class FileNodeReporter;

class FileCountOperation : public FileOperation
{
public:
    explicit FileCountOperation(const QStringList &uris, QObject *parent = nullptr);
    ~FileCountOperation() override;
    void run() override;
    std::shared_ptr<FileOperationInfo> getOperationInfo() override {return nullptr;}

public Q_SLOTS:
    void cancel() override;

private:
    FileNodeReporter *m_reporter = nullptr;
    QStringList m_uris;
};

}

#endif // FILECOUNTOPERATION_H
