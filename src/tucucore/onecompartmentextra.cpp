/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>
#include <Eigen/LU>

#include "tucucore/onecompartmentextra.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

OneCompartmentExtraMicro::OneCompartmentExtraMicro()
{
}

bool OneCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    m_Ke = _parameters.getValue(0);
    m_F = _parameters.getValue(1);
    m_Ka = _parameters.getValue(2);
    m_V = _parameters.getValue(3);
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = static_cast<int>((_intakeEvent.getInterval()).toHours());

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
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void OneCompartmentExtraMicro::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times)
{
    setLogs(Logarithms::Ka, (-m_Ka * _times).array().exp());
    setLogs(Logarithms::Ke, (-m_Ke * _times).array().exp());
}


bool OneCompartmentExtraMicro::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    bool bOK = true;
    Eigen::VectorXd concentrations1, concentrations2;

    // compute concenration1 and 2
    compute(_inResiduals, concentrations1, concentrations2);

    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2[m_NbPoints - 1]);
    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	

    bOK &= checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");

    return bOK;
}

bool OneCompartmentExtraMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;

    // compute concenration1 and 2
    compute(_inResiduals, concentrations1, concentrations2);

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations1[0]);
    _concentrations.push_back(concentrations2[0]);
    
    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[1] = 0;
        concentrations2[1] = 0;
    }

    // Return final residual (computation with m_Int (interval))
    _outResiduals.push_back(concentrations1[1]);
    _outResiduals.push_back(concentrations2[1]);

    bool bOK = checkValue(_outResiduals[0] >= 0, "The final residual1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The final residual2 is negative.");
    bOK &= checkValue(_concentrations[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_concentrations[1] >= 0, "The concentration2 is negative.");

    return bOK;
}

OneCompartmentExtraMacro::OneCompartmentExtraMacro() : OneCompartmentExtraMicro()
{
}

bool OneCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;
    
    m_D = _intakeEvent.getDose() * 1000;
    Value cl = _parameters.getValue(0); // clearance
    m_F = _parameters.getValue(1);
    m_Ka = _parameters.getValue(2);
    m_V = _parameters.getValue(3);
    m_Ke = cl / m_V;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

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
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The clearance is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The clearance is Inf.");
    bOK &= checkValue(m_Ka > 0, "The ka is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The m_Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The m_Ka is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");

    return bOK;
}

}
}
