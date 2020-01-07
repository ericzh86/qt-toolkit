#include "QFastPaths.h"
#include "QFastPaths_p.h"

#include <QStandardPaths>
#include <QCoreApplication>
#include <QStringBuilder>
#include <QLoggingCategory>

Q_GLOBAL_STATIC(QInternalPaths, qInternalPaths)

Q_LOGGING_CATEGORY(lcFastPaths, "QFastPaths")

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

QString qAppPath(const QString &p0)
{
    const QString &path = qInternalPaths->appPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"appPath\".");
        return path;
    }

    return path % '/' % p0;
}

QString qDataPath(const QString &p0)
{
    const QString &path = qInternalPaths->dataPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"dataPath\".");
        return path;
    }

    return path % '/' % p0;
}

QString qCachePath(const QString &p0)
{
    const QString &path = qInternalPaths->cachePath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"cachePath\".");
        return path;
    }

    return path % '/' % p0;
}

QString qConfigPath(const QString &p0)
{
    const QString &path = qInternalPaths->configPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"configPath\".");
        return path;
    }

    return path % '/' % p0;
}

QString qTempPath(const QString &p0)
{
    const QString &path = qInternalPaths->tempPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"tempPath\".");
        return path;
    }

    return path % '/' % p0;
}

QString qAppPath(const QString &p0, const QString &p1)
{
    const QString &path = qInternalPaths->appPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"appPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1;
}

QString qDataPath(const QString &p0, const QString &p1)
{
    const QString &path = qInternalPaths->dataPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"dataPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1;
}

QString qCachePath(const QString &p0, const QString &p1)
{
    const QString &path = qInternalPaths->cachePath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"cachePath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1;
}

QString qConfigPath(const QString &p0, const QString &p1)
{
    const QString &path = qInternalPaths->configPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"configPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1;
}

QString qTempPath(const QString &p0, const QString &p1)
{
    const QString &path = qInternalPaths->tempPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"tempPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1;
}

QString qAppPath(const QString &p0, const QString &p1, const QString &p2)
{
    const QString &path = qInternalPaths->appPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"appPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1 % '/' % p2;
}

QString qDataPath(const QString &p0, const QString &p1, const QString &p2)
{
    const QString &path = qInternalPaths->dataPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"dataPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1 % '/' % p2;
}

QString qCachePath(const QString &p0, const QString &p1, const QString &p2)
{
    const QString &path = qInternalPaths->cachePath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"cachePath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1 % '/' % p2;
}

QString qConfigPath(const QString &p0, const QString &p1, const QString &p2)
{
    const QString &path = qInternalPaths->configPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"configPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1 % '/' % p2;
}

QString qTempPath(const QString &p0, const QString &p1, const QString &p2)
{
    const QString &path = qInternalPaths->tempPath;
    if (path.isEmpty()) {
        qWarning(lcFastPaths, "empty \"tempPath\".");
        return path;
    }

    return path % '/' % p0 % '/' % p1 % '/' % p2;
}

// QInternalPaths

QInternalPaths::QInternalPaths()
{
    appPath    = QCoreApplication::applicationDirPath();
    dataPath   = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    cachePath  = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    tempPath   = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    Q_ASSERT_X(!appPath.isEmpty(), "QFastPaths", "\"appPath\" is empty.");
    Q_ASSERT_X(!dataPath.isEmpty(), "QFastPaths", "\"dataPath\" is empty.");
    Q_ASSERT_X(!cachePath.isEmpty(), "QFastPaths", "\"cachePath\" is empty.");
    Q_ASSERT_X(!configPath.isEmpty(), "QFastPaths", "\"configPath\" is empty.");
    Q_ASSERT_X(!tempPath.isEmpty(), "QFastPaths", "\"tempPath\" is empty.");
}
