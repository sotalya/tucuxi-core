
INCLUDEPATH += $$PWD/../../src \
    $$PWD/../../libs/spdlog/include/ \
    $$PWD/../../libs/boost \
    $$PWD/../../libs/eigen/ \
    $$PWD/../../libs/fructose/include \
    $$PWD/../../libs/date/include \
    $$PWD/../../libs/tiny-js \
    $$PWD/../../libs/rapidxml \
    $$PWD/../../libs/cxxopts/include \
    $$PWD/../../libs/rapidjson/include

!win32 {
    # There is a duplicate here, for nice includes. Using -isystem
    # disables warnings on these libraries, so we can be very
    # strict on the Tucuxi code
    QMAKE_CXXFLAGS += \
        -isystem $$PWD/../../libs/spdlog/include/ \
        -isystem $$PWD/../../libs/boost \
        -isystem $$PWD/../../libs/eigen/ \
        -isystem $$PWD/../../libs/fructose/include \
        -isystem $$PWD/../../libs/date/include \
        -isystem $$PWD/../../libs/tiny-js \
        -isystem $$PWD/../../libs/rapidxml \
        -isystem $$PWD/../../libs/cxxopts/include \
        -isystem $$PWD/../../libs/rapidjson/include
}

!config_nobotan {

win32 {
    INCLUDEPATH += $$PWD/../../libs/botan/build/include
}
else {
    INCLUDEPATH +=  $$PWD/../../libs/botan/build/include/ \
}
}
