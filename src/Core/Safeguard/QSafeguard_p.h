#ifndef QSAFEGUARD_P_H
#define QSAFEGUARD_P_H

#include <QSharedPointer>

#include "QSafeguard.h"

#if defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#include "client/windows/crash_generation/crash_generation_server.h"
#include "client/windows/crash_generation/crash_generation_client.h"
#else
// #include "client/mac/handler/exception_handler.h"
// #include "client/mac/crash_generation/crash_generation_server.h"
// #include "client/mac/crash_generation/crash_generation_client.h"
#endif

class QSafeguardPrivate
{
    Q_DECLARE_PUBLIC(QSafeguard)

public:
    QSafeguardPrivate();
    virtual ~QSafeguardPrivate();
protected:
    QSafeguard *q_ptr;

protected:
    QString dumpPath;
    QString pipeName;

protected:
#if defined(Q_OS_WIN32)
    QSharedPointer<google_breakpad::CrashGenerationServer> crashServer;
    QSharedPointer<google_breakpad::ExceptionHandler> exceptionHandler;
#endif
};

#endif // QSAFEGUARD_P_H
