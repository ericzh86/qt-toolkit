#ifndef QSCHEDULINGTASK_P_H
#define QSCHEDULINGTASK_P_H

#include <QDateTime>
#include <QPointer>
#include <QProcess>
#include <QTimer>

#include "QSchedulingTask.h"
#include "QSchedulingNode.h"

class QSchedulingTaskPrivate
{
    Q_DECLARE_PUBLIC(QSchedulingTask)

public:
    explicit QSchedulingTaskPrivate(const QString &route);
    virtual ~QSchedulingTaskPrivate();
protected:
    QSchedulingTask *q_ptr;

protected:
    const QString      route;
    QPointer<QProcess> process;
    QStringList        arguments;
    int                retries;
protected:
    QSchedulingTask::Mode mode;
    QPointer<QSchedulingNode> node;

protected:
    QTimer   *callTimer;
    QDateTime deadline;
    QTimer   *stopTimer;

protected:
    bool isQuitAll;
    bool isDirty;
};

#endif // QSCHEDULINGTASK_P_H
