include(../Libraries.pri)

QT -= gui
DEFINES += QTK_CORE_LIB
TARGET   = $$qtLibraryTarget(Qtk5Core)

HEADERS += \
    QtkCoreGlobal.h

include(3rdparties/aes/aes.pri)
include(3rdparties/lzma/lzma.pri)

include(CxxListModel/CxxListModel.pri)
include(LogStream/LogStream.pri)
include(LogWriter/LogWriter.pri)
include(Lzma2/Lzma2.pri)
