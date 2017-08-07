/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/threecompartmentintra.h"

namespace Tucuxi {
namespace Core {

ThreeCompartmentIntra::ThreeCompartmentIntra()
{
}

bool ThreeCompartmentIntra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
/*
    PRECOND(parameters.size() >= 6, SHOULDNTGONEGATIVE, "The number of parameters should be equal to 6.")
*/
    Value a0, a1, a2, p, q, r1, r2, phi;

    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_F = _parameters[1].getValue();
    m_Q1 = _parameters[2].getValue();
    m_Q2 = _parameters[3].getValue();
    m_V1 = _parameters[4].getValue();
    m_V2 = _parameters[5].getValue();
    m_Ke = m_Cl / m_V1;
    m_K12 = m_Q1 / m_V1;
    m_K21 = m_Q1 / m_V2;
    m_K13 = m_Q2 / m_V1;
    m_K31 = m_Q2 / m_V2;
    m_Tinf = (_intakeEvent.getInfusionTime()).toMilliseconds();
    m_Int = (_intakeEvent.getInterval()).toMilliseconds();
    m_NbPoints = _intakeEvent.getNumberPoints();

/*
    PRECONDCONT(m_D >= 0, SHOULDNTGONEGATIVE, "The dose is negative.")
    PRECONDCONT(!qIsNaN(m_D), NOTANUMBER, "The dose is NaN.")
    PRECONDCONT(!qIsInf(m_D), ISINFINITY, "The dose is Inf.")
    PRECONDCONT(m_Cl > 0, SHOULDNTGONEGATIVE, "The clearance is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_Cl), NOTANUMBER, "The CL is NaN.")
    PRECONDCONT(!qIsInf(m_Cl), ISINFINITY, "The CL is Inf.")
    PRECONDCONT(m_F > 0, SHOULDNTGONEGATIVE, "The F is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_F), NOTANUMBER, "The F is NaN.")
    PRECONDCONT(!qIsInf(m_F), ISINFINITY, "The F is Inf.")
    PRECONDCONT(m_Q1 > 0, SHOULDNTGONEGATIVE, "The Q1 is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_Q1), NOTANUMBER, "The Q1 is NaN.")
    PRECONDCONT(!qIsInf(m_Q1), ISINFINITY, "The Q1 is Inf.")
    PRECONDCONT(m_Q2 > 0, SHOULDNTGONEGATIVE, "The Q2 is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_Q2), NOTANUMBER, "The Q2 is NaN.")
    PRECONDCONT(!qIsInf(m_Q2), ISINFINITY, "The Q2 is Inf.")
    PRECONDCONT(m_V1 > 0, SHOULDNTGONEGATIVE, "The volume1 is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_V1), NOTANUMBER, "The V1 is NaN.")
    PRECONDCONT(!qIsInf(m_V1), ISINFINITY, "The V1 is Inf.")
    PRECONDCONT(m_V2 > 0, SHOULDNTGONEGATIVE, "The volume2 is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_V2), NOTANUMBER, "The V2 is NaN.")
    PRECONDCONT(!qIsInf(m_V2), ISINFINITY, "The V2 is Inf.")
    PRECONDCONT(m_Tinf > 0, SHOULDNTGONEGATIVE, "The infusion time is not greater than zero.")
    PRECONDCONT(!qIsNaN(m_Tinf), NOTANUMBER, "The infusion time is NaN.")
    PRECONDCONT(!qIsInf(m_Tinf), ISINFINITY, "The infusion time is Inf.")
*/

    a0 = m_Ke * m_K21 * m_K31;
    a1 = m_Ke * m_K31 + m_K21 * m_K31 + m_K21 * m_K13 + m_Ke * m_K21 + m_K31 * m_K12;
    a2 = m_Ke + m_K12 + m_K13 + m_K21 + m_K31;
    p = a1 - std::pow(a2,2) / 3;
    q = 2 * std::pow(a2,3) / 27 - a1 * a2 / 3 + a0;
    r1 = std::sqrt(-(std::pow(p,3) / 27));
    r2 = 2 * std::pow(r1,1/3);
    phi = std::acos(- q / (2 * r1)) / 3;

    m_Alpha = - (std::cos(phi) * r2 - a2 / 3);
    m_Beta = - (std::cos(phi + 2 * 3.1428 / 3) * r2 - a2/3);
    m_Gamma = - (std::cos(phi + 4 * 3.1428/3) * r2 - a2/3);

    return true;
}


void ThreeCompartmentIntra::prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters)
{
}


void ThreeCompartmentIntra::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Alpha"] = (-m_Alpha * _times).array().exp();
    m_precomputedLogarithms["Beta"] = (-m_Beta * _times).array().exp();
    m_precomputedLogarithms["Gamma"] = (-m_Gamma * _times).array().exp();
}


void ThreeCompartmentIntra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Value deltaD = (m_D / m_V1) / m_Tinf; 
    Value alphaTinf = std::exp(-m_Alpha* m_Tinf);
    Value betaTinf = std::exp(-m_Beta * m_Tinf);
    Value gammaTinf = std::exp(-m_Gamma * m_Tinf);

    Value A = (1 / m_V1) * (m_K21 - m_Alpha) * (m_K31 - m_Alpha) / (m_Alpha - m_Beta) / (m_Alpha - m_Gamma);
    Value B = (1 / m_V1) * (m_K21 - m_Beta) * (m_K31 - m_Beta) / (m_Beta - m_Alpha) / (m_Beta - m_Gamma);
    Value C = (1 / m_V1) * (m_K21 - m_Gamma) * (m_K31 - m_Gamma) / (m_Gamma - m_Beta) / (m_Gamma - m_Alpha);
    Value A2 = m_K12 / (m_K21 - m_Alpha) * A;
    Value B2 = m_K12 / (m_K21 - m_Beta) * B;
    Value C2 = m_K12 / (m_K21 - m_Gamma) * C;
    Value A3 = m_K13 / (m_K31 - m_Alpha) * A;
    Value B3 = m_K13 / (m_K31 - m_Beta) * B;
    Value C3 = m_K13 / (m_K31 - m_Gamma) * C;

    // Calculate concentrations for comp1, comp2 and comp3
    Eigen::VectorXd concentrations1;

    for (int t = 0; t < m_NbPoints; ++t) 
    {
       // Comare the point is outside of infusion time or not
       if ((t * (m_Int/m_NbPoints)) < m_Tinf)
       {
            concentrations1(t) = 
		deltaD 
		* (A/m_Alpha * (1 - m_precomputedLogarithms["Alpha"](t)) 
			+ B/m_Beta * (1 - m_precomputedLogarithms["Beta"](t)) 
			+ C/m_Gamma * (1 - m_precomputedLogarithms["Gamma"](t)));
       } 
       else 
       {
            concentrations1(t) = 
		deltaD 
		* (A/m_Alpha * (1 - alphaTinf) * m_precomputedLogarithms["Alpha"](t) / alphaTinf 
			+ B/m_Beta * (1 - betaTinf) * m_precomputedLogarithms["Beta"](t) / betaTinf 
			+ C/m_Gamma * (1 - gammaTinf) *    m_precomputedLogarithms["Gamma"](t) / gammaTinf);
       }
    }

    Value concentrations2 =
	deltaD * 
	(A2/m_Alpha * (1 - alphaTinf) * m_precomputedLogarithms["Alpha"](m_NbPoints-1) / alphaTinf 
		+ B2/m_Beta * (1 - betaTinf) * m_precomputedLogarithms["Beta"](m_NbPoints-1) / betaTinf 
		+ C2/m_Gamma * (1 - gammaTinf) * m_precomputedLogarithms["Gamma"](m_NbPoints-1) / gammaTinf);

    Value concentrations3 = 
	deltaD * 
	(A3/m_Alpha * (1 - alphaTinf) * m_precomputedLogarithms["Alpha"](m_NbPoints-1) / alphaTinf 
		+ B3/m_Beta * (1 - betaTinf) * m_precomputedLogarithms["Beta"](m_NbPoints-1) / betaTinf 
		+ C3/m_Gamma * (1 - gammaTinf) * m_precomputedLogarithms["Gamma"](m_NbPoints-1) / gammaTinf);

    // return concentrations of comp1, comp2 and comp3
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(concentrations3);
    //POSTCONDCONT(concentrations1[m_NbPoints - 1] >= 0, SHOULDNTGONEGATIVE, "The concentration1 is negative.")
    //POSTCONDCONT(concentrations2 >= 0, SHOULDNTGONEGATIVE, "The concentration2 is negative.")
    //POSTCONDCONT(concentrations3 >= 0, SHOULDNTGONEGATIVE, "The concentration3 is negative.")

    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	
}

}
}

