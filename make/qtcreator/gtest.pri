# Add gtest libs
win32 {
    LIBS += "C:\Program Files\googletest-distribution\lib\gtest.lib"
    INCLUDEPATH += "C:\Program Files\googletest-distribution\include"
}
else {
    LIBS += -lgtest
}
