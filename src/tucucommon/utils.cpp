/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "utils.h"

namespace Tucuxi {
namespace Common {

using namespace Tucuxi::Core;
using Tucuxi::Core::Value;

std::string Utils::getAppFolder(char** _argv)
{
    std::string::size_type found = std::string(_argv[0]).find_last_of("/\\");
    std::string appFolder = std::string(_argv[0]).substr(0, found);
    return appFolder;
}

Value Utils::stringToValue(std::string _str, const DataType &_dataType)
{
    Value v;
    std::transform(_str.begin(), _str.end(), _str.begin(), ::tolower);
    switch (_dataType)
    {
    case DataType::Int:
    {
        int tmp = std::stoi(_str);
        v = tmp;
    }
        break;

    case DataType::Double:
        v = std::stod(_str);
        break;

    case DataType::Bool:
        if (_str == "0" || _str == "false") {
            v = 0.0;
        } else {
            v = 1.0;
        }
        break;

    case DataType::Date:
    {
        DateTime dt(_str, "%Y-%b-%d %H:%M:%S");
        v = dt.toSeconds();
    }
        break;

    default:
        // Invalid type, set the value to 0.0.
        v = 0.0;
        break;
    }
    return v;
}


std::string Utils::valueToString(const bool &_value)
{
    return _value == true ? "1" : "0";
}


std::string Utils::valueToString(const int &_value)
{
    return std::to_string(_value);
}


std::string Utils::valueToString(const double &_value)
{
    return std::to_string(_value);
}


std::string Utils::valueToString(const DateTime &_value)
{
    char buf[64];
    sprintf(buf, "%4d-%2d-%2d %2d:%2d:%2d",
            _value.year(), _value.month(), _value.day(),
            _value.hour(), _value.minute(), (int)_value.second());
    return std::string(buf);
}


DateTime Utils::ValueToDate(const Value &_value)
{
    DateTime dt;
    dt += Duration(std::chrono::seconds((long int)_value));
    return dt;
}

}
}
