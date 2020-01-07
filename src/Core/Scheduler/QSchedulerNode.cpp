#include "QSchedulerNode.h"
#include "QSchedulerNode_p.h"

#include <QTimer>
#include <QThread>
#include <QFileInfo>
#include <QDataStream>
#include <QDateTime>

#include <QCoreApplication>
#include <QStringBuilder>
#include <QLoggingCategory>

#include "../FastPaths/QFastPaths.h"

#include "../Safeguard/QSafeguard.h"
#include "../LogWriter/QLogWriter.h"

Q_LOGGING_CATEGORY(lcSchedulerNode, "Ol.Scheduler")

// class QSchedulerNode

QSchedulerNode::QSchedulerNode()
    : d_ptr(new QSchedulerNodePrivate())
{
    d_ptr->q_ptr = this;

    Q_ASSERT(nullptr == QSchedulerNodePrivate::instance);
    QSchedulerNodePrivate::instance = this;

    // Initialize Node

    Q_D(QSchedulerNode);

    connect(this, QOverload<QLocalSocket::LocalSocketError>::of(&QSchedulerNode::error),
            this, &QSchedulerNode::onError);
    connect(this, &QSchedulerNode::stateChanged,
            this, &QSchedulerNode::onStateChanged);

    QCoreApplication *app = QCoreApplication::instance();

    int i = 0;
    QString schedulingName;
    // qInfo("%s", app->arguments().join(' '));
    QStringList arguments = app->arguments().mid(1);
    for (const QString &argument : arguments) {
        if (argument.compare("--scheduling-name") == 0) {
            schedulingName = arguments.value(i + 1);
            if (!schedulingName.isEmpty()) {
                d->arguments = arguments.mid(0,  i);
                break;
            }

            QTimer::singleShot(0, this, [] {
                QCoreApplication::quit();
            });

            return;
        }
        ++i;
    }

    d->schedulerName = schedulingName.mid(0, 17);
    d->schedulerId = schedulingName.mid(18, 8);
    d->safeguardId = schedulingName.mid(27, 8);
    d->routeName = schedulingName.mid(36,-1);

    d->dataPath = qDataPath("SchedulerData", d->schedulerName);

    if ("/cleanup" != d->routeName) {
        connectToServer(d->schedulerName % '.' % d->schedulerId);
        if (!waitForConnected(1000)) {
            QTimer::singleShot(0, this, [] {
                QCoreApplication::quit();
            });
            return;
        }

        QByteArray routeName = d->routeName.toUtf8();
        qint16 routeSize = routeName.size();
        if (!store(&routeSize, 2)) {
            QTimer::singleShot(0, this, [] {
                QCoreApplication::quit();
            });
            return;
        }
        if (!store(routeName.constData(), routeName.size())) {
            QTimer::singleShot(0, this, [] {
                QCoreApplication::quit();
            });
            return;
        }

        if (!waitForBytesWritten(1000)) {
            QTimer::singleShot(0, this, [] {
                QCoreApplication::quit();
            });
            return;
        }

        new QLogWriter(QString(), this);
        connect(this, &QSchedulerNode::readyRead,
                this, &QSchedulerNode::processDatagram);
    }

    if ("00000000" != d->safeguardId) {
        d->safeguard = new QSafeguard(d->dataPath, this);
        d->safeguard->setPipeName(d->schedulerName % '.' % d->safeguardId);
        d->safeguard->createClient();
    }

    if (arguments.contains("--developer-mode")) {
        d->isDeveloperMode = true;
    }
    if (!d->isDeveloperMode) {
        QString fileName = qAppPath("DeveloperMode.ini");
        d->isDeveloperMode = QFileInfo(fileName).exists();
    }

    QTimer::singleShot(0, QCoreApplication::instance(), [d] {
        auto functor = d->routeFunctors.value(d->routeName);
        if (!functor) {
            qWarning(lcSchedulerNode, "route failed.");
            QCoreApplication::quit();
        } else if (!functor()) {
            qWarning(lcSchedulerNode, "start failed.");
            QCoreApplication::quit();
        } else {
            if (d->arguments.isEmpty()) {
                qInfo(lcSchedulerNode, "ready: %s", qPrintable(d->routeName));
            } else {
                qInfo(lcSchedulerNode, "ready: %s %s", qPrintable(d->routeName),
                      qUtf8Printable(d->arguments.join(' ')));
            }
        }
    });
}

QSchedulerNode::~QSchedulerNode()
{
    QSchedulerNodePrivate::instance = nullptr;
}

QSchedulerNode *QSchedulerNode::instance()
{
    return QSchedulerNodePrivate::instance;
}

void QSchedulerNode::addProgramRoute(const QString &route, Functor functor)
{
    Q_D(QSchedulerNode);

    if ("/cleanup" == route) {
        qInfo(lcSchedulerNode, "\"/cleanup\" is keyword, use addCleanupRoute function.");
        return;
    }

    d->routeFunctors.insert(route, functor);
    d->modes.insert(route, Program);
}

void QSchedulerNode::addServiceRoute(const QString &route, Functor functor)
{
    Q_D(QSchedulerNode);

    if ("/cleanup" == route) {
        qInfo(lcSchedulerNode, "\"/cleanup\" is keyword, use addCleanupRoute function.");
        return;
    }

    d->routeFunctors.insert(route, functor);
    d->modes.insert(route, Service);
}

void QSchedulerNode::addCleanupRoute(Functor functor)
{
    Q_D(QSchedulerNode);

    d->routeFunctors.insert("/cleanup", functor);
    d->modes.insert("/cleanup", Program);
}

void QSchedulerNode::call(const QString &routeName, const QStringList &arguments)
{
    Q_D(QSchedulerNode);

    if (ConnectedState != state()) {
        return;
    }

    if (!d->routeFunctors.contains(routeName)) {
        qWarning(lcSchedulerNode, "missing: %s",
                 qPrintable(routeName));
        return;
    } else if ("/cleanup" == routeName) {
        qWarning(lcSchedulerNode, "cleanup: access denied.");
        return;
    }

    QByteArray datagram;
    QDataStream ds(&datagram, QIODevice::WriteOnly);

    ds << routeName << arguments;

    writeDatagram(1, datagram);
}

void QSchedulerNode::quit(const QString &routeName)
{
    Q_D(QSchedulerNode);

    if (ConnectedState != state()) {
        return;
    }

    if (!d->routeFunctors.contains(routeName)) {
        qWarning(lcSchedulerNode, "missing: %s",
                 qPrintable(routeName));
        return;
    } else if ("/cleanup" == routeName) {
        qWarning(lcSchedulerNode, "cleanup: access denied.");
        return;
    }

    QByteArray datagram;
    QDataStream ds(&datagram, QIODevice::WriteOnly);

    ds << routeName;

    writeDatagram(2, datagram);
}

void QSchedulerNode::writeDatagram(qint16 type, const char *data, int size)
{
    Q_D(QSchedulerNode);

    if (size < 0 || size > 0x4000) {
        qCritical(lcSchedulerNode) << "Socket error:"
                                   << QLocalSocket::DatagramTooLargeError;
        return;
    }

    if (thread() == QThread::currentThread()) {
        d->writeDatagram(type, data, size);
    } else {
        QCoreApplication::postEvent(this, new QSchedulerWriteEvent(type, data, size));
    }
}

void QSchedulerNode::writeDatagram(qint16 type, const QByteArray &datagram)
{
    Q_D(QSchedulerNode);

    if (thread() == QThread::currentThread()) {
        d->writeDatagram(type, datagram);
    } else {
        QCoreApplication::postEvent(this, new QSchedulerWriteEvent(type, datagram));
    }
}

QStringList QSchedulerNode::lastArguments() const
{
    Q_D(const QSchedulerNode);

    return d->arguments;
}

bool QSchedulerNode::isDeveloperMode() const
{
    Q_D(const QSchedulerNode);

    return d->isDeveloperMode;
}

QString QSchedulerNode::dataPath() const
{
    Q_D(const QSchedulerNode);

    return d->dataPath;
}

void QSchedulerNode::onError(QLocalSocket::LocalSocketError error)
{
    Q_UNUSED(error);

    QTimer::singleShot(0, this, [] {
        QCoreApplication::quit();
    });
}

void QSchedulerNode::onStateChanged(QLocalSocket::LocalSocketState state)
{
    if (QLocalSocket::UnconnectedState == state) {
        QTimer::singleShot(0, this, [] {
            QCoreApplication::quit();
        });
    }
}

void QSchedulerNode::processDatagram()
{
    Q_D(QSchedulerNode);

    while (true) {
        qint64 size = bytesAvailable();
        if (0 == d->datagramSize) {
            if (size < 4) {
                return;
            }

            if (!load(&d->datagramSize, 2)) {
                QTimer::singleShot(0, this, [] {
                    QCoreApplication::quit();
                });
                return;
            }
            if (!load(&d->datagramType, 2)) {
                QTimer::singleShot(0, this, [] {
                    QCoreApplication::quit();
                });
                return;
            }

            if (d->datagramSize > (16 * 1024)) { // limited
                QTimer::singleShot(0, this, [] {
                    QCoreApplication::quit();
                });
                return;
            }
        } else {
            if (size < d->datagramSize) {
                return;
            }

            if (!read(d->datagram.data(), d->datagramSize)) {
                QTimer::singleShot(0, this, [] {
                    QCoreApplication::quit();
                });
                return;
            }

            dispatch();

            d->datagramSize = 0;
        }
    }
}

void QSchedulerNode::dispatch()
{
    Q_D(QSchedulerNode);

    if (1 == d->datagramType) {
        ;
    } else if (2 == d->datagramType) {
        QDateTime now = QDateTime::currentDateTime();
        if (d->deadline.isValid()) {
            if (d->deadline < now) {
                if (d->safeguard)
                    d->safeguard->makeSnapshot();
            }
            d->deadline = QDateTime();
        } else {
            d->deadline = now.addMSecs(5000);
        }

        QTimer::singleShot(0, this, [] {
            QCoreApplication::quit();
        });
    } else if (5 == d->datagramType) {
        QDataStream ds(d->datagram);

        QStringList arguments;
        ds >> arguments;
        emit called(arguments);
    }
}

bool QSchedulerNode::event(QEvent *event)
{
    Q_D(QSchedulerNode);

    if (event->type() == QSchedulerWriteEvent::eventType) {
        auto swev = reinterpret_cast<QSchedulerWriteEvent *>(event);
        d->writeDatagram(swev->type, swev->datagram);
    }

    return QObject::event(event);
}

// class QSchedulerWriteEvent

const int QSchedulerWriteEvent::eventType = QEvent::registerEventType();

// class QSchedulerNodePrivate

QSchedulerNode *QSchedulerNodePrivate::instance = nullptr;

QSchedulerNodePrivate::QSchedulerNodePrivate()
    : q_ptr(nullptr)
    , datagramSize(0)
    , datagramType(0)
    , datagram(32 * 1024, 0)
    , safeguard(nullptr)
    , isDeveloperMode(false)
{
}

QSchedulerNodePrivate::~QSchedulerNodePrivate()
{
}

void QSchedulerNodePrivate::writeDatagram(qint16 type, const char *data, int size)
{
    Q_Q(QSchedulerNode);

    if (size < 0 || size > 0x4000) {
        qCritical(lcSchedulerNode) << "Socket error:"
                                   << QLocalSocket::DatagramTooLargeError;
        return;
    }

    if (!q->store(&size, 2)) {
        QTimer::singleShot(0, q, [] {
            QCoreApplication::quit();
        });
        return;
    }
    if (!q->store(&type, 2)) {
        QTimer::singleShot(0, q, [] {
            QCoreApplication::quit();
        });
        return;
    }

    if (!q->store(data, size)) {
        QTimer::singleShot(0, q, [] {
            QCoreApplication::quit();
        });
        return;
    }
}

void QSchedulerNodePrivate::writeDatagram(qint16 type, const QByteArray &datagram)
{
    writeDatagram(type, datagram.constData(), datagram.size());
}
