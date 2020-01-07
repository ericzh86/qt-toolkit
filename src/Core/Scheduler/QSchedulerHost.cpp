#include "QSchedulerHost.h"
#include "QSchedulerHost_p.h"

#include <QDir>
#include <QUuid>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QFileInfo>

#include <QCoreApplication>
#include <QStringBuilder>
#include <QLoggingCategory>

#include "../FastPaths/QFastPaths.h"

#include "../Safeguard/QSafeguard.h"
#include "../LogWriter/QLogWriter.h"

Q_LOGGING_CATEGORY(lcSchedulerHost, "Ol.Scheduler")

// class QSchedulerHost

QSchedulerHost::QSchedulerHost()
    : d_ptr(new QSchedulerHostPrivate())
{
    d_ptr->q_ptr = this;

    Q_ASSERT(nullptr == QSchedulerHostPrivate::instance);
    QSchedulerHostPrivate::instance = this;

    // Initialize Host

    Q_D(QSchedulerHost);

    QString dataPath = qDataPath("SchedulerData", d->schedulerName);
    if (QDir().mkpath(dataPath)) {
        new QLogWriter(dataPath, this);
        qInfo(lcSchedulerHost, "init: %s",
              qUtf8Printable(dataPath));
    }

    d->quitTimer = new QTimer(this);
    d->quitTimer->setInterval(50);
    d->quitTimer->setSingleShot(false);
    connect(d->quitTimer, &QTimer::timeout,
            this, &QSchedulerHost::cleanTasks);

    for (int i = 0; i < 10; ++i) {
        quint32 id_ = qHash(QUuid::createUuid());
        QString id = QString("%1").arg(id_, 8, 16, QLatin1Char('0'));

        QString tmpId = d->schedulerName % '.' % id;
        if (QLocalServer::listen(tmpId)) {
            d->schedulerId = id;
            break;
        }
    }
    if (d->schedulerId.isEmpty()) {
        QTimer::singleShot(0, this, [] {
            QCoreApplication::quit();
        });
        return;
    }

    do {
        QSafeguard *safeguard = new QSafeguard(dataPath, this);
        for (int i = 0; i < 10; ++i) {
            quint32 id_ = qHash(QUuid::createUuid());
            QString id = QString("%1").arg(id_, 8, 16, QLatin1Char('0'));

            safeguard->setPipeName(d->schedulerName % '.' % id);
            if (safeguard->createServer()) {
                d->safeguard = safeguard;
                d->safeguardId = id;
                break;
            }
        }
    } while (false);
    if (d->safeguardId.isEmpty()) {
        d->safeguardId = QStringLiteral("00000000");
    }

    if ("00000000" != d->safeguardId) {
        auto safeguard = new QSafeguard(dataPath, this);
        safeguard->setPipeName(d->schedulerName % '.' % d->safeguardId);
        safeguard->createClient();
    }

    d->arguments = QCoreApplication::arguments().mid(1);

    int ii = d->arguments.indexOf("--developer-mode");
    if (ii >= 0) {
        d->isDeveloperMode = true;
        d->arguments.removeAt(ii);
    }
    if (!d->isDeveloperMode) {
        QString fileName = qAppPath("DeveloperMode.ini");
        d->isDeveloperMode = QFileInfo(fileName).exists();
    }

    QTimer::singleShot(0, QCoreApplication::instance(),
                       [this, d] { call("/", d->arguments); });
}

QSchedulerHost::~QSchedulerHost()
{
    QSchedulerHostPrivate::instance = nullptr;
}

QSchedulerHost *QSchedulerHost::instance()
{
    return QSchedulerHostPrivate::instance;
}

void QSchedulerHost::addProgramRoute(const QString &route, Functor functor)
{
    Q_D(QSchedulerHost);

    if ("/cleanup" == route) {
        qInfo(lcSchedulerHost, "\"/cleanup\" is keyword, use addCleanupRoute function.");
        return;
    }

    d->routeFunctors.insert(route, functor);
    d->modes.insert(route, Program);
}

void QSchedulerHost::addServiceRoute(const QString &route, Functor functor)
{
    Q_D(QSchedulerHost);

    if ("/cleanup" == route) {
        qInfo(lcSchedulerHost, "\"/cleanup\" is keyword, use addCleanupRoute function.");
        return;
    }

    d->routeFunctors.insert(route, functor);
    d->modes.insert(route, Service);
}

void QSchedulerHost::addCleanupRoute(Functor functor)
{
    Q_D(QSchedulerHost);

    d->routeFunctors.insert("/cleanup", functor);
    d->modes.insert("/cleanup", Program);
}

void QSchedulerHost::call(const QString &routeName, const QStringList &arguments)
{
    Q_D(QSchedulerHost);

    qInfo(lcSchedulerHost, "call: %s %s", qPrintable(routeName), qUtf8Printable(arguments.join(' ')));

    if (!isListening()) {
        return;
    } else if (d->isClosing) {
        return;
    }
    if (!d->modes.contains(routeName)) {
        qWarning(lcSchedulerHost, "missing: %s",
                 qPrintable(routeName));
        if ("/" == routeName) {
            QTimer::singleShot(0, this, [] {
                QCoreApplication::quit();
            });
        }
        return;
    }

    auto task = d->tasks.value(routeName);
    if (!task) {
        task = new QSchedulingTask(routeName, this);
        Mode mode = d->modes.value(routeName);
        if (Program == mode)
            task->setMode(QSchedulingTask::Program);
        else
            task->setMode(QSchedulingTask::Service);
        d->tasks.insert(routeName, task);

        connect(task, &QSchedulingTask::quitAll,
                this, [d] {
            if (!d->isClosing) {
                d->isClosing = true;
                d->quitTimer->start();

                qInfo(lcSchedulerHost, "clear tasks");
                for (auto task : d->tasks) {
                    task->clear();
                }
            }
        });
    }

    QStringList schedulerArguments = arguments;
    QString schedulingName = d->schedulerName % '.' % d->schedulerId % '.' % d->safeguardId % '.' % routeName;
    schedulerArguments << "--scheduling-name" << schedulingName;
    if (d->isDeveloperMode) {
        schedulerArguments << "--developer-mode";
    }

    task->call(schedulerArguments);
}

void QSchedulerHost::quit(const QString &routeName)
{
    Q_D(QSchedulerHost);

    if (d->isClosing) {
        return;
    }

    auto task = d->tasks.value(routeName);
    if(task) {
        qInfo(lcSchedulerHost, "quit: %s",
              qPrintable(routeName));

        task->quit();
    }
}

bool QSchedulerHost::isDeveloperMode() const
{
    Q_D(const QSchedulerHost);

    return d->isDeveloperMode;
}

void QSchedulerHost::incomingConnection(quintptr s)
{
    Q_D(QSchedulerHost);

    QScopedPointer<QSchedulingNode> node(new QSchedulingNode());
    if (!node->setSocketDescriptor(s)) {
        return;
    }

    qint16 routeSize = -1;
    for (int i = 0; i < 20; ++i) {
        if (node->waitForReadyRead(250)) {
            if (routeSize < 0) {
                if (node->bytesAvailable() < 2) {
                    continue;
                }
                if (!node->load(&routeSize, 2)) {
                    return;
                }
            }

            if (routeSize > 0) {
                if (node->bytesAvailable() < routeSize) {
                    continue;
                }
                QByteArray tmp(routeSize, 0);
                if (!node->load(tmp.data(), routeSize)) {
                    return;
                }

                connect(node.data(), QOverload<QLocalSocket::LocalSocketError>::of(&QSchedulingNode::error),
                        this, &QSchedulerHost::onError);
                connect(node.data(), &QSchedulingNode::stateChanged,
                        this, &QSchedulerHost::onStateChanged);

                connect(node.data(), &QSchedulingNode::callRequested,
                        this, &QSchedulerHost::call);
                connect(node.data(), &QSchedulingNode::quitRequested,
                        this, &QSchedulerHost::quit);

                connect(node.data(), &QSchedulingNode::readyRead,
                        node.data(), &QSchedulingNode::processDatagram);

                auto routeName = QString::fromUtf8(tmp);
                node->setRouteName(routeName);
                d->nodes.insert(routeName, node.take());

                // attach node
                if (auto task = d->tasks.value(routeName)) {
                    auto node = d->nodes.value(routeName);
                    task->attachNode(node);

                    node->processDatagram();
                }

                return;
            }
        }
    }
}

void QSchedulerHost::onError(QLocalSocket::LocalSocketError error)
{
    Q_UNUSED(error);

    Q_D(QSchedulerHost);

    auto node = qobject_cast<QSchedulingNode *>(QObject::sender());
    if (node && d->nodes.contains(node->routeName())) {
        d->nodes.remove(node->routeName());
        node->deleteLater();
    }
}

void QSchedulerHost::onStateChanged(QLocalSocket::LocalSocketState state)
{
    Q_D(QSchedulerHost);

    if (QLocalSocket::UnconnectedState == state) {
        auto node = qobject_cast<QSchedulingNode *>(QObject::sender());
        if (node && d->nodes.contains(node->routeName())) {
            d->nodes.remove(node->routeName());
            node->deleteLater();
        }
    }
}

void QSchedulerHost::cleanTasks()
{
    Q_D(QSchedulerHost);

    for (QSchedulingTask *task : d->tasks.values()) {
        if (task->isClosed()) {
            d->tasks.remove(task->route());
            qInfo(lcSchedulerHost, "cleaned: %s",
                  qPrintable(task->route()));
        }
    }

    if (d->tasks.isEmpty()) {
        d->quitTimer->stop();

        if (d->routeFunctors.contains("/cleanup")) {
            QString schedulingName = d->schedulerName % '.' % d->schedulerId % '.' % d->safeguardId % "./cleanup";

            QStringList schedulerArguments;
            schedulerArguments << "--scheduling-name" << schedulingName;
            QProcess::startDetached(QCoreApplication::applicationFilePath(), schedulerArguments);
        }

        QLogWriter::writeGracefulEnding(0);
        QCoreApplication::quit();
    }
}

// class QSchedulerHostPrivate

QSchedulerHost *QSchedulerHostPrivate::instance = nullptr;

QSchedulerHostPrivate::QSchedulerHostPrivate()
    : q_ptr(nullptr)
    , isClosing(false)
    , quitTimer(nullptr)
    , safeguard(nullptr)
    , isDeveloperMode(false)
{
    QDateTime now = QDateTime::currentDateTime();
    schedulerName = now.toString("yyMMdd.hhmmss.zzz");
}

QSchedulerHostPrivate::~QSchedulerHostPrivate()
{
}
