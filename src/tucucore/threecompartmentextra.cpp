/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/threecompartmentextra.h"

namespace Tucuxi {
namespace Core {

ThreeCompartmentExtra::ThreeCompartmentExtra()
{
}

bool ThreeCompartmentExtra::checkInputs(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 6, "The number of parameters should be equal to 6."))
	    return false;
    
    Value a0, a1, a2, p, q, r1, r2, phi;

    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters[0].getValue();
    m_F = _parameters[1].getValue();
    m_Q1 = _parameters[2].getValue();
    m_Q2 = _parameters[3].getValue();
    m_V1 = _parameters[4].getValue();
    m_V2 = _parameters[5].getValue();
    m_Ka = _parameters[6].getValue();
    m_Ke = m_Cl / m_V1;
    m_K12 = m_Q1 / m_V1;
    m_K21 = m_Q1 / m_V2;
    m_K13 = m_Q2 / m_V1;
    m_K31 = m_Q2 / m_V2;
    m_NbPoints = _intakeEvent.getNumberPoints();

    bOK &= checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_Cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Cl), "The CL is Inf.");
    bOK &= checkValue(m_F > 0, "The F is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_Q1 > 0, "The Q1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Q1), "The Q1 is NaN.");
    bOK &= checkValue(!std::isinf(m_Q1), "The Q1 is Inf.");
    bOK &= checkValue(m_Q2 > 0, "The Q2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Q2), "The Q2 is NaN.");
    bOK &= checkValue(!std::isinf(m_Q2), "The Q2 is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(m_V2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(m_V2), "The V2 is Inf.");

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

    return bOK;
}


void ThreeCompartmentExtra::prepareComputations(const IntakeEvent& _intakeEvent, const Parameters& _parameters)
{
}


void ThreeCompartmentExtra::computeLogarithms(const IntakeEvent& _intakeEvent, const Parameters& _parameters, Eigen::VectorXd& _times)
{
    m_precomputedLogarithms["Alpha"] = (-m_Alpha * _times).array().exp();
    m_precomputedLogarithms["Beta"] = (-m_Beta * _times).array().exp();
    m_precomputedLogarithms["Gamma"] = (-m_Gamma * _times).array().exp();
    m_precomputedLogarithms["Ka"] = (-m_Ka * _times).array().exp();
}


bool ThreeCompartmentExtra::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    bool bOK = true;

    Concentration resid1 = _inResiduals[0] + m_F*m_D/m_V1;
    Concentration resid2 = _inResiduals[1];
    Concentration resid3 = _inResiduals[2];

    Value A = 1/m_V1 * (m_Ka/(m_Ka - m_Alpha)) * (m_K21 - m_Alpha) * (m_K31 - m_Alpha) / (m_Alpha - m_Beta) / (m_Alpha - m_Gamma);
    Value B = 1/m_V1 * (m_Ka/(m_Ka - m_Beta)) * (m_K21 - m_Beta) * (m_K31 - m_Beta) / (m_Beta - m_Alpha) / (m_Beta - m_Gamma);
    Value C = 1/m_V1 * (m_Ka/(m_Ka - m_Gamma)) * (m_K21 - m_Gamma) * (m_K31 - m_Gamma) / (m_Gamma - m_Beta) / (m_Gamma - m_Alpha);

    Value A2 = m_K12 / (m_K21 - m_Alpha) * A;
    Value B2 = m_K12 / (m_K21 - m_Beta) * B;
    Value C2 = m_K12 / (m_K21 - m_Gamma) * C;
    Value A3 = m_K13 / (m_K31 - m_Alpha) * A;
    Value B3 = m_K13 / (m_K31 - m_Beta) * B;
    Value C3 = m_K13 / (m_K31 - m_Gamma) * C;

    // Calculate concentrations for comp1, comp2 and comp3
    Eigen::VectorXd concentrations1 = 
        resid1 * (B * m_precomputedLogarithms["Beta"] 
            + A * m_precomputedLogarithms["Alpha"] 
            + C * m_precomputedLogarithms["Gamma"]
            - (A + B + C) * m_precomputedLogarithms["Ka"]);

    Value concentrations2 = 
        resid2 + resid1 * (B2 * m_precomputedLogarithms["Beta"](m_NbPoints - 1) 
            + A2 * m_precomputedLogarithms["Alpha"](m_NbPoints - 1) 
            + C2 * m_precomputedLogarithms["Gamma"](m_NbPoints - 1)
            - (A2 + B2 + C2) * m_precomputedLogarithms["Ka"](m_NbPoints - 1));

    Value concentrations3 = 
        resid3 + resid1 * (B3 * m_precomputedLogarithms["Beta"](m_NbPoints - 1) 
            + A3 * m_precomputedLogarithms["Alpha"](m_NbPoints - 1) 
            + C3 * m_precomputedLogarithms["Gamma"](m_NbPoints - 1)
            - (A3 + B3 + C3) * m_precomputedLogarithms["Ka"](m_NbPoints - 1));


    // return concentrations of comp1, comp2 and comp3
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2);
    _outResiduals.push_back(concentrations3);
    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bOK &= checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");
    bOK &= checkValue(_outResiduals[2] >= 0, "The concentration3 is negative.");

    return bOK;
}

}
}

