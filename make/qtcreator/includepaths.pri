
INCLUDEPATH += $$PWD/../../src \
    $$PWD/../../libs/spdlog-master-20170622/include/ \
    $$PWD/../../libs/boost-1.83.0 \
    $$PWD/../../libs/eigen-3.3.2/ \
    $$PWD/../../libs/fructose/include \
    $$PWD/../../libs/date/include \
    $$PWD/../../libs/tiny-js-master-20170629 \
    $$PWD/../../libs/rapidxml \
    $$PWD/../../libs/cxxopts \
    $$PWD/../../libs/rapidjson/include

!win32 {
    # There is a duplicate here, for nice includes. Using -isystem
    # disables warnings on these libraries, so we can be very
    # strict on the Tucuxi code
    QMAKE_CXXFLAGS += \
        -isystem $$PWD/../../libs/spdlog-master-20170622/include/ \
        -isystem $$PWD/../../libs/boost-1.83.0 \
        -isystem $$PWD/../../libs/eigen-3.3.2/ \
        -isystem $$PWD/../../libs/fructose/include \
        -isystem $$PWD/../../libs/date/include \
        -isystem $$PWD/../../libs/tiny-js-master-20170629 \
        -isystem $$PWD/../../libs/rapidxml \
        -isystem $$PWD/../../libs/cxxopts \
        -isystem $$PWD/../../libs/rapidjson/include
}

!config_nobotan {

win32 {
    !config_tucucore_lib {
        INCLUDEPATH += C:\Botan\Botan2-64\include\botan-2
    }
    else {
        INCLUDEPATH +=  $$PWD/../../libs/botan-2.19.3/build/include/ \
    }
}
else {
    INCLUDEPATH +=  $$PWD/../../libs/botan-2.19.3/build/include/ \
}
}
