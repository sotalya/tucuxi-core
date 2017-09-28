/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "utils.h"

namespace Tucuxi {
namespace Common {

std::string Utils::getAppFolder(char** _argv)
{
    std::string::size_type found = std::string(_argv[0]).find_last_of("/\\");
    std::string appFolder = std::string(_argv[0]).substr(0, found);
    return appFolder;
}

}
}
