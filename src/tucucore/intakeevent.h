/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_INTAKEEVENT_H
#define TUCUXI_MATH_INTAKEEVENT_H

#include "tucucore/timedevent.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief A class reprensting the event of taking a dose.
/// Represents a Dose, as extracted from a DAL Dosage
class IntakeEvent : public TimedEvent {
public:
    /// \brief The default constructor is not needed
    IntakeEvent() = delete;
    
    /// \brief Constructor
    IntakeEvent(time_t _time, DeltaTime _offsetTime, Dose _dose, DeltaTime _interval, int _route, DeltaTime _infusionTime, int _nbPoints)
        : TimedEvent(_time), 
          m_offsetTime(_offsetTime), 
          m_dose(_dose), 
          m_nbPoints(_nbPoints), 
          m_interval(_interval), 
          m_route(_route), 
          m_infusionTime(_infusionTime) /*, calc(0) */ {}

    /// \brief Destructor
    ~IntakeEvent() {}
        
/*
    // As a convention with boost multi-index, modifications of
    // values should occur through a functor like this
    //
    struct change_density {
        change_density(const int _nbp) :new_density(_nbp) {}
        void operator() (IntakeEvent& i) { i.nbPoints = new_density; }
    private:
        int new_density;
    };
*/
    
    void setInterval(DeltaTime _interval)   { m_interval = _interval; }     /// Change the interval value
    DeltaTime getInterval() const           { return m_interval; }          /// Get the interval value
    
    Value getDose() const                   { return m_dose; }              /// Get the dose
    
    DeltaTime getOffsetTime() const         { return m_offsetTime; }        /// Get the time since the start of the treatment
    
    // The association with intakeintervalcalculator happens here
    // The intaketocalculatorassociator sets this value
    // void setCalc(IntakeIntervalCalculator& _calc) { calc = &_calc; }


private:
    Dose m_dose;                /// The dose in mg
    DeltaTime m_offsetTime;     /// Number of hours since the first dose
    
    int m_nbPoints;             /// Number of points to compute for this intake

    int m_route;                /// The route of administration
    DeltaTime m_interval;       /// The time before the next intake
    DeltaTime m_infusionTime;   /// The duration in case of an infusion

    // IntakeIntervalCalculator* calc;
};

}
}

#endif // TUCUXI_MATH_INTAKEEVENT_H
