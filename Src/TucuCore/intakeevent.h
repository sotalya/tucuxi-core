
#ifndef TUCUXI_MATH_INTAKEEVENT_H
#define TUCUXI_MATH_INTAKEEVENT_H

#include "timedevent.h"

namespace Tucuxi {
namespace Math {

//
// Represents a Dose, as extracted from a DAL Dosage
//
class IntakeEvent : public TimedEvent {
public:
	IntakeEvent() /*: calc(0)*/ {}
	IntakeEvent(time_t _time, double _offsetTime, double _value, double _interval, int _route, double _infusionTime, int _nbPoints)
		: TimedEvent(_time), 
		  m_offsetTime(_offsetTime), 
		  m_value(_value), 
		  m_nbPoints(_nbPoints), 
		  m_interval(_interval), 
		  m_route(_route), 
		  m_infusionTime(_infusionTime) /*, calc(0) */ {}
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
	
	void setInterval(Float _interval)	{ m_interval = _interval; }
	Float getInterval() const			{ return m_interval; }

	Float getValue() const				{ return m_value;  }

	Float getOffsetTime() const			{ return m_offsetTime;  }

	// The association with intakeintervalcalculator happens here
	// The intaketocalculatorassociator sets this value
	// void setCalc(IntakeIntervalCalculator& _calc) { calc = &_calc; }


private:
	// The numeric value of the number of hours since the first dose
	double m_offsetTime;
	double m_value;

	int m_nbPoints;

	double m_interval;
	int m_route;
	double m_infusionTime;

	// IntakeIntervalCalculator* calc;			
};

}
}

#endif // TUCUXI_MATH_INTAKEEVENT_H
