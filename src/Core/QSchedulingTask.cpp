#include "QSchedulingTask.h"
#include "QSchedulingTask_p.h"

#include <QDataStream>

#include <QCoreApplication>
#include <QTimer>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcSchedulingTask, "QScheduler")

// class QSchedulingTask

QSchedulingTask::QSchedulingTask(const QString &route, QObject *parent)
    : QObject(parent)
    , d_ptr(new QSchedulingTaskPrivate(route))
{
    d_ptr->q_ptr = this;

    Q_D(QSchedulingTask);

    d->callTimer = new QTimer(this);
    d->callTimer->setSingleShot(true);
    d->callTimer->setInterval(50);
    d->stopTimer = new QTimer(this);
    d->stopTimer->setInterval(50);

    connect(d->callTimer, &QTimer::timeout,
            this, QOverload<>::of(&QSchedulingTask::call));
    connect(d->stopTimer, &QTimer::timeout,
            this, &QSchedulingTask::quit);
}

QSchedulingTask::~QSchedulingTask()
{
}

const QString &QSchedulingTask::route() const
{
    Q_D(const QSchedulingTask);

    return d->route;
}

void QSchedulingTask::setMode(QSchedulingTask::Mode mode)
{
    Q_D(QSchedulingTask);

    d->mode = mode;

    if (Program == mode) {
        qInfo(lcSchedulingTask, "program: %s", qPrintable(d->route));
    } else {
        qInfo(lcSchedulingTask, "service: %s", qPrintable(d->route));
    }
}

QSchedulingTask::Mode QSchedulingTask::mode() const
{
    Q_D(const QSchedulingTask);

    return d->mode;
}

void QSchedulingTask::call()
{
    Q_D(QSchedulingTask);

    if (d->isDirty) {
        return;
    }

    if (d->retries <= 0) {
        return;
    } else if (d->process) {
        if (d->node) {
            QByteArray datagram;
            QDataStream ds(&datagram, QIODevice::WriteOnly);
            ds << d->arguments;
            d->node->writeDatagram(5, datagram);
        }
        return;
    }
    --d->retries;

    QString program = QCoreApplication::applicationFilePath();

    d->process = new QProcess(this);
    d->process->setProgram(program);
    d->process->setArguments(d->arguments);

    connect(d->process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred),
            this, &QSchedulingTask::onError);
    connect(d->process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &QSchedulingTask::onFinished);

    d->isQuitAll = true;
    d->process->start();
}

void QSchedulingTask::call(const QStringList &arguments)
{
    Q_D(QSchedulingTask);

    if (d->isDirty) {
        return;
    }

    if (Program == d->mode) {
        d->retries = 1;
    } else {
        d->retries = 5;
    }

    d->arguments = arguments;
    QSchedulingTask::call();
}

void QSchedulingTask::quit()
{
    Q_D(QSchedulingTask);

    d->retries = 0;

    if (d->process && (QProcess::NotRunning != d->process->state())) {
        d->isQuitAll = false;
        if (d->stopTimer->isActive()) {
            QDateTime now = QDateTime::currentDateTime();
            if (d->deadline <= now) {
                qInfo(lcSchedulingTask, "kill: %s",
                      qPrintable(d->route));

                d->isQuitAll = false;
                d->process->kill();
                d->stopTimer->stop();
            } else {
                qInfo(lcSchedulingTask, "wait: %s",
                      qPrintable(d->route));

                if (d->node)
                    d->node->quit();
            }
        } else {
            qInfo(lcSchedulingTask, "wait: %s",
                  qPrintable(d->route));

            if (d->node)
                d->node->quit();

            QDateTime now = QDateTime::currentDateTime();
            d->deadline = now.addMSecs(7500);
            d->stopTimer->start();
        }
    } else {
        qInfo(lcSchedulingTask, "skip: %s",
              qPrintable(d->route));
    }
}

void QSchedulingTask::attachNode(QSchedulingNode *node)
{
    Q_D(QSchedulingTask);

    qInfo(lcSchedulingTask, "connect: %s",
          qPrintable(d->route));
    d->node = node;
}

QSchedulingNode *QSchedulingTask::node() const
{
    Q_D(const QSchedulingTask);

    return d->node;
}

bool QSchedulingTask::isClosed() const
{
    Q_D(const QSchedulingTask);

    return d->process.isNull();
}

void QSchedulingTask::clear()
{
    Q_D(QSchedulingTask);

    if (d->isDirty) {
        return;
    }

    d->isDirty = true;

    quit();
}

void QSchedulingTask::onError(QProcess::ProcessError error)
{
    Q_D(QSchedulingTask);

    if (d->process) {
        QStringList arguments = d->process->arguments();
        int ii = arguments.indexOf("--scheduling-name");
        if (ii >= 0)
            arguments = arguments.mid(0, ii);

        QByteArray reason;
        switch (error) {
        case QProcess::FailedToStart:
            reason = "FailedToStart";
            break;
        case QProcess::Crashed:
            reason = "Crashed";
            break;
        case QProcess::Timedout:
            reason = "Timedout";
            break;
        case QProcess::ReadError:
            reason = "ReadError";
            break;
        case QProcess::WriteError:
            reason = "WriteError";
            break;
        case QProcess::UnknownError:
            reason = "UnknownError";
            break;
        }

        if (arguments.isEmpty()) {
            qCritical(lcSchedulingTask, "error(%s): %s",
                      reason.constData(), qPrintable(d->route));
        } else {
            qCritical(lcSchedulingTask, "error(%s): %s %s", reason.constData(),
                      qPrintable(d->route), qUtf8Printable(arguments.join(' ')));
        }

        d->stopTimer->stop();
        d->process->deleteLater();
        d->process = nullptr;

        if (d->retries > 0) {
            if (!d->isDirty && !d->callTimer->isActive()) {
                if (arguments.isEmpty()) {
                    qInfo(lcSchedulingTask, "restart: %s", qPrintable(d->route));
                } else {
                    qInfo(lcSchedulingTask, "restart: %s %s", qPrintable(d->route),
                          qUtf8Printable(arguments.join(' ')));
                }

                d->callTimer->start();
            }
        } else if ("/" == d->route) {
            emit quitAll();
        } else if (Service == d->mode) {
            if (d->isQuitAll) {
                emit quitAll();
            }
        }
    }
}

void QSchedulingTask::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_D(QSchedulingTask);

    if (d->process) {
        QStringList arguments = d->process->arguments();
        int ii = arguments.indexOf("--scheduling-name");
        if (ii >= 0)
            arguments = arguments.mid(0, ii);

        switch (exitStatus) {
        case QProcess::NormalExit:
            if (arguments.isEmpty()) {
                qInfo(lcSchedulingTask, "exit(Normal %d): %s", exitCode, qPrintable(d->route));
            } else {
                qInfo(lcSchedulingTask, "exit(Normal %d): %s %s", exitCode,
                      qPrintable(d->route), qUtf8Printable(arguments.join(' ')));
            }
            break;
        case QProcess::CrashExit:
            if (arguments.isEmpty()) {
                qCritical(lcSchedulingTask, "exit(Crash %d): %s", exitCode, qPrintable(d->route));
            } else {
                qCritical(lcSchedulingTask, "exit(Crash %d): %s %s", exitCode,
                          qPrintable(d->route), qUtf8Printable(arguments.join(' ')));
            }
            break;
        }

        d->stopTimer->stop();
        d->process->deleteLater();
        d->process = nullptr;

        if (d->retries > 0) {
            if (!d->isDirty && !d->callTimer->isActive()) {
                if (arguments.isEmpty()) {
                    qInfo(lcSchedulingTask, "restart: %s", qPrintable(d->route));
                } else {
                    qInfo(lcSchedulingTask, "restart: %s %s", qPrintable(d->route),
                          qUtf8Printable(arguments.join(' ')));
                }

                d->callTimer->start();
            }
        } else if ("/" == d->route) {
            emit quitAll();
        } else if (Service == d->mode) {
            if (d->isQuitAll) {
                emit quitAll();
            }
        }
    }
}

// class QSchedulingTaskPrivate

QSchedulingTaskPrivate::QSchedulingTaskPrivate(const QString &route)
    : q_ptr(nullptr)
    , route(route)
    , retries(0)
    , mode(QSchedulingTask::Program)
    , callTimer(nullptr)
    , stopTimer(nullptr)
    , isQuitAll(false)
    , isDirty(false)
{
}

QSchedulingTaskPrivate::~QSchedulingTaskPrivate()
{
}
