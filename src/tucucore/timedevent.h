#ifndef TUCUXI_MATH_TIMEDEVENT_H
#define TUCUXI_MATH_TIMEDEVENT_H

namespace Tucuxi {
namespace Core {


/// \ingroup TucuCore
/// \brief Base class for all types of "events"
/// An event is a change in a value that influences the computation of prediction. It could be 
/// a new intake, a new measured concentration, a change of a covariate, ...
class TimedEvent
{
public:
    /// \brief Constructor, "deleted" because not necessary
    TimedEvent() = delete; // {}
    
    /// \brief Constructor defining the date of the event
    TimedEvent(time_t _time) : time(_time) {}

private:
    time_t time;    /// The date of the event
};

}
}

#endif // TUCUXI_MATH_TIMEDEVENT_H