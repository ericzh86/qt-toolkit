#include "QFastPaths.h"
#include "QFastPaths_p.h"

#include <QStandardPaths>
#include <QCoreApplication>
#include <QStringBuilder>

Q_GLOBAL_STATIC(QInternalPaths, qInternalPaths)

// QPaths

QString qAppPath()
{
    return qInternalPaths->appPath;
}

QString qDataPath()
{
    return qInternalPaths->dataPath;
}

QString qCachePath()
{
    return qInternalPaths->cachePath;
}

QString qConfigPath()
{
    return qInternalPaths->configPath;
}

QString qTempPath()
{
    return qInternalPaths->tempPath;
}

QString qAppPath(const QString &name)
{
    return qInternalPaths->appPath % name;
}

QString qDataPath(const QString &name)
{
    return qInternalPaths->dataPath % name;
}

QString qCachePath(const QString &name)
{
    return qInternalPaths->cachePath % name;
}

QString qConfigPath(const QString &name)
{
    return qInternalPaths->configPath % name;
}

QString qTempPath(const QString &name)
{
    return qInternalPaths->tempPath % name;
}

QString qAppPath(const QString &name, const QString &name1)
{
    return qInternalPaths->appPath % name % '/' % name1;
}

QString qDataPath(const QString &name, const QString &name1)
{
    return qInternalPaths->dataPath % name % '/' % name1;
}

QString qCachePath(const QString &name, const QString &name1)
{
    return qInternalPaths->cachePath % name % '/' % name1;
}

QString qConfigPath(const QString &name, const QString &name1)
{
    return qInternalPaths->configPath % name % '/' % name1;
}

QString qTempPath(const QString &name, const QString &name1)
{
    return qInternalPaths->tempPath % name % '/' % name1;
}

QString qAppPath(const QString &name, const QString &name1, const QString &name2)
{
    return qInternalPaths->appPath % name % '/' % name1 % '/' % name2;
}

QString qDataPath(const QString &name, const QString &name1, const QString &name2)
{
    return qInternalPaths->dataPath % name % '/' % name1 % '/' % name2;
}

QString qCachePath(const QString &name, const QString &name1, const QString &name2)
{
    return qInternalPaths->cachePath % name % '/' % name1 % '/' % name2;
}

QString qConfigPath(const QString &name, const QString &name1, const QString &name2)
{
    return qInternalPaths->configPath % name % '/' % name1 % '/' % name2;
}

QString qTempPath(const QString &name, const QString &name1, const QString &name2)
{
    return qInternalPaths->tempPath % name % '/' % name1 % '/' % name2;
}

// QInternalPaths

QInternalPaths::QInternalPaths()
{
    appPath = QCoreApplication::applicationDirPath();

    dataPath   = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    cachePath  = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    tempPath   = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
}
