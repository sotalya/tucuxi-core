/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/loggerhelper.h"
#include "tucucommon/general.h"

#include "tucucore/intakeevent.h"
#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {



void PertinentTimesCalculatorStandard::calculateTimes(const IntakeEvent& _intakeEvent,
              int _nbPoints,
              Eigen::VectorXd& _times)
{
    for(int i = 0; i < _nbPoints; i++) {
        _times[i] = static_cast<double>(i) / static_cast<double>(_nbPoints - 1) * static_cast<double>(_intakeEvent.getInterval().toHours());
    }
}

void PertinentTimesCalculatorInfusion::calculateTimes(const IntakeEvent& _intakeEvent,
              int _nbPoints,
              Eigen::VectorXd& _times)
{
    double infusionTime = _intakeEvent.getInfusionTime().toHours();
    double interval = _intakeEvent.getInterval().toHours();

    if (_nbPoints == 2) {
        _times[0] = 0;
        _times[1] = interval;
        return;
    }

    if (_nbPoints == 1) {
        _times[0] = interval;
        return;
    }

    double postTime = interval - infusionTime;

    int nbInfus = std::max(2, static_cast<int>((infusionTime / interval) * static_cast<double>(_nbPoints)));
    int nbPost = _nbPoints - nbInfus;

    for(int i = 0; i < nbInfus; i++) {
        _times[i] = static_cast<double>(i) / static_cast<double>(nbInfus - 1) * infusionTime;
    }

    for(int i = 0; i < nbPost; i++) {
        _times[i + nbInfus] = infusionTime + static_cast<double>(i + 1) / static_cast<double>(nbPost) * postTime;
    }
}


IntakeIntervalCalculator::~IntakeIntervalCalculator()
{}

IntakeIntervalCalculatorAnalytical::~IntakeIntervalCalculatorAnalytical()
{
    delete m_pertinentTimesCalculator;
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

    // TMP_UNUSED_PARAMETER(toHours);

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


bool IntakeIntervalCalculator::checkValue(bool _isOk, const std::string& _errMsg)
{
    if (!_isOk) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error(_errMsg);
        }
    }
    return _isOk;
}

IntakeIntervalCalculatorRK4::~IntakeIntervalCalculatorRK4()
{}


ComputingResult IntakeIntervalCalculatorRK4::calculateIntakePoints(
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
     if (!checkInputs(_intakeEvent, _parameters))
     {
         return ComputingResult::BadParameters;
     }

     // Create our serie of times
     int nbPoints = _intakeEvent.getNbPoints();
     int toHours = _intakeEvent.getInterval().toHours();

     TMP_UNUSED_PARAMETER(toHours);

    // YTA : This LinSpaced function crashes on Linux, so using a custom
    // method...
//    Eigen::VectorXd times = Eigen::VectorXd::LinSpaced(_intakeEvent.getNbPoints(), 0, _intakeEvent.getInterval().toHours());
    Eigen::VectorXd times(nbPoints);
    // Shall we use nbPoints-1 or nbPoints?
    for(int i=0;i< nbPoints; i++)
        times[i] = ((double) i)/((double)nbPoints-1)*(double)_intakeEvent.getInterval().toHours();

    /* RTH: removed exponentials calcul/fetch from here.*/
    if (!computeConcentrations(_inResiduals, _isAll, _concentrations, _outResiduals)) {
        return ComputingResult::BadConcentration;
    }

    times = times.array() + _intakeEvent.getOffsetTime().toHours();
    _times.assign(times.data(), times.data() + times.size());

    return ComputingResult::Ok;

/*
    //1.interpolate and check error (assume nbPoints is always odd)
    //2. if error ok return current result, else increase density and rerun (for now just rerun with new density
    //and discard old, but eventually should add to existing with a merge of vectors
    if (isFixedDensity || densityError(timesv, concsv) < density_error_thresh || nbPoints > MAX_PT_DENSITY) {
        //Set the new residual
        finalResiduals[0] = concsv[nbPoints - 1];
        POSTCONDCONT(finalResiduals[0] >= 0, SHOULDNTGONEGATIVE, "The concentration is negative.")
        concs.assign(concsv.data(), concsv.data() + concsv.size());
        timesv = timesv.array() + ink.offsettime;
        times.assign(timesv.data(), timesv.data() + timesv.size());
        return Result::Ok;
    }

    // Need more points!
    return Result::DensityError;
*/
}


ComputingResult IntakeIntervalCalculatorRK4::calculateIntakeSinglePoint(
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

