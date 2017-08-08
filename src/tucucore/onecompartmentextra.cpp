/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/onecompartmentextra.h"

namespace Tucuxi {
namespace Core {

OneCompartmentExtra::OneCompartmentExtra()
{
}

bool OneCompartmentExtra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
//	PRECOND(parameters.size() >= 4, SHOULDNTGONEGATIVE, "The number of parameters should be equal to 4.")

    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_F = _parameters[1].getValue();
    m_Ka = _parameters[2].getValue();
    m_V = _parameters[3].getValue();
    m_Ke = m_Cl / m_V;
    m_NbPoints = _intakeEvent.getNumberPoints();
/*
    PRECONDCONT(D >= 0, SHOULDNTGONEGATIVE, "The dose is negative.")
    PRECONDCONT(!qIsNaN(D), NOTANUMBER, "The dose is NaN.")
    PRECONDCONT(!qIsInf(D), ISINFINITY, "The dose is Inf.")
    PRECONDCONT(V > 0, SHOULDNTGONEGATIVE, "The volume is not greater than zero.")
    PRECONDCONT(!qIsNaN(V), NOTANUMBER, "The V is NaN.")
    PRECONDCONT(!qIsInf(V), ISINFINITY, "The V is Inf.")
    PRECONDCONT(F > 0, SHOULDNTGONEGATIVE, "The volume is not greater than zero.")
    PRECONDCONT(!qIsNaN(F), NOTANUMBER, "The F is NaN.")
    PRECONDCONT(!qIsInf(F), ISINFINITY, "The F is Inf.")
    PRECONDCONT(Ke > 0, SHOULDNTGONEGATIVE, "The clearance is not greater than zero.")
    PRECONDCONT(!qIsNaN(Ke), NOTANUMBER, "The CL is NaN.")
    PRECONDCONT(!qIsInf(Ke), ISINFINITY, "The CL is Inf.")
    PRECONDCONT(Ka > 0, SHOULDNTGONEGATIVE, "The clearance is not greater than zero.")
    PRECONDCONT(!qIsNaN(Ka), NOTANUMBER, "The Ka is NaN.")
    PRECONDCONT(!qIsInf(Ka), ISINFINITY, "The Ka is Inf.")
*/
    return true;
}


void OneCompartmentExtra::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}


void OneCompartmentExtra::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ka"] = (-m_Ka * _times).array().exp();
    m_precomputedLogarithms["Ke"] = (-m_Ke * _times).array().exp();
}


void OneCompartmentExtra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1] + m_F*m_D/m_V;
    Concentration part2 = m_Ka*resid2 / (-m_Ka + m_Ke);

    Eigen::VectorXd concentrations = 
        m_precomputedLogarithms["Ke"] * resid1 
	+ (m_precomputedLogarithms["Ka"] - m_precomputedLogarithms["Ke"]) * part2;
    
    _outResiduals.push_back(concentrations[m_NbPoints - 1]);
    // POSTCONDCONT(finalResiduals[0] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    _outResiduals.push_back(resid2 * m_precomputedLogarithms["Ka"][m_NbPoints - 1]);
    // POSTCONDCONT(finalResiduals[1] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    _concentrations.assign(concentrations.data(), concentrations.data() + concentrations.size());	
}

}
}
