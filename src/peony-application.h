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

#include <QApplication>

class PeonyApplication : public QApplication
{
    Q_OBJECT
public:
    PeonyApplication(int argc, char *argv[]);
};

#endif // PEONYAPPLICATION_H
