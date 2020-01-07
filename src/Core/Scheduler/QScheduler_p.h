#ifndef QSCHEDULER_P_H
#define QSCHEDULER_P_H

#include <QHash>

#include "QScheduler.h"

class QSchedulerPrivate
{
    Q_DECLARE_PUBLIC(QScheduler)

public:
    QSchedulerPrivate();
    virtual ~QSchedulerPrivate();
protected:
    QScheduler *q_ptr;
public:
    static bool initialized;

public:
    static QScheduler *createScheduler();
protected:
    static QScheduler *instance;
    static QSettings   *settings;
protected:
    typedef std::function<bool()> Functor;
    QHash<QString, Functor> routeFunctors;
};

#endif // QSCHEDULER_P_H
