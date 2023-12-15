!config_nobotan {
    #Paths and linker flags for Windows
    win32{
        exists($$PWD/../../libs/botan/build) {
            CONFIG(debug, debug|release) {
                LIBS += -L$$PWD/../../libs/botan -lbotanD \
            }
            CONFIG(release, debug|release) {
                LIBS += -L$$PWD/../../libs/botan -lbotan
            }
            INCLUDEPATH += $$PWD/../..//libs/botan/build/include
        }
        else {
            error("Error: Build directory not found in libs/botan. Run the setup.bat file found in tucuxi-core")
        }

        LIBS += Iphlpapi.lib \
                -luser32
    }


    #Paths and linker flags for Linux/MacOS
    !android {
        unix {
            exists($$PWD/../../libs/botan) {
                LIBS += $$PWD/../../libs/botan/libbotan-2.a
                INCLUDEPATH += $$PWD/../../libs/botan/build/include/
            }
            else:exists($$PWD/../../bin/botan.a) {
                LIBS += $$PWD/../../bin/botan.a
                INCLUDEPATH += $$PWD/../../libs/botan/build/include/
            }
            else:exists($$PWD/../../libs/botan) {
                macx {
                    LIBS += -L $$PWD/../../libs/botan -lbotan
                }
                else {
                    LIBS += $$PWD/../../libs/botan/objs/botan.a
                }
                INCLUDEPATH += $$PWD/../../libs/botan/build/include/
            }
            else:exists(/usr/include/botan-1.10) {
                LIBS           += -L/usr/lib -lbotan-1.10
                INCLUDEPATH    += /usr/include/botan-1.10
            }
            else:exists(/usr/include/botan-1.11) {
                LIBS           += -L/usr/lib -lbotan-1.11
                INCLUDEPATH    += /usr/include/botan-1.11
            }
            else:exists(../../../ezechiel-deps-botan) {
                LIBS           += -L../../../ezechiel-deps-botan -lbotan-1.11
                INCLUDEPATH += ../../../ezechiel-deps-botan/build/include/botan
            }
            else {
                error("Error: Impossible to find botan")
            }
        }
    }
} # config_nobotan
