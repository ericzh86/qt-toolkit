#ifndef FASTPATHSTEST_H
#define FASTPATHSTEST_H

#include <QtTest>

class FastPathsTest : public QObject
{
    Q_OBJECT

public:
    FastPathsTest();
    virtual ~FastPathsTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testCase();
};

#endif // FASTPATHSTEST_H
