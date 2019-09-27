#include "intakeintervalcalculatoranalytical.h"

#include "intakeevent.h"

namespace Tucuxi {
namespace Core {

IntakeIntervalCalculatorAnalytical::~IntakeIntervalCalculatorAnalytical()
{
}

ComputingResult IntakeIntervalCalculatorAnalytical::calculateIntakePoints(
        std::vector<Concentrations>& _concentrations,
        TimeOffsets & _times,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        bool _isAll,
        Residuals & _outResiduals,
        const bool _isDensityConstant)
{
    if (m_firstCalculation) {
        m_firstCalculation = false;
        m_lastThreadId = std::this_thread::get_id();
    }
    else {
        if (m_lastThreadId != std::this_thread::get_id()) {
            // Somthing strange. It should not be used by another thread
            // Maybe raise an error here
        }
    }
    TMP_UNUSED_PARAMETER(_isDensityConstant);
    m_loggingErrors = false;
    if (!checkInputs(_intakeEvent, _parameters))
    {
        m_loggingErrors = true;
        return ComputingResult::BadParameters;
    }
    m_loggingErrors = true;

    // Create our serie of times
    int nbPoints = _intakeEvent.getNbPoints();

    // YTA : This LinSpaced function crashes on Linux, so using a custom
    // method...
//    Eigen::VectorXd times = Eigen::VectorXd::LinSpaced(_intakeEvent.getNbPoints(), 0, _intakeEvent.getInterval().toHours());
    Eigen::VectorXd times(nbPoints);
    m_pertinentTimesCalculator->calculateTimes(_intakeEvent, nbPoints, times);

    // Can we reuse cached exponentials?
    if (!m_cache.get(_intakeEvent.getInterval(), _parameters, nbPoints, m_precomputedExponentials))	{
        computeExponentials(times);
        m_cache.set(_intakeEvent.getInterval(), _parameters, nbPoints, m_precomputedExponentials);
    }

    if (!computeConcentrations(_inResiduals, _isAll, _concentrations, _outResiduals)) {
        return ComputingResult::BadConcentration;
    }

    times = times.array() + _intakeEvent.getOffsetTime().toHours();
    _times.assign(times.data(), times.data() + times.size());

    return ComputingResult::Ok;
}


ComputingResult IntakeIntervalCalculatorAnalytical::calculateIntakeSinglePoint(
    std::vector<Concentrations>& _concentrations,
    const IntakeEvent& _intakeEvent,
    const ParameterSetEvent& _parameters,
    const Residuals& _inResiduals,
    const Value& _atTime,
    bool _isAll,
    Residuals& _outResiduals)
{
    if (m_firstCalculation) {
        m_firstCalculation = false;
        m_lastThreadId = std::this_thread::get_id();
    }
    else {
        if (m_lastThreadId != std::this_thread::get_id()) {
            // Somthing strange. It should not be used by another thread
            // Maybe raise an error here
        }
    }

    if (!checkInputs(_intakeEvent, _parameters)) {
        return ComputingResult::BadParameters;
    }

    // To reuse interface of computeExponentials with multiple points, remaine time as a vector.
    Eigen::VectorXd times(2);
    times << static_cast<double>(_atTime), static_cast<double>(_intakeEvent.getInterval().toHours());

    computeExponentials(times);

    if (!computeConcentration(_atTime, _inResiduals, _isAll, _concentrations, _outResiduals)) {
        return ComputingResult::BadConcentration;
    }

    return ComputingResult::Ok;
}




} // namespace Core
} // namespace Tucuxi
