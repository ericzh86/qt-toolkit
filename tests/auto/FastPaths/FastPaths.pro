include(../../TestCases.pri)

QT -= gui

HEADERS += \
    FastPathsTest.h
SOURCES +=  \
    FastPathsTest.cpp

LIBS += -L$$DESTDIR -l$$qtLibraryTarget(Qtk0Core)
INCLUDEPATH += $$SRCROOT/Core
