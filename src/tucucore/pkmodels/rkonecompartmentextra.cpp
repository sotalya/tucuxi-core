/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>
#include <Eigen/LU>

#include "tucucore/pkmodels/rkonecompartmentextra.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

RK4OneCompartmentExtraMicro::RK4OneCompartmentExtraMicro()
{
}

bool RK4OneCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
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


void RK4OneCompartmentExtraMicro::computeExponentials(Eigen::VectorXd& _times)
{
    /* RTH: RK4 has no exponentials to compute*/
}


bool RK4OneCompartmentExtraMicro::computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    /* RTH: Have to set the sizes explicitly here because the exponentials are 
     * not used.*/
    Eigen::VectorXd concentrations1(m_NbPoints), concentrations2(m_NbPoints);
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);

    // compute concenration1 and 2
    compute(_inResiduals, concentrations1, concentrations2);
    _outResiduals[firstCompartment] = concentrations1[m_NbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2[m_NbPoints - 1];

    // Return concentrations of first compartment
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());	
    // Return concentrations of other compartments
    if (_isAll == true) {
        _concentrations[secondCompartment].assign(concentrations2.data(), concentrations2.data() + concentrations2.size());	
    }

    bool bOK = checkValue(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

    return bOK;
}

bool RK4OneCompartmentExtraMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    TMP_UNUSED_PARAMETER(_atTime);
    Eigen::VectorXd concentrations1(m_NbPoints), concentrations2(m_NbPoints);
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);
    int atTime = static_cast<int>(SingleConcentrations::AtTime);
    int atEndInterval = static_cast<int>(m_NbPoints-1);

    // compute concenration1 and 2
    compute(_inResiduals, concentrations1, concentrations2);

    // return concentraions (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations1[atTime]);
    if (_isAll == true) {
	_concentrations[secondCompartment].push_back(concentrations2[atTime]);
    }
    
    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[atEndInterval] = 0;
        concentrations2[atEndInterval] = 0;
    }

    // Return final residual (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2[atEndInterval];

    bool bOK = checkValue(_outResiduals[firstCompartment] >= 0, "The final residual1 is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The final residual2 is negative.");

    return bOK;
}

RK4OneCompartmentExtraMacro::RK4OneCompartmentExtraMacro() : RK4OneCompartmentExtraMicro()
{
}

bool RK4OneCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    Value cl = _parameters.getValue(ParameterId::CL); // clearance
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ke = cl / m_V;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
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

