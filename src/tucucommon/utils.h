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


#ifndef TUCUXI_TUCUCOMMON_UTILS_H
#define TUCUXI_TUCUCOMMON_UTILS_H

#include <array>
#include <memory>
#include <string>

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

#include "tucucore/definitions.h"

#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#else
#include <stdio.h> //sprintf
#endif


namespace Tucuxi {
namespace Common {

namespace Utils {

using Tucuxi::Core::Value;

//public:

/// \brief Convert a std::string to a Value.
/// \param _str String to convert.
/// \param _type Type of the value given in the string.
/// \return Value contained in the string.
/// \pre _str contains a string that can be converted to a Value.
Value stringToValue(const std::string& _str, const Tucuxi::Core::DataType& _dataType);

/// \brief Convert a boolean to a Value.
/// \param _value Input boolean to convert.
/// \return Boolean converted into a value (that is, either 0 or 1).
Value varToValue(const bool& _value);

/// \brief Convert a date to a Value.
/// \param _value Input date to convert.
/// \return Date converted into a value (that is, corresponding number of seconds).
Value varToValue(const DateTime& _value);

/// \brief Convert a boolean variable to a string.
/// \param _value Variable to convert to a string.
/// \return String describing the variable.
std::string varToString(const bool& _value);

/// \brief Convert an integer variable to a string.
/// \param _value Variable to convert to a string.
/// \return String describing the variable.
std::string varToString(const int& _value);

/// \brief Convert a floating point variable to a string.
/// \param _value Variable to convert to a string.
/// \return String describing the variable.
std::string varToString(const double& _value);

/// \brief Convert a date variable to a string.
/// \param _value Variable to convert to a string.
/// \return String describing the variable.
std::string varToString(const DateTime& _value);

/// \brief Convert a Value variable -- containing a date expressed in seconds -- into a "true" date.
/// \param _value Value variable to convert to date.
/// \return Equivalent date.
DateTime ValueToDate(const Value& _value);

/// \brief Express the difference between two dates in hours.
/// \param _t1 First date in the difference.
/// \param _t2 Second date in the difference.
/// \note The absolute value of the difference is returned!
/// \return Number of hours by which the two dates differ.
/// Be careful, the returned value is an int, and as such is floored.
int dateDiffInHours(const DateTime& _t1, const DateTime& _t2);

/// \brief Express the difference between two dates in days.
/// \param _t1 First date in the difference.
/// \param _t2 Second date in the difference.
/// \note The absolute value of the difference is returned!
/// \return Number of days by which the two dates differ.
/// Be careful, the returned value is an int, and as such is floored.
int dateDiffInDays(const DateTime& _t1, const DateTime& _t2);

/// \brief Express the difference between two dates in weeks.
/// \param _t1 First date in the difference.
/// \param _t2 Second date in the difference.
/// \note The absolute value of the difference is returned!
/// \return Number of weeks by which the two dates differ.
/// Be careful, the returned value is an int, and as such is floored.
int dateDiffInWeeks(const DateTime& _t1, const DateTime& _t2);

/// \brief Express the difference between two dates in months.
/// \param _t1 First date in the difference.
/// \param _t2 Second date in the difference.
/// \note The absolute value of the difference is returned!
/// \return Number of months by which the two dates differ.
/// Be careful, the returned value is an int, and as such is floored.
int dateDiffInMonths(const DateTime& _t1, const DateTime& _t2);

/// \brief Express the difference between two dates in years.
/// \param _t1 First date in the difference.
/// \param _t2 Second date in the difference.
/// \note The absolute value of the difference is returned!
/// \return Number of years by which the two dates differ.
/// Be careful, the returned value is an int, and as such is floored.
int dateDiffInYears(const DateTime& _t1, const DateTime& _t2);

std::string getAppFolder(char** _argv);

template<typename... Args>
std::string strFormat(const std::string& _format, Args... _args)
{
    size_t size = static_cast<size_t>(snprintf(nullptr, 0, _format.c_str(), _args...) + 1); // Extra space for '\0'
    //    std::vector<char> buf(size);
    //    snprintf(buf.data(), size, _format.c_str(), _args...);
    //    return std::string(buf.data(), buf.data() + size - 1);
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, _format.c_str(), _args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

} // namespace Utils
} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_UTILS_H
