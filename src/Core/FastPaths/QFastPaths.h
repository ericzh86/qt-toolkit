#ifndef QFASTPATHS_H
#define QFASTPATHS_H

#include "../QtkCoreGlobal.h"

QTK_CORE_EXPORT QString qAppPath();
QTK_CORE_EXPORT QString qDataPath();
QTK_CORE_EXPORT QString qCachePath();
QTK_CORE_EXPORT QString qConfigPath();
QTK_CORE_EXPORT QString qTempPath();

QTK_CORE_EXPORT QString qAppPath(const QString &name);
QTK_CORE_EXPORT QString qDataPath(const QString &name);
QTK_CORE_EXPORT QString qCachePath(const QString &name);
QTK_CORE_EXPORT QString qConfigPath(const QString &name);
QTK_CORE_EXPORT QString qTempPath(const QString &name);

QTK_CORE_EXPORT QString qAppPath(const QString &name, const QString &name1);
QTK_CORE_EXPORT QString qDataPath(const QString &name, const QString &name1);
QTK_CORE_EXPORT QString qCachePath(const QString &name, const QString &name1);
QTK_CORE_EXPORT QString qConfigPath(const QString &name, const QString &name1);
QTK_CORE_EXPORT QString qTempPath(const QString &name, const QString &name1);

QTK_CORE_EXPORT QString qAppPath(const QString &name, const QString &name1, const QString &name2);
QTK_CORE_EXPORT QString qDataPath(const QString &name, const QString &name1, const QString &name2);
QTK_CORE_EXPORT QString qCachePath(const QString &name, const QString &name1, const QString &name2);
QTK_CORE_EXPORT QString qConfigPath(const QString &name, const QString &name1, const QString &name2);
QTK_CORE_EXPORT QString qTempPath(const QString &name, const QString &name1, const QString &name2);

#endif // QFASTPATHS_H
