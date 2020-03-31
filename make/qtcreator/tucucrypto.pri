
SOURCES +=  \
    $$PWD/../../libs/spdlog-master-20170622/include/spdlog/fmt/bundled/format.cc

HEADERS += \
    $$PWD/../../src/tucucrypto/cryptohelper.h \
    $$PWD/../../src/tucucrypto/licensemanager.h \
    $$PWD/../../src/tucucrypto/systeminfo.h \

SOURCES += \
    $$PWD/../../src/tucucrypto/cryptohelper.cpp \
    $$PWD/../../src/tucucrypto/licensemanager.cpp \
    $$PWD/../../src/tucucrypto/systeminfo.cpp \

DISTFILES += \
    $$PWD/../../src/tucucrypto/makefile
