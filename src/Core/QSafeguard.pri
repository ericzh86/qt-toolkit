HEADERS += \
    $$PWD/QSafeguard.h \
    $$PWD/QSafeguard_p.h
SOURCES += \
    $$PWD/QSafeguard.cpp

### breakpad

win32 {
    INCLUDEPATH += $$PWD/3rdparties/breakpad/windows

    CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Debug/lib -lcommon
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Debug/lib -lcrash_generation_client
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Debug/lib -lcrash_generation_server
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Debug/lib -lexception_handler
      } else {
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Release/lib -lcommon
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Release/lib -lcrash_generation_client
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Release/lib -lcrash_generation_server
    LIBS += -L$$PWD/3rdparties/breakpad/windows/client/windows/Release/lib -lexception_handler
    }
} else {
    # INCLUDEPATH += $$PWD/3rdparties/breakpad/mac

    # LIBS += -L$$PWD/3rdparties/breakpad/mac -lbreakpad
}
