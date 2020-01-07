include(../Libraries.pri)

QT -= gui
DEFINES += QTK_CORE_LIB
TARGET   = $$qtLibraryTarget(Qtk0Core)

HEADERS += \
    QtkCoreGlobal.h

include(3rdparties/aes/aes.pri)
include(3rdparties/lzma/lzma.pri)

include(QFastPaths.pri)
include(QCxxListModel.pri)
include(QLogStream.pri)
include(QLogWriter.pri)
include(QLzma2.pri)
include(QSafeguard.pri)
include(QScheduler.pri)
