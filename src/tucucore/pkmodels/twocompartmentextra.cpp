/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

TwoCompartmentExtraMicro::TwoCompartmentExtraMicro()
{
}

bool TwoCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_Ka > 0, "The m_Ka is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The m_Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The m_Ka is Inf.");
    bOK &= checkValue(m_Ke > 0, "The m_Ke is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The m_Ke is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The m_Ke is Inf.");
    bOK &= checkValue(m_K12 > 0, "The K12 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K12), "The K12 is NaN.");
    bOK &= checkValue(!std::isinf(m_K12), "The K12 is Inf.");
    bOK &= checkValue(m_K21 > 0, "The K21 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K21), "The K21 is NaN.");
    bOK &= checkValue(!std::isinf(m_K21), "The K21 is Inf.");
    bOK &= checkValue(m_F > 0, "The F is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is not greater than zero.");
    bOK &= checkValue(m_Alpha >= 0, "Alpha is negative.");
    bOK &= checkValue(m_Beta >= 0, "Beta is negative.");

    return true;
}


void TwoCompartmentExtraMicro::computeExponentials(Eigen::VectorXd& _times) 
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
    setExponentials(Exponentials::Ka, (-m_Ka * _times).array().exp());
}


bool TwoCompartmentExtraMicro::computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Value concentrations2, concentrations3;
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);
    int thirdCompartment = static_cast<int>(Compartments::Third);

    // Compute concentrations
    bool bOK = compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // Return residuals of comp1, comp2 and comp3
    _outResiduals[firstCompartment] = concentrations1[m_NbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2;
    _outResiduals[thirdCompartment] = concentrations3;

    // Return concentrations of comp1, comp2 and comp3
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    bOK &= checkValue(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[thirdCompartment] >= 0, "The concentration is negative.");

    return bOK;
}

bool TwoCompartmentExtraMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Value concentrations2, concentrations3;
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);
    int thirdCompartment = static_cast<int>(Compartments::Third);
    int atTime = static_cast<int>(SingleConcentrations::AtTime);
    int atEndInterval = static_cast<int>(SingleConcentrations::AtEndInterval);

    TMP_UNUSED_PARAMETER(_atTime);

    // Compute concentrations
    bool bOK = compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // return concentraions (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations1[atTime]);
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[atEndInterval] = 0;
        concentrations2 = 0;
        concentrations3 = 0;
    }

    // Return final residual of comp1, comp2 and comp3 (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2;
    _outResiduals[thirdCompartment] = concentrations3;

    bOK &= checkValue(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    bOK &= checkValue(_outResiduals[thirdCompartment] >= 0, "The concentration is negative.");

    return bOK;
}

TwoCompartmentExtraMacro::TwoCompartmentExtraMacro() : TwoCompartmentExtraMicro()
{
}

bool TwoCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }
    
    m_D = _intakeEvent.getDose() * 1000;
    Value cl = _parameters.getValue(ParameterId::Cl);
    Value q = _parameters.getValue(ParameterId::Q);
    Value v2 = _parameters.getValue(ParameterId::V2);
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK)/2;
    m_Beta = (sumK - m_RootK)/2;
    m_NbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The CL is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The m_V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The m_V1 is Inf.");
    bOK &= checkValue(v2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(v2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(v2), "The V2 is Inf.");
    bOK &= checkValue(m_Ka > 0, "The Ka is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ka), "The Ka is NaN.");
    bOK &= checkValue(!std::isinf(m_Ka), "The Ka is Inf.");
    bOK &= checkValue(m_F > 0, "The F is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_F), "The F is NaN.");
    bOK &= checkValue(!std::isinf(m_F), "The F is Inf.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is not greater than zero.");
    bOK &= checkValue(m_Alpha >= 0, "Alpha is negative.");
    bOK &= checkValue(m_Beta >= 0, "Beta is negative.");

    return true;
}

}
}

