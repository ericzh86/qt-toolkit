#ifndef QFASTPATHS_P_H
#define QFASTPATHS_P_H

#include <QString>

#include "QFastPaths.h"

class QInternalPaths
{
public:
    QInternalPaths();

public:
    QString appPath;
    QString dataPath;
    QString cachePath;
    QString configPath;
    QString tempPath;
};

#endif // QFASTPATHS_P_H
