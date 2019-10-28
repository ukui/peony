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

class PeonyApplication : public SingleApplication
{
    Q_OBJECT
public:
    PeonyApplication(int &argc, char *argv[], const char *applicationName = "peony-qt");
};

#endif // PEONYAPPLICATION_H
