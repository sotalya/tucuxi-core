/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_DURATION_H
#define TUCUXI_TUCUCOMMON_DURATION_H

#include <chrono>
#include <iostream>

#include "basetypes.h"

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief A class to handle a duration.
/// The class is based on the std::chrono::duration<double> type representing a number of seconds
/// \sa DateTime, Duration
class Duration
{
public:
    /// Creates an empty duration (duration of 0)
    Duration();

    /// \brief Construct a duration from a std::chrono::hours
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in hours
    explicit Duration(const std::chrono::hours &_value);

    /// \brief Construct a duration from a std::chrono::minutes
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in minutes
    explicit Duration(const std::chrono::minutes &_value);

    /// \brief Construct a duration from a std::chrono::seconds
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in seconds
    explicit Duration(const std::chrono::seconds &_value);

    /// \brief Construct a duration from a std::chrono::milliseconds
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in milliseconds
    explicit Duration(const std::chrono::milliseconds &_value);

    /// \brief Construct a duration from a time expressed in hh, mm, ss
    /// \param _hours Number of hours.
    /// \param _minutes Number of minutes.
    /// \param _seconds Number of seconds.
    Duration(const std::chrono::hours &_hours,
             const std::chrono::minutes &_minutes,
             const std::chrono::seconds &_seconds);

    /// \brief Returns the duration in one of the std::chrono duration class (hours, minutes, seconds, ...)
    /// @return The duration in the specified T unit.
    template<class T> T get() const
    {
        return std::chrono::duration_cast<T>(m_duration);
    }

    /// \brief Is the duration empty?
    /// If the duration has not been set or if the duration equals to 0, returns true.
    /// @return True if the duration is empty
    bool isEmpty () const;

    /// \brief Checks if the duration is negative.
    /// @return Returns true if the duration is negative, otherwise returns false.
    ///
    bool isNegative() const;

    /// \brief Reset the duration to 0h00
    void clear();

    /// \brief Add two duration
    const Duration operator+(const Duration &_other) const;

    /// \brief Add the two and assign them
    Duration& operator+=(const Duration &_other);

    /// \brief Substract two duration
    /// If the duration to substract is smaller than the duration substracted, the resulting duration will be negative.
    const Duration operator-(const Duration &_other) const;

    /// \brief Substract the two and assign them
    Duration& operator-=(const Duration &_other);

    /// \brief Multiply the duration by this value
    const Duration operator*(double _factor) const;

    /// \brief Multiplication compound assignment overload.
    /// @param rhs Right-hand side integer.
    Duration& operator*=(double _factor);

    /// \brief Divide the duration by this value
    const Duration operator/(double _divider) const;

    /// \brief Divide the duration by this value
    Duration& operator/=(double _divider);

    /// \brief Divide the duration by this duration
    double operator/(const Duration &_other) const;

    /// \brief Obtain the remainder of dividing the duration by the specified one
    const Duration operator%(const Duration &_other) const;

    /// \brief Obtain the remainder of dividing the duration by the speicifed divider
    const Duration operator%(int _divider) const;

    /// \brief Is the duration smaller?
    bool operator<(const Duration &) const;

    /// \brief Is the duration bigger?
    bool operator>(const Duration &) const;

    /// \brief Is the duration bigger or equal?
    bool operator>=(const Duration &) const;

    /// \brief Is the duration smaller or equal?
    bool operator<=(const Duration &) const;

    /// \brief Are these durations equals?
    bool operator==(const Duration &) const;

    /// \brief Are these durations not equals?
    bool operator!=(const Duration &) const;

    /// \brief Retrieve the duration as a number of days
    /// @return The number of days reprensenting the duration.
    int64 toDays() const;

    /// \brief Retrieve the duration as a number of hours
    /// @return The number of hours reprensenting the duration.
    int64 toHours() const;

    /// \brief Retrieve the duration as a number of minutes
    /// @return The number of minutes reprensenting the duration.
    int64 toMinutes() const;

    /// \brief Retrieve the duration as a number of seconds
    /// @return The number of seconds reprensenting the duration.
    int64 toSeconds() const;

    /// \brief Retrieve the duration as a number of milliseconds
    /// @return The number of milliseconds reprensenting the duration.
    int64 toMilliseconds() const;

private:
    std::chrono::duration<double> m_duration;  /// The encapsulated duration
};

}
}

#endif // TUCUXI_TUCUCOMMON_DURATION_H
