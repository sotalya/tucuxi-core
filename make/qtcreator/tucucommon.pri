
SOURCES +=  \
    $$PWD/../../libs/spdlog-master-20170622/include/spdlog/fmt/bundled/format.cc

HEADERS += \
    $$PWD/../../src/tucucommon/component.h \
    $$PWD/../../src/tucucommon/componentmanager.h \
    $$PWD/../../src/tucucommon/cryptohelper.h \
    $$PWD/../../src/tucucommon/ilogger.h \
    $$PWD/../../src/tucucommon/interface.h \
    $$PWD/../../src/tucucommon/logger.h \
    $$PWD/../../src/tucucommon/licensemanager.h \
    $$PWD/../../src/tucucommon/loggerhelper.h \
    $$PWD/../../src/tucucommon/utils.h \
    $$PWD/../../src/tucucommon/datetime.h \
    $$PWD/../../src/tucucommon/duration.h \
    $$PWD/../../src/tucucommon/jsengine.h \
    $$PWD/../../src/tucucommon/timeofday.h \
    $$PWD/../../src/tucucommon/basetypes.h \
    $$PWD/../../src/tucucommon/systeminfo.h \
    $$PWD/../../src/tucucommon/xmlattribute.h \
    $$PWD/../../src/tucucommon/xmldocument.h \
    $$PWD/../../src/tucucommon/xmliterator.h \
    $$PWD/../../src/tucucommon/xmlnode.h \
    $$PWD/../../src/tucucommon/general.h \
    $$PWD/../../src/tucucommon/translatablestring.h \
    $$PWD/../../src/tucucommon/iimport.h

SOURCES += \
    $$PWD/../../src/tucucommon/componentmanager.cpp \
    $$PWD/../../src/tucucommon/cryptohelper.cpp \
    $$PWD/../../src/tucucommon/logger.cpp \
    $$PWD/../../src/tucucommon/licensemanager.cpp \
    $$PWD/../../src/tucucommon/utils.cpp \
    $$PWD/../../src/tucucommon/datetime.cpp \
    $$PWD/../../src/tucucommon/duration.cpp \
    $$PWD/../../src/tucucommon/jsengine.cpp \
    $$PWD/../../src/tucucommon/timeofday.cpp \
    $$PWD/../../src/tucucommon/systeminfo.cpp \
    $$PWD/../../src/tucucommon/xmlattribute.cpp \
    $$PWD/../../src/tucucommon/xmldocument.cpp \
    $$PWD/../../src/tucucommon/xmlnode.cpp \
    $$PWD/../../src/tucucommon/translatablestring.cpp

DISTFILES += \
    $$PWD/../../src/tucucommon/makefile
