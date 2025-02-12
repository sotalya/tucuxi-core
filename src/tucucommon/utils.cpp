/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "utils.h"

namespace Tucuxi {
namespace Common {

using namespace Tucuxi::Core;
using Tucuxi::Core::Value;


std::string Utils::getAppFolder(char** _argv)
{
    std::string::size_type found = std::string(_argv[0]).find_last_of("/\\");
    if (found == std::string::npos) {
        return ".";
    }
    std::string appFolder = std::string(_argv[0]).substr(0, found);
    return appFolder;
}


int Utils::dateDiffInHours(const DateTime& _t1, const DateTime& _t2)
{
    DateTime t1;
    DateTime t2;

    if (_t1 > _t2) {
        t1 = _t1;
        t2 = _t2;
    }
    else {
        t1 = _t2;
        t2 = _t1;
    }
    return static_cast<int>((t1 - t2).toHours());
}


int Utils::dateDiffInDays(const DateTime& _t1, const DateTime& _t2)
{
    DateTime t1;
    DateTime t2;

    if (_t1 > _t2) {
        t1 = _t1;
        t2 = _t2;
    }
    else {
        t1 = _t2;
        t2 = _t1;
    }
    return static_cast<int>((t1 - t2).toDays());
}


int Utils::dateDiffInWeeks(const DateTime& _t1, const DateTime& _t2)
{
    DateTime t1;
    DateTime t2;

    if (_t1 > _t2) {
        t1 = _t1;
        t2 = _t2;
    }
    else {
        t1 = _t2;
        t2 = _t1;
    }
    return static_cast<int>((t1 - t2).toDays() / 7.0);
}


int Utils::dateDiffInMonths(const DateTime& _t1, const DateTime& _t2)
{
    DateTime t1;
    DateTime t2;

    if (_t1 > _t2) {
        t1 = _t1;
        t2 = _t2;
    }
    else {
        t1 = _t2;
        t2 = _t1;
    }
    return ((t1.year() - t2.year()) * 12 + (t1.month() - t2.month()) - ((t1.day() > t2.day()) ? 0 : 1));
}


int Utils::dateDiffInYears(const DateTime& _t1, const DateTime& _t2)
{
    DateTime t1;
    DateTime t2;

    if (_t1 > _t2) {
        t1 = _t1;
        t2 = _t2;
    }
    else {
        t1 = _t2;
        t2 = _t1;
    }
    return (t1.year() - t2.year() - ((t1.month() < t2.month() && (t1.year() > t2.year())) ? 1 : 0));
}


Value Utils::varToValue(const bool& _value)
{
    return _value ? 1.0 : 0.0;
}


Value Utils::varToValue(const DateTime& _value)
{
    return stringToValue(varToString(_value), DataType::Date);
}


Value Utils::stringToValue(const std::string& _str, const DataType& _dataType)
{
    Value v = 0.0;
    std::string str = _str;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    switch (_dataType) {
    case DataType::Int: {
        int tmp = std::stoi(str);
        v = tmp;
    } break;

    case DataType::Double:
        v = std::stod(str);
        break;

    case DataType::Bool: {
        bool is0 = false;
        try {
            double val = std::stod(str);
            if (val == 0.0) {
                is0 = true;
            }
        }
        catch (...) { // NOLINT(bugprone-empty-catch)
        }
        if (str == "0" || str == "false" || is0) {
            v = 0.0;
        }
        else {
            v = 1.0;
        }
    } break;

    case DataType::Date: {
        DateTime dt(str, "%Y-%m-%d %H:%M:%S");
        v = dt.toSeconds();
    } break;

    default:
        // Invalid type, set the value to 0.0.
        v = 0.0;
        break;
    }
    return v;
}


std::string Utils::varToString(const bool& _value)
{
    return _value ? "1" : "0";
}


std::string Utils::varToString(const int& _value)
{
    return std::to_string(_value);
}


std::string Utils::varToString(const double& _value)
{
    return std::to_string(_value);
}


std::string Utils::varToString(const DateTime& _value)
{
    std::array<char, 64> buf{};
    snprintf(
            buf.data(),
            buf.size(),
            "%04d-%02d-%02d %02d:%02d:%02d",
            _value.year(),
            _value.month(),
            _value.day(),
            _value.hour(),
            _value.minute(),
            _value.second());
    return std::string(buf.data());
}


DateTime Utils::ValueToDate(const Value& _value)
{
    DateTime dt = DateTime::fromDurationSinceEpoch(Duration(std::chrono::seconds(static_cast<int64>(_value))));

    return dt;
}

} // namespace Common
} // namespace Tucuxi
