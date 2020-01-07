#ifndef CXXLISTMODELTESTER_P_H
#define CXXLISTMODELTESTER_P_H

#include <QtDebug>

#include "CxxListModelTester.h"

typedef QPair<int, int> ListModelChanged;

class ListModelTesterPrivate
{
    Q_DECLARE_PUBLIC(ListModelTester)

public:
    ListModelTesterPrivate();
    virtual ~ListModelTesterPrivate();
protected:
    ListModelTester *q_ptr;

protected:
    Internal::QCxxListModel *model;
    QList<ListModelChanged>  changedList;
    int                      count;
};

#endif // CXXLISTMODELTESTER_P_H
