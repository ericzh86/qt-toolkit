#ifndef QSCHEDULERHOST_P_H
#define QSCHEDULERHOST_P_H

#include <QHash>
#include <QTimer>

#include "QSchedulerHost.h"

#include "QSchedulingNode.h"
#include "QSchedulingTask.h"

#include "../Safeguard/QSafeguard.h"

class QSchedulerHostPrivate
{
    Q_DECLARE_PUBLIC(QSchedulerHost)

public:
    QSchedulerHostPrivate();
    virtual ~QSchedulerHostPrivate();
protected:
    QSchedulerHost *q_ptr;

protected:
    static QSchedulerHost *instance;
protected:
    typedef std::function<bool()> Functor;
    QHash<QString, Functor> routeFunctors;
    QHash<QString, QSchedulerHost::Mode> modes;

protected:
    QStringList arguments;
    QString schedulerName;
    QString schedulerId;
    QString safeguardId;
    bool    isClosing;

protected:
    QHash<QString, QSchedulingNode *> nodes;
    QHash<QString, QSchedulingTask *> tasks;

protected:
    QTimer *quitTimer;

protected:
    QSafeguard *safeguard;
    bool isDeveloperMode;
};

#endif // QSCHEDULERHOST_P_H
