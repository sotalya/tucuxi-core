//@@license@@

#include <Eigen/Dense>

#include "tucucore/pkmodels/threecompartmentextra.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

ThreeCompartmentExtraMicro::ThreeCompartmentExtraMicro()
    : IntakeIntervalCalculatorBase<3, ThreeCompartmentExtraExponentials>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}

std::vector<std::string> ThreeCompartmentExtraMicro::getParametersId()
{
    return {"Ke", "V1", "K12", "K21", "K13", "K31", "Ka", "F"};
}

bool ThreeCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_F = _parameters.getValue(ParameterId::F);
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_K13 = _parameters.getValue(ParameterId::K13);
    m_K31 = _parameters.getValue(ParameterId::K31);
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    Value a0 = m_Ke * m_K21 * m_K31;
    Value a1 = m_Ke * m_K31 + m_K21 * m_K31 + m_K21 * m_K13 + m_Ke * m_K21 + m_K31 * m_K12;
    Value a2 = m_Ke + m_K12 + m_K13 + m_K21 + m_K31;
    Value p = a1 - std::pow(a2, 2) / 3;
    Value q = 2 * std::pow(a2, 3) / 27 - a1 * a2 / 3 + a0;
    Value r1 = std::sqrt(-(std::pow(p, 3) / 27));
    Value r2 = 2 * std::pow(r1, 1 / 3);
    Value phi = std::acos(-q / (2 * r1)) / 3;

    m_Alpha = -(std::cos(phi) * r2 - a2 / 3);
    m_Beta = -(std::cos(phi + 2 * 3.1428 / 3) * r2 - a2 / 3);
    m_Gamma = -(std::cos(phi + 4 * 3.1428 / 3) * r2 - a2 / 3);

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_F: {}", m_F);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_Ka: {}", m_Ka);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_K13: {}", m_K13);
    logHelper.debug("m_K31: {}", m_K31);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkStrictlyPositiveValue(m_K12, "K12");
    bOK &= checkStrictlyPositiveValue(m_K21, "K21");
    bOK &= checkStrictlyPositiveValue(m_K13, "K13");
    bOK &= checkStrictlyPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkPositiveValue(m_Gamma, "Gamma");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}

void ThreeCompartmentExtraMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
    setExponentials(Exponentials::Gamma, (-m_Gamma * _times).array().exp());
    setExponentials(Exponentials::Ka, (-m_Ka * _times).array().exp());
}


bool ThreeCompartmentExtraMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Value concentrations2 = NAN;
    Value concentrations3 = NAN;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // return residuals of comp1, comp2 and comp3
    _outResiduals[firstCompartment] = concentrations1[m_nbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2;
    _outResiduals[thirdCompartment] = concentrations3;

    // return concentration
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration3 is negative.");

    return bOK;
}

bool ThreeCompartmentExtraMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
{
    TMP_UNUSED_PARAMETER(_atTime);
    Eigen::VectorXd concentrations1;
    Value concentrations2;
    Value concentrations3;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

    // Calculate concentrations for comp1 and comp2
    compute(_inResiduals, concentrations1, concentrations2, concentrations3);

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

    // return final residual (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2;
    _outResiduals[thirdCompartment] = concentrations3;

    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration3 is negative.");

    return bOK;
}

ThreeCompartmentExtraMacro::ThreeCompartmentExtraMacro() = default;

std::vector<std::string> ThreeCompartmentExtraMacro::getParametersId()
{
    return {"CL", "V1", "Q", "V2", "Q1", "Q2", "Ka", "F"};
}

bool ThreeCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    Value cl = _parameters.getValue(ParameterId::CL);
    m_F = _parameters.getValue(ParameterId::F);
    Value q3 = _parameters.getValue(ParameterId::Q3);
    Value q2 = _parameters.getValue(ParameterId::Q2);
    m_V1 = _parameters.getValue(ParameterId::V1);
    Value v2 = _parameters.getValue(ParameterId::V2);
    Value v3 = _parameters.getValue(ParameterId::V3);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_Ke = cl / m_V1;
    m_K12 = q2 / m_V1;
    m_K21 = q2 / v2;
    m_K13 = q3 / m_V1;
    m_K31 = q3 / v3;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    Value a0 = m_Ke * m_K21 * m_K31;
    Value a1 = m_Ke * m_K31 + m_K21 * m_K31 + m_K21 * m_K13 + m_Ke * m_K21 + m_K31 * m_K12;
    Value a2 = m_Ke + m_K12 + m_K13 + m_K21 + m_K31;
    Value p = a1 - std::pow(a2, 2) / 3;
    Value q = 2 * std::pow(a2, 3) / 27 - a1 * a2 / 3 + a0;
    Value r1 = std::sqrt(-(std::pow(p, 3) / 27));
    Value r2 = 2 * std::pow(r1, 1 / 3);
    Value phi = std::acos(-q / (2 * r1)) / 3;

    m_Alpha = -(std::cos(phi) * r2 - a2 / 3);
    m_Beta = -(std::cos(phi + 2 * 3.1428 / 3) * r2 - a2 / 3);
    m_Gamma = -(std::cos(phi + 4 * 3.1428 / 3) * r2 - a2 / 3);

#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("cl: {}", cl);
    logHelper.debug("m_F: {}", m_F);
    logHelper.debug("q1: {}", q1);
    logHelper.debug("q2: {}", q2);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("v2: {}", v2);
    logHelper.debug("m_Ka: {}", m_Ka);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_K13: {}", m_K13);
    logHelper.debug("m_K31: {}", m_K31);
    logHelper.debug("m_nbPoints: {}", m_nbPoints);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(q, "Q");
    bOK &= checkStrictlyPositiveValue(q3, "Q3");
    bOK &= checkStrictlyPositiveValue(q2, "Q2");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkPositiveValue(m_Gamma, "Gamma");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}

} // namespace Core
} // namespace Tucuxi
