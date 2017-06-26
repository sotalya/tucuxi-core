#ifndef TUCUXI_MATH_INTAKEINTERVALCALCULATOR_H
#define TUCUXI_MATH_INTAKEINTERVALCALCULATOR_H

#include <map>

#include "Eigen/Dense"

#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/cachedlogarithms.h"

namespace Tucuxi {
namespace Math {

class IntakeIntervalCalculator
{
public:
	enum class Result {
		Ok,
		BadParameters,
		DensityError
	};

public:
	IntakeIntervalCalculator() {}

	/**
	* Calculate all points at the times in the supplied time container
	* Variable denisty is used by default, which means IntakeEvent
	* is not constant as the final density is stored there
	* Cornish Fisher cumulants calculated within
	* @param _concentrations Vector of concentrations - allocated within
	* @param _times Vector of times - allocated within
	* @param _intakeEvent Intake for the cycle (all cyles start with an intake)
	* @param _parameters Parameters for the cycle (all cycles have constant parameters)
	* @param _inResiduals Initial residual concentrations
	* @param _cycleSize The initial (if set to variable) number of points, always odd
	* @param _outResiduals Final residual concentrations
	* @param _isDensityConstant Flag to indicate if initial number of points should be used with a constant density
	*/
	virtual Result calculateIntakePoints(
		Concentrations& _concentrations,
		Times & _times,
		IntakeEvent& _intakeEvent,
		const ParameterList& _parameters,
		const Residuals& _inResiduals,
		const CycleSize _cycleSize,
		Residuals& _outResiduals,
		const bool _isDensityConstant);

	/**
	* Calculates one single point and initial and final residuals
	* @param _concentrations vector of concentrations. allocated within
	* @param _intakeEvent intake for the cycle (all cyles start with an intake)
	* @param _parameters Parameters for the cycle (all cycles have constant parameters)
	* @param _inResiduals Initial residual concentrations
	* @param _atTime The time of the point of interest
	* @param _outResiduals Final residual concentrations
	*/
	virtual Result calculateIntakeSinglePoint(
		Concentrations& _concentrations,
		const IntakeEvent& _intakeEvent,
		const ParameterList& _parameters,
		const Residuals& _inResiduals,
		const Time _atTime,
		Residuals& _outResiduals);

protected:
	virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) = 0;
	virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) = 0;
	virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) = 0;
	virtual void computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) = 0;

protected:
	PrecomputedLogarithms m_precomputedLogarithms;

private:
	CachedLogarithms m_cache;
};

}
}

#endif // TUCUXI_MATH_INTAKEINTERVALCALCULATOR_H
