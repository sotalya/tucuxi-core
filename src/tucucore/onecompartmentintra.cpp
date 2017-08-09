/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>
#include <math.h>
#include <algorithm>

#include "tucucore/onecompartmentintra.h"

namespace Tucuxi {
namespace Core {

OneCompartmentIntra::OneCompartmentIntra()
{
}

bool OneCompartmentIntra::checkInputs(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
//	PRECOND(parameters.size() >= 4, SHOULDNTGONEGATIVE, "The number of parameters should be equal to 4.")

    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_V = _parameters[1].getValue();
    m_Ke = m_Cl / m_V;
    m_Tinf = (_intakeEvent.getInfusionTime()).toMilliseconds();
    m_Int = (_intakeEvent.getInterval()).toMilliseconds();
    m_NbPoints = _intakeEvent.getNumberPoints();
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
    PRECOND(m_Tinf >= 0, SHOULDNTGONEGATIVE, "The infusion time is zero or negative.")
    PRECONDCONT(!qIsNaN(m_Tinf), NOTANUMBER, "The m_Tinf is NaN.")
    PRECONDCONT(!qIsInf(m_Tinf), ISINFINITY, "The m_Tinf is Inf.")

*/
    return true;
}


void OneCompartmentIntra::prepareComputations(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
}


void OneCompartmentIntra::computeLogarithms(const IntakeEvent& _intakeEvent, const Parameters& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Ke"] = (-m_Ke * _times).array().exp();
}


bool OneCompartmentIntra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Concentration part1 = m_D/(m_Tinf*m_Cl);
    size_t ke_size = m_precomputedLogarithms["Ke"].size();
    int forcesize = std::min(ceil(m_Tinf/m_Int * ke_size), ceil(ke_size));
    int therest;

    // Calcaulate concentrations
    Eigen::VectorXd concentrations = Eigen::VectorXd::Constant(ke_size, _inResiduals[0]);
    concentrations = concentrations.cwiseProduct(m_precomputedLogarithms["Ke"]);

    concentrations.head(forcesize) = 
        concentrations.head(forcesize) 
	+ part1 * (1.0 - m_precomputedLogarithms["Ke"].head(forcesize).array()).matrix();
    
    therest = concentrations.size() - forcesize;
    concentrations.tail(therest) = 
        concentrations.tail(therest) 
	+ part1 * (exp(m_Ke * m_Tinf) - 1) * m_precomputedLogarithms["Ke"].tail(therest);

    // Set the new residual
    _outResiduals.push_back(concentrations[m_NbPoints - 1]);
    // POSTCONDCONT(finalResiduals[0] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
    _concentrations.assign(concentrations.data(), concentrations.data() + concentrations.size());	

    return true;
}

}
}
