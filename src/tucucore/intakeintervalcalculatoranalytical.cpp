#include "intakeintervalcalculatoranalytical.h"

#include "intakeevent.h"

namespace Tucuxi {
namespace Core {

IntakeIntervalCalculatorAnalytical::~IntakeIntervalCalculatorAnalytical()
= default;

ComputingStatus IntakeIntervalCalculatorAnalytical::calculateIntakePoints(
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
        return ComputingStatus::BadParameters;
    }
    m_loggingErrors = true;


    if (_inResiduals.size() < getResidualSize()) {
        throw std::runtime_error("[IntakeIntervalCalculator] Input residual vector size too short");
    }
    if (_outResiduals.size() < getResidualSize()) {
        throw std::runtime_error("[IntakeIntervalCalculator] Output residual vector size too short");
    }
    CHECK_CALCULATEINTAKEPOINTS_INPUTS;

    // Create our serie of times
    CycleSize nbPoints = _intakeEvent.getNbPoints();
    Eigen::VectorXd times(nbPoints);
    m_pertinentTimesCalculator->calculateTimes(_intakeEvent, static_cast<Eigen::Index>(nbPoints), times);

    // Can we reuse cached exponentials?
    if (!m_cache.get(_intakeEvent.getInterval(), _parameters, nbPoints, m_precomputedExponentials))	{
        computeExponentials(times);
        m_cache.set(_intakeEvent.getInterval(), _parameters, nbPoints, m_precomputedExponentials);
    }

    if (!computeConcentrations(_inResiduals, _isAll, _concentrations, _outResiduals)) {
        return ComputingStatus::BadConcentration;
    }

    times = times.array() + _intakeEvent.getOffsetTime().toHours();
    _times.assign(times.data(), times.data() + times.size());

    return ComputingStatus::Ok;
}


ComputingStatus IntakeIntervalCalculatorAnalytical::calculateIntakeSinglePoint(
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

    m_loggingErrors = false;
    if (!checkInputs(_intakeEvent, _parameters)) {
        m_loggingErrors = true;
        return ComputingStatus::BadParameters;
    }
    m_loggingErrors = true;

    CHECK_CALCULATEINTAKESINGLEPOINT_INPUTS;

    // To reuse interface of computeExponentials with multiple points, remaine time as a vector.
    Eigen::VectorXd times(2);
    times << static_cast<double>(_atTime), static_cast<double>(_intakeEvent.getInterval().toHours());

    computeExponentials(times);

    if (!computeConcentration(_atTime, _inResiduals, _isAll, _concentrations, _outResiduals)) {
        return ComputingStatus::BadConcentration;
    }

    return ComputingStatus::Ok;
}




} // namespace Core
} // namespace Tucuxi
