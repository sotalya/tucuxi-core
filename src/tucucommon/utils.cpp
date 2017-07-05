/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "utils.h"

namespace Tucuxi {
namespace Common {

std::string Utils::getAppFolder(char** argv)
{
    std::string::size_type found = std::string(argv[0]).find_last_of("/\\");
    std::string appFolder = std::string(argv[0]).substr(0, found);
    return appFolder;
}

}
}
