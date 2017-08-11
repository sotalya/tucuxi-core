/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_INTAKEEVENT_H
#define TUCUXI_CORE_INTAKEEVENT_H

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

#include "tucucore/timedevent.h"
#include "tucucore/intakeintervalcalculator.h"

using Tucuxi::Common::DateTime;
using Tucuxi::Common::Duration;

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief A class reprensting the event of taking a dose.
/// Represents a Dose, as extracted from a DAL Dosage.
class IntakeEvent : public TimedEvent 
{
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

    /// \brief Default copy constructor.
    /// \param _other Source object to copy from.
    IntakeEvent(const IntakeEvent &other) = default;

    /// \brief Default assignment operator.
    /// \param _other Source object to copy from.
    IntakeEvent& operator=(const IntakeEvent &other) = default;

    /// \brief Comparison operator.
    /// \param _other Object to compare to.
    /// \return true if the two objects are equal, false otherwise.
    bool operator==(const IntakeEvent &other) const
    {
        return (m_time == other.m_time &&
                m_dose == other.m_dose &&
                m_offsetTime == other.m_offsetTime &&
                m_nbPoints == other.m_nbPoints &&
                m_route == other.m_route &&
                m_interval == other.m_interval &&
                m_infusionTime == other.m_infusionTime);
    }

    /// \brief Change the time before the next intake.
    /// \param _interval New interval size.
    void setInterval(Duration _interval)
    {
        m_interval = _interval;
    }

    /// \brief Get the interval value in milliseconds.
    /// \return Interval value in ms.
    Duration getInterval() const
    {
        return m_interval;
    }

    /// \brief Get the number of points to compute for this intake
    /// \return Number of points to compute for this intake
    int getNbPoints() const
    {
        return m_nbPoints;
    }
    
    /// \brief Get the dose.
    /// \return Dose value.
    Dose getDose() const
    {
        return m_dose;
    }
    
    /// \brief Get the time (in milliseconds) since the start of the treatment.
    /// \return Time in ms since the beginning of the treatment.
    Duration getOffsetTime() const
    {
        return m_offsetTime;
    }
    
    /// \brief Get the route of administration of the treatment.
    /// \return Route of administration.
    RouteOfAdministration getRoute() const
    {
        return m_route;
    }

    /// \brief Get the infusion time (in milliseconds).
    /// \return Infusion time (in ms).
    Duration getInfusionTime() const
    {
        return m_infusionTime;
    }

    /// \brief Compare two intakes, sorting them according to intake time.
    /// \param _other The intake to compare to.
    /// \return True if the current IntakeEvent is relative to an intake that precedes the one is compared to, false
    /// otherwise.
    bool operator<(const IntakeEvent& _other) const
    {
        return m_time < _other.m_time;
    }

    IntakeIntervalCalculator::Result calculateIntakePoints(
        Concentrations& _concentrations,
        TimeOffsets & _times,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        const CycleSize _cycleSize,
        Residuals& _outResiduals,
        const bool _isDensityConstant) const
    {
        return m_calculator->calculateIntakePoints(_concentrations, _times, _intakeEvent, _parameters, _inResiduals, _cycleSize, _outResiduals, _isDensityConstant);
    }


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

    IntakeIntervalCalculator* m_calculator;
};

}
}

#endif // TUCUXI_CORE_INTAKEEVENT_H
