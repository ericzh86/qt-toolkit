#include "FastPathsTest.h"

#include "QFastPaths.h"

FastPathsTest::FastPathsTest()
{
}

FastPathsTest::~FastPathsTest()
{
}

void FastPathsTest::initTestCase()
{
}

void FastPathsTest::cleanupTestCase()
{
}

void FastPathsTest::testCase()
{
    QString appPath = QCoreApplication::applicationDirPath();

    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QString cachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    QVERIFY(qAppPath() == appPath);
    QVERIFY(qDataPath() == dataPath);
    QVERIFY(qCachePath() == cachePath);
    QVERIFY(qConfigPath() == configPath);
    QVERIFY(qTempPath() == tempPath);

    QCOMPARE(qAppPath("1"), appPath + "/1");
    QCOMPARE(qDataPath("1"), dataPath + "/1");
    QCOMPARE(qCachePath("1"), cachePath + "/1");
    QCOMPARE(qConfigPath("1"), configPath + "/1");
    QCOMPARE(qTempPath("1"), tempPath + "/1");

    QCOMPARE(qAppPath("1", "2"), appPath + "/1/2");
    QCOMPARE(qDataPath("1", "2"), dataPath + "/1/2");
    QCOMPARE(qCachePath("1", "2"), cachePath + "/1/2");
    QCOMPARE(qConfigPath("1", "2"), configPath + "/1/2");
    QCOMPARE(qTempPath("1", "2"), tempPath + "/1/2");

    QCOMPARE(qAppPath("1", "2", "3"), appPath + "/1/2/3");
    QCOMPARE(qDataPath("1", "2", "3"), dataPath + "/1/2/3");
    QCOMPARE(qCachePath("1", "2", "3"), cachePath + "/1/2/3");
    QCOMPARE(qConfigPath("1", "2", "3"), configPath + "/1/2/3");
    QCOMPARE(qTempPath("1", "2", "3"), tempPath + "/1/2/3");
}

QTEST_GUILESS_MAIN(FastPathsTest)
