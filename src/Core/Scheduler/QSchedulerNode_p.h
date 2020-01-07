#ifndef QSCHEDULERNODE_P_H
#define QSCHEDULERNODE_P_H

#include <QHash>
#include <QEvent>
#include <QDateTime>

#include "QSchedulerNode.h"

#include "../Safeguard/QSafeguard.h"

// class QSchedulerWriteEvent

class QSchedulerWriteEvent : public QEvent
{
public:
    QSchedulerWriteEvent(qint16 type, const char *data, int size)
        : QEvent(static_cast<QEvent::Type>(eventType))
        , type(type), datagram(data, size)
    { }
    QSchedulerWriteEvent(qint16 type, const QByteArray &datagram)
        : QEvent(static_cast<QEvent::Type>(eventType))
        , type(type), datagram(datagram)
    { }
public:
    qint16 type;
    QByteArray datagram;
    static const int eventType;
};

// class QSchedulerNodePrivate

class QSchedulerNodePrivate
{
    Q_DECLARE_PUBLIC(QSchedulerNode)

public:
    QSchedulerNodePrivate();
    virtual ~QSchedulerNodePrivate();
protected:
    QSchedulerNode *q_ptr;

protected:
    static QSchedulerNode *instance;
protected:
    typedef std::function<bool()> Functor;
    QHash<QString, Functor> routeFunctors;
    QHash<QString, QSchedulerNode::Mode> modes;

public:
    void writeDatagram(qint16 type, const char *data, int size);
    void writeDatagram(qint16 type, const QByteArray &datagram);

protected:
    QStringList arguments;
    QString schedulerName;
    QString schedulerId;
    QString safeguardId;
    QString routeName;

protected:
    qint16 datagramSize;
    qint16 datagramType;
    QByteArray datagram;

protected:
    QSafeguard *safeguard;
    bool isDeveloperMode;
    QDateTime deadline;
    QString dataPath;
};

#endif // QSCHEDULERNODE_P_H
