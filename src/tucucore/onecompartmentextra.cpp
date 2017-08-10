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
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_F = _parameters[1].getValue();
    m_Ka = _parameters[2].getValue();
    m_V = _parameters[3].getValue();
    m_Ke = m_Cl / m_V;
    m_NbPoints = _intakeEvent.getNumberPoints();

    // check the inputs
    bOK &= checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V), "The m_V is NaN.");
    bOK &= checkValue(!std::isinf(m_V), "The _V is Inf.");
    bOK &= checkValue(m_F > 0, "The volume is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_Ke > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The CL is Inf.");
    bOK &= checkValue(m_Ka > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The m_Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The m_Ka is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue((_intakeEvent.getInterval()).toMilliseconds() >= 0, "The interval time is zero or negative.");

    return bOK;
}


void OneCompartmentExtra::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}


void OneCompartmentExtra::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ka"] = (-m_Ka * _times).array().exp();
    m_precomputedLogarithms["Ke"] = (-m_Ke * _times).array().exp();
}


bool OneCompartmentExtra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    bool bOK = true;

    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1] + m_F*m_D/m_V;
    Concentration part2 = m_Ka*resid2 / (-m_Ka + m_Ke);

    Eigen::VectorXd concentrations1 = 
        m_precomputedLogarithms["Ke"] * resid1 
	+ (m_precomputedLogarithms["Ka"] - m_precomputedLogarithms["Ke"]) * part2;
    
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(resid2 * m_precomputedLogarithms["Ka"][m_NbPoints - 1]);
    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bOK &= checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");

    return bOK;
}

bool OneCompartmentExtra::computeConcentration(const int64& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    bool bOK = true;

    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1] + m_F*m_D/m_V;
    Concentration part2 = m_Ka*resid2 / (-m_Ka + m_Ke);

    // Calcuate concentrations
    Eigen::VectorXd concentrations1 = 
        m_precomputedLogarithms["Ke"] * resid1 
	+ (m_precomputedLogarithms["Ka"] - m_precomputedLogarithms["Ke"]) * part2;
    // TODO check: why the equation is different from multiple points
    Eigen::VectorXd concentrations2;
#if 0
    Eigen::VectorXd concentrations2 = 
        m_F * m_D / m_V * m_precomputedLogarithms["Ka"] 
	/ (1 - m_precomputedLogarithms["Ka"]);
    Eigen::VectorXd concentrations2 = 
        m_F * m_D / m_V * m_precomputedLogarithms["Ka"] 
	/ (std::for_each(
		m_precomputedLogarithms["Ka"].data(), 
		m_precomputedLogarithms["Ka"].data() + m_precomputedLogarithms["Ka"].size(), 
		[](Value& a) { a = 1 - a; }));
#endif

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations1[0]);
    _concentrations.push_back(concentrations2[0]);

    // return final residual (computation with m_Int (interval))
    _outResiduals.push_back(concentrations1[1]);
    _outResiduals.push_back(concentrations2[1]);

    bOK &= checkValue(_outResiduals[0] >= 0, "The final residual1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The final residual2 is negative.");
    bOK &= checkValue(_concentrations[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_concentrations[1] >= 0, "The concentration2 is negative.");

    return bOK;
}

}
}
