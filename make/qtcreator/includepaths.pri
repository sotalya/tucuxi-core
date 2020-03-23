
INCLUDEPATH += $$PWD/../../src \
    $$PWD/../../libs/spdlog-master-20170622/include/ \
    $$PWD/../../libs/boost-1.63.0 \
    $$PWD/../../libs/eigen-3.3.2/ \
    $$PWD/../../libs/fructose-1.3/fructose/include \
    $$PWD/../../libs/date-master-20170711 \
    $$PWD/../../libs/tiny-js-master-20170629 \
    $$PWD/../../libs/rapidxml-1.13 \
    $$PWD/../../libs/cxxopts \
    $$PWD/../../libs/rapidjson-master-20190220/include


win32 {
    !config_tucucore_lib {
        INCLUDEPATH += C:\Botan\Botan2-64\include\botan-2
    }
    else {
        INCLUDEPATH +=  $$PWD/../../libs/botan-2.1.0/build/include/ \
    }
}
else {
    INCLUDEPATH +=  $$PWD/../../libs/botan-2.1.0/build/include/ \
}
