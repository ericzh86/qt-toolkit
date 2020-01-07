#ifndef QSCHEDULINGNODE_P_H
#define QSCHEDULINGNODE_P_H

#include "QSchedulingNode.h"

class QSchedulingNodePrivate
{
    Q_DECLARE_PUBLIC(QSchedulingNode)

public:
    QSchedulingNodePrivate();
    virtual ~QSchedulingNodePrivate();
protected:
    QSchedulingNode *q_ptr;

protected:
    QString routeName;

protected:
    qint16 datagramSize;
    qint16 datagramType;
    QByteArray datagram;
};

#endif // QSCHEDULINGNODE_P_H
