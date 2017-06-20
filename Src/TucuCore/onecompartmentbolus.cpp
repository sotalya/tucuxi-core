
#include <Eigen/Dense>

#include "onecompartmentbolus.h"

namespace Tucuxi {
namespace Math {

OneCompartmentBolus::OneCompartmentBolus()
{
}

bool OneCompartmentBolus::checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
/*
	PRECOND(parameters.size() >= 2, SHOULDNTGONEGATIVE, "The number of parameters should be equal to 2.")
*/
	m_D = _intakeEvent.getValue() * 1000;
	m_V = _parameters[0].getValue();
	m_Ke = _parameters[1].getValue();
/*
	PRECONDCONT(D >= 0, SHOULDNTGONEGATIVE, "The dose is negative.")
	PRECONDCONT(!qIsNaN(D), NOTANUMBER, "The dose is NaN.")
	PRECONDCONT(!qIsInf(D), ISINFINITY, "The dose is Inf.")
	PRECONDCONT(V > 0, SHOULDNTGONEGATIVE, "The volume is not greater than zero.")
	PRECONDCONT(!qIsNaN(V), NOTANUMBER, "The V is NaN.")
	PRECONDCONT(!qIsInf(V), ISINFINITY, "The V is Inf.")
	PRECONDCONT(Ke > 0, SHOULDNTGONEGATIVE, "The clearance is not greater than zero.")
	PRECONDCONT(!qIsNaN(Ke), NOTANUMBER, "The CL is NaN.")
	PRECONDCONT(!qIsInf(Ke), ISINFINITY, "The CL is Inf.")
*/
	return true;
}

void OneCompartmentBolus::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}

void OneCompartmentBolus::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{	
	m_precomputedLogarithms["Ke"] = (m_Ke * _times).array().exp();
}

void OneCompartmentBolus::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
	Eigen::VectorXd concentrations = (m_D / m_V + _inResiduals[0]) * m_precomputedLogarithms["Ke"];
	_outResiduals.push_back(concentrations[concentrations.size() - 1]);
	//POSTCONDCONT(finalResiduals[0] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
	_concentrations.assign(concentrations.data(), concentrations.data() + concentrations.size());	
}

}
}