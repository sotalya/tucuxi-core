/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_UTILS_H
#define TUCUXI_TUCUCOMMON_UTILS_H

#include <string>

namespace Tucuxi {
namespace Common {

class Utils
{
public:
    static std::string getAppFolder(char** argv);
};

}
}

#endif // TUCUXI_TUCUCOMMON_UTILS_H