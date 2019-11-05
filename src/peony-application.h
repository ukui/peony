#ifndef PEONYAPPLICATION_H
#define PEONYAPPLICATION_H

/**
 * @brief A single application use client/server desgin.
 * @details single application with c/s conmunication mode is a common desgin used for many file manager, like peony, caja, and nautilus.
 * <pre>
 * There are several benefits to this type application:
 *  * Reduce wastes of resources
 *  * Make application(s) easy to manage
 *  * Reduce IPC
 * </pre>
 *
 */

#include "singleapplication.h"
#include <QCommandLineParser>
#include <QCommandLineOption>

class PeonyApplication : public SingleApplication
{
    Q_OBJECT
public:
    explicit PeonyApplication(int &argc, char *argv[], const char *applicationName = "peony-qt");

protected Q_SLOTS:
    void parseCmd(quint32 id, QByteArray msg);

private:
    QCommandLineParser parser;
    QCommandLineOption quitOption = QCommandLineOption(QStringList()<<"q"<<"quit", tr("Close all peony-qt windows and quit"));
    QCommandLineOption showItemsOption = QCommandLineOption(QStringList()<<"i"<<"show-items", tr("Show items"));
    QCommandLineOption showFoldersOption = QCommandLineOption(QStringList()<<"f"<<"show-folders", tr("Show folders"));
    QCommandLineOption showPropertiesOption = QCommandLineOption(QStringList()<<"p"<<"show-properties", tr("Show properties"));

    bool m_first_parse = true;
};

#endif // PEONYAPPLICATION_H
