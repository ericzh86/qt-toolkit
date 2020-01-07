#include "QSchedulingNode.h"
#include "QSchedulingNode_p.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QLoggingCategory>

#include "../Safeguard/QSafeguard.h"
#include "../LogWriter/QLogWriter.h"

Q_LOGGING_CATEGORY(lcSchedulingNode, "Ol.Scheduler")

// class QSchedulingNode

QSchedulingNode::QSchedulingNode()
    : QLocalSocket(QCoreApplication::instance())
    , d_ptr(new QSchedulingNodePrivate())
{
    d_ptr->q_ptr = this;
}

QSchedulingNode::~QSchedulingNode()
{
    flush();
    close();
}

void QSchedulingNode::setRouteName(const QString &name)
{
    Q_D(QSchedulingNode);

    Q_ASSERT(d->routeName.isEmpty());
    Q_ASSERT(!name.isEmpty());
    d->routeName = name;
}

QString QSchedulingNode::routeName() const
{
    Q_D(const QSchedulingNode);

    return d->routeName;
}

void QSchedulingNode::call(const QStringList &arguments)
{
    Q_D(QSchedulingNode);

    if (state() != QLocalSocket::ConnectedState) {
        return;
    }

    QByteArray datagram;
    QDataStream ds(&datagram, QIODevice::WriteOnly);

    ds << d->routeName << arguments;

    writeDatagram(1, datagram);
}

void QSchedulingNode::quit()
{
    Q_D(QSchedulingNode);

    if (state() != QLocalSocket::ConnectedState) {
        return;
    }

    QByteArray datagram;
    QDataStream ds(&datagram, QIODevice::WriteOnly);

    ds << d->routeName;

    writeDatagram(2, datagram);
}

bool QSchedulingNode::writeDatagram(qint16 type, const char *data, int size)
{
    if (size < 0 || size > 0x4000) {
        qCritical(lcSchedulingNode) << "Socket error:"
                                    << QLocalSocket::DatagramTooLargeError;
        return false;
    }

    if (!store(&size, 2)) { return false; }
    if (!store(&type, 2)) { return false; }

    if (!store(data, size)) { return false; }

    return true;
}

bool QSchedulingNode::writeDatagram(qint16 type, const QByteArray &datagram)
{
    return QSchedulingNode::writeDatagram(type, datagram.constData(), datagram.size());
}

void QSchedulingNode::processDatagram()
{
    Q_D(QSchedulingNode);

    while (true) {
        qint64 size = bytesAvailable();
        if (0 == d->datagramSize) {
            if (size < 4) { return; }

            if (!load(&d->datagramSize, 2)) { return; }
            if (!load(&d->datagramType, 2)) { return; }

            if (d->datagramSize > (16 * 1024)) { // limited
                QLocalSocket::close();
                return;
            }
        } else {
            if (size < d->datagramSize) { return; }
            if (!read(d->datagram.data(), d->datagramSize)) { return; }

            dispatch();
            d->datagramSize = 0;
        }
    }
}

void QSchedulingNode::dispatch()
{
    Q_D(QSchedulingNode);

    if (3 == d->datagramType) {
        QLogWriter::writeBlockBuffer(d->datagram.constData(), d->datagramSize);
    } else if (4 == d->datagramType) {
        QLogWriter::writePlainBuffer(d->datagram.constData(), d->datagramSize);
    } else if (1 == d->datagramType) {
        QString     routeName;
        QStringList arguments;

        QDataStream ds(d->datagram);
        ds >> routeName >> arguments;

        emit callRequested(routeName, arguments);
    } else if (2 == d->datagramType) {
        QString routeName;

        QDataStream ds(d->datagram);
        ds >> routeName;

        emit quitRequested(routeName);
    } else if (11 == d->datagramType) {
        /*
        auto raven = OlRaven::instance();
        if (!raven)
            raven = new OlRaven(QString());

        QString message = QString::fromUtf8(d->datagram, d->datagramSize);

        int i = message.indexOf("=", 5);
        if (i >= 0) {
            QString k = message.mid(5, i - 5);
            QString v = message.mid(i + 1);
            if (v.isEmpty()) {
                v = "<empty>";
            }

            raven->setTagValue(k, v);
        }
        */
    } else if (12 == d->datagramType) {
        /*
        auto raven = OlRaven::instance();
        if (!raven)
            raven = new OlRaven(QString());
        */

        // QString message = QString::fromUtf8(d->datagram, d->datagramSize);

        // raven->send(message);
    }
}

// class QSchedulingNodePrivate

QSchedulingNodePrivate::QSchedulingNodePrivate()
    : q_ptr(nullptr)
    , datagramSize(0)
    , datagramType(0)
    , datagram(32 * 1024, 0)
{
}

QSchedulingNodePrivate::~QSchedulingNodePrivate()
{
}
