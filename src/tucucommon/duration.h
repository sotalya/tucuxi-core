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


#ifndef TUCUXI_TUCUCOMMON_DURATION_H
#define TUCUXI_TUCUCOMMON_DURATION_H

#include <chrono>
#include <iostream>

#include "basetypes.h"

namespace Tucuxi {
namespace Common {

using days = std::chrono::duration<int, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;
using years = std::chrono::duration<int, std::ratio_multiply<std::ratio<146097, 400>, days::period>>;
using months = std::chrono::duration<int, std::ratio_divide<years::period, std::ratio<12>>>;

/// \ingroup TucuCommon
/// \brief A class to handle a duration.
/// The class is based on the std::chrono::duration<double> type representing a number of seconds
/// \sa DateTime, Duration
class Duration
{
public:
    /// Creates an empty duration (duration of 0)
    Duration();

    /// \brief Construct a duration from a std::chrono::years
    /// This also provide implicit conversions when using all the other methods.
    /// \param _value Duration in years
    Duration(const years& _value);

    /// \brief Construct a duration from a std::chrono::months
    /// This also provide implicit conversions when using all the other methods.
    /// \param _value Duration in months
    Duration(const months& _value);

    /// \brief Construct a duration from a std::chrono::days
    /// This also provide implicit conversions when using all the other methods.
    /// \param _value Duration in days
    Duration(const days& _value);

    /// \brief Construct a duration from a std::chrono::hours
    /// This also provide implicit conversions when using all the other methods.
    /// \param _value Duration in hours
    Duration(const std::chrono::hours& _value);

    /// \brief Construct a duration from a std::chrono::minutes
    /// This also provide implicit conversions when using all the other methods.
    /// \param _value Duration in minutes
    Duration(const std::chrono::minutes& _value);

    /// \brief Construct a duration from a std::chrono::seconds
    /// This also provide implicit conversions when using all the other methods.
    /// \param _value Duration in seconds
    Duration(const std::chrono::seconds& _value);

    /// \brief Construct a duration from a std::chrono::milliseconds
    /// This also provide implicit conversions when using all the other methods.
    /// \param _value Duration in milliseconds
    Duration(const std::chrono::milliseconds& _value);

    /// \brief Construct a duration from a time expressed in hh, mm, ss
    /// \param _hours Number of hours.
    /// \param _minutes Number of minutes.
    /// \param _seconds Number of seconds.
    Duration(
            const std::chrono::hours& _hours,
            const std::chrono::minutes& _minutes,
            const std::chrono::seconds& _seconds);

    /// \brief Is the duration empty?
    /// If the duration has not been set or if the duration equals to 0, returns true.
    /// \return True if the duration is empty
    bool isEmpty() const;

    /// \brief Checks if the duration is negative.
    /// \return Returns true if the duration is negative, otherwise returns false.
    ///
    bool isNegative() const;

    /// \brief Reset the duration to 0h00
    void clear();

    /// \brief Add two duration
    Duration operator+(const Duration& _other) const;

    /// \brief Add the two and assign them
    Duration& operator+=(const Duration& _other);

    /// \brief Substract two duration
    /// If the duration to substract is smaller than the duration substracted, the resulting duration will be negative.
    Duration operator-(const Duration& _other) const;

    /// \brief Substract the two and assign them
    Duration& operator-=(const Duration& _other);

    /// \brief Multiply the duration by this value
    Duration operator*(double _factor) const;

    /// \brief Multiply the duration by this value, considering a convertion to double is valid
    template<typename T>
    Duration operator*(T _factor) const
    {
        return (*this) * static_cast<double>(_factor);
    }

    /// \brief Multiplication compound assignment overload.
    /// \param rhs Right-hand side integer.
    Duration& operator*=(double _factor);

    /// \brief Divide the duration by this value
    Duration operator/(double _divider) const;

    /// \brief Divide the duration by this value
    Duration& operator/=(double _divider);

    /// \brief Divide the duration by this duration
    double operator/(const Duration& _other) const;

    /// \brief Obtain the remainder of dividing the duration by the specified one
    Duration operator%(const Duration& _other) const;

    /// \brief Obtain the remainder of dividing the duration by the speicifed divider
    Duration operator%(int _divider) const;

    /// \brief Is the duration smaller?
    bool operator<(const Duration&) const;

    /// \brief Is the duration bigger?
    bool operator>(const Duration&) const;

    /// \brief Is the duration bigger or equal?
    bool operator>=(const Duration&) const;

    /// \brief Is the duration smaller or equal?
    bool operator<=(const Duration&) const;

    /// \brief Are these durations equals?
    bool operator==(const Duration&) const;

    /// \brief Are these durations not equals?
    bool operator!=(const Duration&) const;

    /// \brief Retrieve the duration as a number of yeras
    /// \return The number of years reprensenting the duration.
    int64 toYears() const;

    /// \brief Retrieve the duration as a number of months
    /// \return The number of months reprensenting the duration.
    int64 toMonths() const;

    /// \brief Retrieve the duration as a number of days
    /// \return The number of days reprensenting the duration.
    double toDays() const;

    /// \brief Retrieve the duration as a number of hours
    /// \return The number of hours reprensenting the duration.
    double toHours() const;

    /// \brief Retrieve the duration as a number of minutes
    /// \return The number of minutes reprensenting the duration.
    double toMinutes() const;

    /// \brief Retrieve the duration as a number of seconds
    /// \return The number of seconds reprensenting the duration.
    double toSeconds() const;

    /// \brief Retrieve the duration as a number of milliseconds
    /// \return The number of milliseconds reprensenting the duration.
    int64 toMilliseconds() const;

    /// \brief Class' output operator.
    /// \param _output Output stream.
    /// \param _dt Self reference to the Duration to print.
    /// \return Output stream given as input (for output chaining).
    friend std::ostream& operator<<(std::ostream& _output, const Duration& _dt)
    {
        _output << "Years: " << _dt.toYears() << ";\n"
                << "Months: " << _dt.toMonths() << ";\n"
                << "Days: " << _dt.toDays() << ";\n"
                << "Hours: " << _dt.toHours() << ";\n"
                << "Minutes: " << _dt.toMinutes() << ";\n"
                << "Seconds: " << _dt.toSeconds() << ".\n";
        return _output;
    }

private:
    /// \brief Returns the duration in one of the std::chrono duration class (hours, minutes, seconds, ...)
    /// \return The duration in the specified T unit.
    template<class T>
    T get() const
    {
        return std::chrono::duration_cast<T>(m_duration);
    }

    std::chrono::duration<ChronoBaseType> m_duration; /// The encapsulated duration

#ifdef EASY_DEBUG
    std::string m_durationString;
    void updateDurationString();
#endif // EASY_DEBUG
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_DURATION_H
