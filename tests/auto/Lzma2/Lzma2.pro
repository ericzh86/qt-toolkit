include(../../TestCases.pri)

QT -= gui

HEADERS += \
    Lzma2Test.h
SOURCES += \
    Lzma2Test.cpp

RESOURCES += \
    Lzma2.qrc

LIBS += -L$$DESTDIR -l$$qtLibraryTarget(Qtk0Core)
INCLUDEPATH += $$SRCROOT/Core
