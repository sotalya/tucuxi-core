//@@license@@

#include <gtest/gtest.h>

#include "tucucommon/utils.h"

std::string appFolder;

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    appFolder = Tucuxi::Common::Utils::getAppFolder(argv);
    return RUN_ALL_TESTS();
}
