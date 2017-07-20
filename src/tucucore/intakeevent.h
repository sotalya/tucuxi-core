/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_INTAKEEVENT_H
#define TUCUXI_MATH_INTAKEEVENT_H

#include "tucucore/timedevent.h"

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

using Tucuxi::Common::DateTime;
using Tucuxi::Common::Duration;

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief A class reprensting the event of taking a dose.
/// Represents a Dose, as extracted from a DAL Dosage.
class IntakeEvent : public TimedEvent {
public:
    /// \brief The default constructor is not needed
    IntakeEvent() = delete;
    
    /// \brief Constructor
    /// \param _time Time of the dose intake.
    /// \param _offsetTime Number of hours since the first dose.
    /// \param _dose Quantity of drug administered.
    /// \param _interval Time before the next intake.
    /// \param _route Route of administration.
    /// \param _infusionTime Duration in case of an infusion.
    /// \param _nbPoints Number of points to compute for this intake.
    IntakeEvent(DateTime _time, Duration _offsetTime, Dose _dose, Duration _interval,
                RouteOfAdministration _route, Duration _infusionTime, int _nbPoints)
        : TimedEvent(_time), 
          m_dose(_dose),
          m_offsetTime(_offsetTime),
          m_nbPoints(_nbPoints),
          m_route(_route),
          m_interval(_interval),
          m_infusionTime(_infusionTime) {}

    /// \brief Destructor
    ~IntakeEvent() {}

    IntakeEvent(const IntakeEvent &other) = default;
    IntakeEvent& operator=(const IntakeEvent &other) = default;
    bool operator==(const IntakeEvent &other)
    {
        return (/*m_time == other.m_time &&*/
                m_dose == other.m_dose &&
                m_offsetTime == other.m_offsetTime &&
                m_nbPoints == other.m_nbPoints &&
                m_route == other.m_route &&
                m_interval == other.m_interval &&
                m_infusionTime == other.m_infusionTime);
    }

    void setInterval(Duration _interval) { m_interval = _interval; }     /// Change the interval value
    DeltaTime getInterval() const        { return m_interval.get<std::chrono::milliseconds>().count(); }          /// Get the interval value
    
    Dose getDose() const                 { return m_dose; }              /// Get the dose
    
    DeltaTime getOffsetTime() const      { return m_offsetTime.get<std::chrono::milliseconds>().count(); }        /// Get the time since the start of the treatment
    
    // The association with intakeintervalcalculator happens here
    // The intaketocalculatorassociator sets this value
    // void setCalc(IntakeIntervalCalculator& _calc) { calc = &_calc; }


private:
    /// The dose in mg
    Dose m_dose;
    /// Number of hours since the first dose
    Duration m_offsetTime;
    
    /// Number of points to compute for this intake
    int m_nbPoints;

    /// The route of administration
    RouteOfAdministration m_route;
    /// The time before the next intake
    Duration m_interval;
    /// The duration in case of an infusion
    Duration m_infusionTime;

    // IntakeIntervalCalculator* calc;
};

}
}

#endif // TUCUXI_MATH_INTAKEEVENT_H
