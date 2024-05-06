//@@license@@

#include <Eigen/Dense>
#include <Eigen/LU>

#include "tucucore/pkmodels/onecompartmentextra.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

OneCompartmentExtraMicro::OneCompartmentExtraMicro()
    : IntakeIntervalCalculatorBase<2, OneCompartmentExtraExponentials>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}

std::vector<std::string> OneCompartmentExtraMicro::getParametersId()
{
    return {"Ke", "V", "Ka", "F"};
}


bool OneCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


void OneCompartmentExtraMicro::computeExponentials(Eigen::VectorXd& _times)
{
    //    const auto size = _times.size();
    //    Eigen::VectorXd vec(size);
    //    for (int i = 0; i < size; i++) {
    //        vec[i] = std::exp(-m_Ka * _times[i]);
    //    }
    //    setExponentials(Exponentials::Ka, vec);
    setExponentials(Exponentials::Ka, (-m_Ka * _times).array().exp());
    setExponentials(Exponentials::Ke, (-m_Ke * _times).array().exp());
}


bool OneCompartmentExtraMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);

    // compute concenration1 and 2
    compute(_inResiduals, concentrations1, concentrations2);
    _outResiduals[firstCompartment] = concentrations1[m_nbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2[m_nbPoints - 1];

    // Return concentrations of first compartment
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
    // Return concentrations of other compartments
    if (_isAll) {
        _concentrations[secondCompartment].assign(
                concentrations2.data(), concentrations2.data() + concentrations2.size());
    }

    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

    return bOK;
}

bool OneCompartmentExtraMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
{
    TMP_UNUSED_PARAMETER(_atTime);
    Eigen::VectorXd concentrations1, concentrations2;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

    // compute concenration1 and 2
    compute(_inResiduals, concentrations1, concentrations2);

    // return concentraions (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations1[atTime]);
    if (_isAll) {
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

    bool bOK = checkCondition(_outResiduals[firstCompartment] >= 0, "The final residual1 is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The final residual2 is negative.");

    return bOK;
}

OneCompartmentExtraMacro::OneCompartmentExtraMacro() : OneCompartmentExtraMicro() {}

std::vector<std::string> OneCompartmentExtraMacro::getParametersId()
{
    return {"CL", "V", "Ka", "F"};
}

bool OneCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    Value cl = _parameters.getValue(ParameterId::CL); // clearance
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ke = cl / m_V;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}

} // namespace Core
} // namespace Tucuxi
