#ifndef TUCUXI_MATH_TIMEDEVENT_H
#define TUCUXI_MATH_TIMEDEVENT_H

namespace Tucuxi {
namespace Math {

class TimedEvent
{
public:
	TimedEvent() {}
	TimedEvent(time_t _time) : time(_time) {}
private:
	time_t time;
};

}
}

#endif // TUCUXI_MATH_TIMEDEVENT_H