#include "QSafeguard.h"
#include "QSafeguard_p.h"

#include <QStringBuilder>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcSafeguard, "QSafeguard")

// class QSafeguard

QSafeguard::QSafeguard(const QString &dumpPath, QObject *parent)
    : QObject(parent)
    , d_ptr(new QSafeguardPrivate())
{
    d_ptr->q_ptr = this;

    d_ptr->dumpPath = dumpPath;
}

QSafeguard::QSafeguard(QObject *parent)
    : QObject(parent)
    , d_ptr(new QSafeguardPrivate())
{
    d_ptr->q_ptr = this;
}

QSafeguard::~QSafeguard()
{
}

void QSafeguard::setDumpPath(const QString &path)
{
    Q_D(QSafeguard);

    d->dumpPath = path;
}

void QSafeguard::setPipeName(const QString &name)
{
    Q_D(QSafeguard);

    d->pipeName = name;
}

const QString &QSafeguard::dumpPath() const
{
    Q_D(const QSafeguard);

    return d->dumpPath;
}

const QString &QSafeguard::pipeName() const
{
    Q_D(const QSafeguard);

    return d->pipeName;
}

bool QSafeguard::createServer()
{
    Q_D(QSafeguard);

    Q_ASSERT(!d->dumpPath.isEmpty());
    Q_ASSERT(!d->pipeName.isEmpty());

#if defined(Q_OS_WIN32)
    QString pipeName = QString::fromLatin1("\\\\.\\pipe\\") % d->pipeName;
    QSharedPointer<google_breakpad::CrashGenerationServer> crashServer(new google_breakpad::CrashGenerationServer(
                                                                           pipeName.toStdWString(),
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           true,
                                                                           &d->dumpPath.toStdWString()
                                                                       ));

    if (!crashServer->Start()) {
        qWarning(lcSafeguard, "crash server start failed.");
        return false;
    }

    qInfo(lcSafeguard,  "crash server ready...");
    d->crashServer = crashServer;
    return true;
#else
    /*
    QString pipeName = QString::fromLatin1("\\\\.\\pipe\\") % d->pipeName;
    QSharedPointer<google_breakpad::CrashGenerationServer> crashServer(new google_breakpad::CrashGenerationServer(
                                                                           pipeName.toStdString().c_str(),
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           nullptr,
                                                                           true,
                                                                           d->dumpPath.toStdString()));
                                                                           */
#endif

    /*
    if (!crashServer->Start()) {
        qWarning(lcSafeguard, "crash server start failed.");
        return false;
    }

    qInfo(lcSafeguard,  "crash server ready...");
    d->crashServer = crashServer;
    */
    return false;
}

void QSafeguard::createClient()
{
    Q_D(QSafeguard);

    Q_ASSERT(!d->dumpPath.isEmpty());
    Q_ASSERT(!d->pipeName.isEmpty());

#if defined(Q_OS_WIN32)
    QString pipeName = QString::fromLatin1("\\\\.\\pipe\\") % d->pipeName;
    d->exceptionHandler.reset(new google_breakpad::ExceptionHandler(d->dumpPath.toStdWString(),
                                                                    nullptr,
                                                                    nullptr,
                                                                    nullptr,
                                                                    google_breakpad::ExceptionHandler::HANDLER_ALL,
                                                                    MiniDumpNormal,
                                                                    pipeName.toStdWString().c_str(),
                                                                    nullptr));

    if (d->exceptionHandler->IsOutOfProcess()) {
        qInfo(lcSafeguard, "daemon mode.");
    } else {
        qInfo(lcSafeguard, "normal mode.");
    }
#else
    /*
    QString pipeName = QString::fromLatin1("\\\\.\\pipe\\") % d->pipeName;
    d->exceptionHandler.reset(new google_breakpad::ExceptionHandler(d->dumpPath.toStdString(),
                                                                    nullptr,
                                                                    nullptr,
                                                                    nullptr,
                                                                    true,
                                                                    pipeName.toStdString().c_str()));
                                                                    */
#endif

    /*
    if (d->exceptionHandler->IsOutOfProcess()) {
        qInfo(lcSafeguard, "daemon mode.");
    } else {
        qInfo(lcSafeguard, "normal mode.");
    }
    */
}

void QSafeguard::makeSnapshot()
{
#if defined(Q_OS_WIN32)
    Q_D(QSafeguard);

    if (d->exceptionHandler) {
        d->exceptionHandler->WriteMinidump();
    }
#endif
}

// class QSafeguardPrivate

QSafeguardPrivate::QSafeguardPrivate()
    : q_ptr(nullptr)
{
}

QSafeguardPrivate::~QSafeguardPrivate()
{
}
