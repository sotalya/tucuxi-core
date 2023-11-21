TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

win32{
    exists($$PWD/../../../libs/botan/build) {
        CONFIG(debug, debug|release) {
            LIBS += -L$$PWD/../../../libs/botan -lbotan \
                    -luser32
        }
        CONFIG(release, debug|release) {
            LIBS += -L$$PWD/../../../libs/botan -lbotan \
                    -luser32
        }
        INCLUDEPATH += $$PWD/../../../libs/botan/build/include
    }
    else {
        error("Error: Build directory not found in libs/botan. Run the setup.bat file found in tucuxi-core")
    }

    LIBS += Iphlpapi.lib
}

include(../general.pri)
include(../tucucommon.pri)
include(../tucucrypto.pri)

HEADERS += \
    ../../../test/tucucrypto/test_licensemanager.h \
    ../../../test/tucucrypto/test_cryptohelper.h \

SOURCES += \
    ../../../test/tucucrypto/tests.cpp


!win32 {
    # Because of Eigen:
    QMAKE_CXXFLAGS += -Wno-int-in-bool-context

    # Because of macros and clang:
    QMAKE_CXXFLAGS += -Wno-extra-semi-stmt
}
