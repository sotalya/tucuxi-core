

CONFIG += c++14
CONFIG -= qt

unix {

QMAKE_CXXFLAGS += -Wall -Wconversion -Wunreachable-code -Wuninitialized -Wold-style-cast

QMAKE_LFLAGS += -Wl,--no-as-needed -ldl
}

include($$PWD/includepaths.pri)
