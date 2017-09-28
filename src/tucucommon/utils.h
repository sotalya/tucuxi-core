/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_UTILS_H
#define TUCUXI_TUCUCOMMON_UTILS_H

#include <memory>
#include <string>

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Common {

using Tucuxi::Core::Value;
using Tucuxi::Core::DataType;

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

    /// \brief Convert a std::string to a Value.
    /// \param _str String to convert.
    /// \param _type Type of the value given in the string.
    /// \return Value contained in the string.
    /// \pre _str contains a string that can be converted to a Value.
    static Value stringToValue(std::string _str,
                               const DataType &_dataType = DataType::Double);
};

}
}

#endif // TUCUXI_TUCUCOMMON_UTILS_H
