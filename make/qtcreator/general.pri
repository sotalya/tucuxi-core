

CONFIG += c++14
CONFIG -= qt

LIBS += ../../../libs/tiny-js-master-20170629/objs/tinyjs.a \
        ../../../libs/botan-2.1.0/objs/botan.a

QMAKE_LFLAGS += -Wl,--no-as-needed -ldl

INCLUDEPATH += ../../../src \
    ../../../libs/spdlog-master-20170622/include/ \
    ../../../libs/boost-1.63.0 \
    ../../../libs/eigen-3.3.2/ \
    ../../../libs/botan-2.1.0/build/include/ \
    ../../../libs/eigen-3.3.2/ \
    ../../../libs/fructose-1.3/fructose/include \
    ../../../libs/date-master-20170711 \
    ../../../libs/tiny-js-master-20170629 \
    ../../../libs/rapidxml-1.13
