/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_DURATION_H
#define TUCUXI_TUCUCOMMON_DURATION_H

#include <chrono>

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief A class to handle a duration.
/// The class is based on the std::chrono::duration<float> type representing a number of seconds
/// \sa DateTime, Duration
class Duration
{
public:
    /// Creates an empty duration (duration of 0)
    Duration();

    /// \brief Construct a duration from a std::chrono::hours
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in hours
    Duration(const std::chrono::hours &_value);

    /// \brief Construct a duration from a std::chrono::minutes
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in minutes
    Duration(const std::chrono::minutes _value);

    /// \brief Construct a duration from a std::chrono::seconds
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in seconds
    Duration(const std::chrono::seconds d);

    /// \brief Construct a duration from a std::chrono::milliseconds
    /// This also provide implicit conversions when using all the other methods.
    /// @param _value Duration in milliseconds
    Duration(const std::chrono::milliseconds d);

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
    const Duration operator*(float _factor) const;

    /// \brief Multiplication compound assignment overload.
    /// @param rhs Right-hand side integer.
    Duration& operator*=(float _factor);

    /// \brief Divide the duration by this value
    const Duration operator/(float _divider) const;

    /// \brief Divide the duration by this value
    Duration& operator/=(float _divider);

    /// \brief Divide the duration by this duration
    float operator/(const Duration &_other) const;

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

private:
    std::chrono::duration<float> m_duration;  /// The encapsulated duration
};

}
}

#endif // TUCUXI_TUCUCOMMON_DURATION_H
