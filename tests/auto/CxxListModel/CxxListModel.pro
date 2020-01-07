include(../../TestCases.pri)

QT -= gui

HEADERS += \
    CxxListModelTest.h \
    CxxListModelTester.h \
    CxxListModelTester_p.h
SOURCES += \
    CxxListModelTest.cpp \
    CxxListModelTester.cpp

LIBS += -L$$DESTDIR -l$$qtLibraryTarget(Qtk5Core)
INCLUDEPATH += $$SRCROOT/Core
