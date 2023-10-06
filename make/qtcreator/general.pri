

CONFIG += c++14
CONFIG -= qt

#LIBS += ../../../libs/tiny-js-master-20170629/objs/tinyjs.a \
#        ../../../libs/botan-2.19.3/objs/botan.a
unix {
LIBS += $$PWD/../../bin/tinyjs.a

!config_nobotan {
        $$PWD/../../bin/botan.a
}

QMAKE_CXXFLAGS += -Wall -Wconversion -Wunreachable-code -Wuninitialized -Wold-style-cast

QMAKE_LFLAGS += -Wl,--no-as-needed -ldl
}

include($$PWD/includepaths.pri)
