

CONFIG += c++14
CONFIG -= qt

#LIBS += ../../../libs/tiny-js-master-20170629/objs/tinyjs.a \
#        ../../../libs/botan-2.1.0/objs/botan.a
unix {
LIBS += $$PWD/../../bin/tinyjs.a \
        $$PWD/../../bin/botan.a


QMAKE_LFLAGS += -Wl,--no-as-needed -ldl
}

include($$PWD/includepaths.pri)
