include(../../TestCases.pri)

QT -= gui

HEADERS += \
    QtLzma2Test.h
SOURCES += \
    QtLzma2Test.cpp

RESOURCES += \
    Lzma2.qrc

LIBS += -L$$DESTDIR -l$$qtLibraryTarget(Qtk0Core)
INCLUDEPATH += $$SRCROOT/Core
