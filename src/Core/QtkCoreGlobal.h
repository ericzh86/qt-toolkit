#ifndef QTKCOREGLOBAL_H
#define QTKCOREGLOBAL_H

#include <QtGlobal>

#if defined(QTK_CORE_LIB)
#  define QTK_CORE_EXPORT Q_DECL_EXPORT
#else
#  define QTK_CORE_EXPORT Q_DECL_IMPORT
#endif

#endif // QTKCOREGLOBAL_H
