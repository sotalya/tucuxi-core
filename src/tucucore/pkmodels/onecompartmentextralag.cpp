//@@license@@

#include <Eigen/Dense>
#include <Eigen/LU>

#include "tucucore/pkmodels/onecompartmentextralag.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

OneCompartmentExtraLagMicro::OneCompartmentExtraLagMicro()
    : IntakeIntervalCalculatorBase<2, OneCompartmentExtraLagExponentials>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}


std::vector<std::string> OneCompartmentExtraLagMicro::getParametersId()
{
    return {"Ke", "V", "Ka", "F", "Tlag"};
}

bool OneCompartmentExtraLagMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V = _parameters.getValue(ParameterId::V);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    if (m_Tlag < 0.0) {
        m_Tlag = 0.0;
    }

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    if (m_nbPoints == 2) {
        m_nbPoints0 = static_cast<Eigen::Index>(std::min(
                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)), ceil(static_cast<double>(m_nbPoints))));
    }
    else {
        //    m_nbPoints0 = std::min(m_nbPoints, std::max(2, static_cast<int>((m_Tlag / m_Int) * static_cast<double>(m_nbPoints))));
        m_nbPoints0 = std::min(
                static_cast<Eigen::Index>(m_nbPoints),
                std::max(
                        Eigen::Index{2},
                        static_cast<Eigen::Index>(std::min(
                                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)),
                                ceil(static_cast<double>(m_nbPoints))))));
    }
    m_nbPoints1 = m_nbPoints - m_nbPoints0;


    return bOK;
}


inline void OneCompartmentExtraLagMicro::compute(
        const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    if (m_Tlag <= 0.0) {
        Concentration resid1 = _inResiduals[0];
        Concentration resid2 = _inResiduals[1] + m_F * m_D / m_V;
        Concentration part2 = m_Ka * resid2 / (-m_Ka + m_Ke);

        _concentrations1 = exponentials(Exponentials::Ke) * resid1
                           + (exponentials(Exponentials::Ka) - exponentials(Exponentials::Ke)) * part2;
        _concentrations2 = resid2 * exponentials(Exponentials::Ka);
    }
    else {
        Concentration resid1 = _inResiduals[0];
        Concentration resid2 = _inResiduals[1];
        Concentration part2 = m_Ka * resid2 / (-m_Ka + m_Ke);

        Eigen::VectorXd concentrations1 = exponentials(Exponentials::Ke) * resid1
                                          + (exponentials(Exponentials::Ka) - exponentials(Exponentials::Ke)) * part2;
        Eigen::VectorXd concentrations2 = resid2 * exponentials(Exponentials::Ka);

        Concentration postLagResid1 =
                exp(-m_Ke * m_Tlag) * resid1 + (exp(-m_Ka * m_Tlag) - exp(-m_Ke * m_Tlag)) * part2;
        Concentration postLagResid2 = resid2 * exp(-m_Ka * m_Tlag) + m_F * m_D / m_V;
        Concentration postLagPart2 = m_Ka * postLagResid2 / (-m_Ka + m_Ke);

        Eigen::VectorXd concentrations1post =
                exponentials(Exponentials::KePost) * postLagResid1
                + (exponentials(Exponentials::KaPost) - exponentials(Exponentials::KePost)) * postLagPart2;
        Eigen::VectorXd concentrations2post = postLagResid2 * exponentials(Exponentials::KaPost);

        _concentrations1 = Eigen::VectorXd(m_nbPoints);
        _concentrations2 = Eigen::VectorXd(m_nbPoints);
        for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(m_nbPoints0); i++) {
            _concentrations1[i] = concentrations1[i];
            _concentrations2[i] = concentrations2[i];
        }
        for (Eigen::Index i = static_cast<Eigen::Index>(m_nbPoints0); i < static_cast<Eigen::Index>(m_nbPoints); i++) {
            _concentrations1[i] = concentrations1post[i];
            _concentrations2[i] = concentrations2post[i];
        }
    }
}

void OneCompartmentExtraLagMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Ka, (-m_Ka * _times).array().exp());
    setExponentials(Exponentials::Ke, (-m_Ke * _times).array().exp());
    Eigen::VectorXd diff = m_Tlag * Eigen::VectorXd::Ones(_times.size());
    setExponentials(Exponentials::KaPost, (-m_Ka * (_times - diff)).array().exp());
    setExponentials(Exponentials::KePost, (-m_Ke * (_times - diff)).array().exp());
}


bool OneCompartmentExtraLagMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Eigen::VectorXd concentrations2;
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

bool OneCompartmentExtraLagMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
{
    TMP_UNUSED_PARAMETER(_atTime);
    Eigen::VectorXd concentrations1;
    Eigen::VectorXd concentrations2;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    int atTime = static_cast<int>(SingleConcentrations::AtTime);
    int atEndInterval = static_cast<int>(SingleConcentrations::AtEndInterval);

    // We fix nb points to be 2, to be coherent with calculateExponentials
    m_nbPoints = 2;
    // Then, depending on the fact that the time of interest is before the lag time
    // or not we set the number of points for each of these two intervals
    if (_atTime <= m_Tlag) {
        m_nbPoints0 = 1;
        m_nbPoints1 = 1;
    }
    else {
        m_nbPoints0 = 0;
        m_nbPoints1 = 2;
    }
    // compute concentration1 and 2
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

OneCompartmentExtraLagMacro::OneCompartmentExtraLagMacro() {}

std::vector<std::string> OneCompartmentExtraLagMacro::getParametersId()
{
    return {"CL", "V", "Ka", "F", "Tlag"};
}

bool OneCompartmentExtraLagMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
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
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    if (m_Tlag < 0.0) {
        m_Tlag = 0.0;
    }

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    if (m_nbPoints == 2) {
        m_nbPoints0 = static_cast<Eigen::Index>(std::min(
                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)), ceil(static_cast<double>(m_nbPoints))));
    }
    else {
        //    m_nbPoints0 = std::min(m_nbPoints, std::max(2, static_cast<int>((m_Tlag / m_Int) * static_cast<double>(m_nbPoints))));
        m_nbPoints0 = std::min(
                static_cast<Eigen::Index>(m_nbPoints),
                std::max(
                        Eigen::Index{2},
                        static_cast<Eigen::Index>(std::min(
                                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)),
                                ceil(static_cast<double>(m_nbPoints))))));
    }
    m_nbPoints1 = m_nbPoints - m_nbPoints0;


    return bOK;
}

} // namespace Core
} // namespace Tucuxi
