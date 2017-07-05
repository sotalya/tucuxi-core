/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_UTILS_H
#define TUCUXI_TUCUCOMMON_UTILS_H

#include <memory>
#include <string>

namespace Tucuxi {
namespace Common {

class Utils
{
public:
    static std::string getAppFolder(char** _argv);

    template<typename ... Args>
    static std::string strFormat(const std::string& _format, Args ... _args)
    {
        size_t size = std::snprintf(nullptr, 0, _format.c_str(), _args ...) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, _format.c_str(), _args ...);
        return std::string(buf.get(), buf.get() + size - 1);                    // We don't want the '\0' inside
    }
};

}
}

#endif // TUCUXI_TUCUCOMMON_UTILS_H