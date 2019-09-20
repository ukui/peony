#ifndef PATHEDIT_H
#define PATHEDIT_H

#include <QLineEdit>
#include "peony-core_global.h"

namespace Peony {

class PathBarModel;
class PathCompleter;

class PEONYCORESHARED_EXPORT PathEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit PathEdit(QWidget *parent = nullptr);

Q_SIGNALS:
    void uriChangeRequest(const QString &uri);
    void editCancelled();

public Q_SLOTS:
    void setUri(const QString &uri);

protected:
    void focusOutEvent(QFocusEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    QString m_last_uri;

    PathBarModel *m_model;
    PathCompleter *m_completer;
};

}

#endif // PATHEDIT_H
