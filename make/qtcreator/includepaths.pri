
INCLUDEPATH += $$PWD/../../src \
    $$PWD/../../libs/spdlog/include/ \
    $$PWD/../../libs/boost \
    $$PWD/../../libs/eigen/ \
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
        -isystem $$PWD/../../libs/date/include \
        -isystem $$PWD/../../libs/tiny-js \
        -isystem $$PWD/../../libs/rapidxml \
        -isystem $$PWD/../../libs/cxxopts/include \
        -isystem $$PWD/../../libs/rapidjson/include
}
