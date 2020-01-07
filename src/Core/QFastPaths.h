#ifndef QFASTPATHS_H
#define QFASTPATHS_H

#include "QtkCoreGlobal.h"

QTK_CORE_EXPORT QString qAppPath();
QTK_CORE_EXPORT QString qDataPath();
QTK_CORE_EXPORT QString qCachePath();
QTK_CORE_EXPORT QString qConfigPath();
QTK_CORE_EXPORT QString qTempPath();

QTK_CORE_EXPORT QString qAppPath(const QString &p0);
QTK_CORE_EXPORT QString qDataPath(const QString &p0);
QTK_CORE_EXPORT QString qCachePath(const QString &p0);
QTK_CORE_EXPORT QString qConfigPath(const QString &p0);
QTK_CORE_EXPORT QString qTempPath(const QString &p0);

QTK_CORE_EXPORT QString qAppPath(const QString &p0, const QString &p1);
QTK_CORE_EXPORT QString qDataPath(const QString &p0, const QString &p1);
QTK_CORE_EXPORT QString qCachePath(const QString &p0, const QString &p1);
QTK_CORE_EXPORT QString qConfigPath(const QString &p0, const QString &p1);
QTK_CORE_EXPORT QString qTempPath(const QString &p0, const QString &p1);

QTK_CORE_EXPORT QString qAppPath(const QString &p0, const QString &p1, const QString &p2);
QTK_CORE_EXPORT QString qDataPath(const QString &p0, const QString &p1, const QString &p2);
QTK_CORE_EXPORT QString qCachePath(const QString &p0, const QString &p1, const QString &p2);
QTK_CORE_EXPORT QString qConfigPath(const QString &p0, const QString &p1, const QString &p2);
QTK_CORE_EXPORT QString qTempPath(const QString &p0, const QString &p1, const QString &p2);

#endif // QFASTPATHS_H
