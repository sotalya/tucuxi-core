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

}
}
