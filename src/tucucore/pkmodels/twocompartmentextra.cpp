//@@license@@

#include <Eigen/Dense>

#include "tucucore/pkmodels/twocompartmentextra.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

TwoCompartmentExtraMicro::TwoCompartmentExtraMicro()
    : IntakeIntervalCalculatorBase<3, TwoCompartmentExtraExponentials>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}

std::vector<std::string> TwoCompartmentExtraMicro::getParametersId()
{
    return {"Ke", "V1", "K12", "K21", "Ka", "F"};
}

bool TwoCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK) / 2;
    m_Beta = (sumK - m_RootK) / 2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_K12, "K12");
    bOK &= checkStrictlyPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void TwoCompartmentExtraMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
    setExponentials(Exponentials::Ka, (-m_Ka * _times).array().exp());
}


bool TwoCompartmentExtraMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Eigen::VectorXd concentrations2, concentrations3;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);

    // Compute concentrations
    bool bOK = compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    if (!bOK) {
        return false;
    }
    // Return residuals of comp1, comp2 and comp3
    _outResiduals[firstCompartment] = concentrations1[m_nbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2[m_nbPoints - 1];
    _outResiduals[thirdCompartment] = concentrations3[m_nbPoints - 1];

    // Return concentrations of comp1, comp2 and comp3
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    bOK &= checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration is negative.");

    return bOK;
}

bool TwoCompartmentExtraMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Eigen::VectorXd concentrations2, concentrations3;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

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
        concentrations2[atEndInterval] = 0;
        concentrations3[atEndInterval] = 0;
    }

    // Return final residual of comp1, comp2 and comp3 (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2[atEndInterval];
    _outResiduals[thirdCompartment] = concentrations3[atEndInterval];

    bOK &= checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration is negative.");

    return bOK;
}

TwoCompartmentExtraMacro::TwoCompartmentExtraMacro() : TwoCompartmentExtraMicro() {}

std::vector<std::string> TwoCompartmentExtraMacro::getParametersId()
{
    return {"CL", "V1", "Q", "V2", "Ka", "F"};
}

bool TwoCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    Value cl = _parameters.getValue(ParameterId::CL);
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
    m_Alpha = (sumK + m_RootK) / 2;
    m_Beta = (sumK - m_RootK) / 2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(q, "Q");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");


    return bOK;
}


std::vector<std::string> TwoCompartmentExtraMacroRatios::getParametersId()
{
    return {"CL", "V1", "V2", "Ka", "F"};
}

bool TwoCompartmentExtraMacroRatios::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 5, "The number of parameters should be equal to 5.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    Value cl = _parameters.getValue(ParameterId::CL);
    m_V1 = _parameters.getValue(ParameterId::V1);
    Value rQCL = _parameters.getValue(ParameterId::RQCL);
    Value rV2V1 = _parameters.getValue(ParameterId::RV2V1);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    Value v2 = m_V1 * rV2V1;
    Value q = cl * rQCL;
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK) / 2;
    m_Beta = (sumK - m_RootK) / 2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");


    return bOK;
}

} // namespace Core
} // namespace Tucuxi
