/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/loggerhelper.h"

#include "tucucore/intakeevent.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/cachedlogarithms.h"

namespace Tucuxi {
namespace Core {

IntakeIntervalCalculator::Result IntakeIntervalCalculator::calculateIntakePoints(
    Concentrations& _concentrations,
    TimeOffsets & _times,
    const IntakeEvent& _intakeEvent,
    const ParameterSetEvent& _parameters,
    const Residuals& _inResiduals,
    const CycleSize _cycleSize,
    Residuals & _outResiduals,
    const bool _isDensityConstant)
{
    if (!checkInputs(_intakeEvent, _parameters))
    {
        return Result::BadParameters;
    }

    // Create our serie of times
    Eigen::VectorXd times = Eigen::VectorXd::LinSpaced(_cycleSize, 0, static_cast<int>(_intakeEvent.getInterval().toMilliseconds()));

    // Can we reuse cached logarithms? 
    if (!m_cache.get(_intakeEvent.getInterval(), _parameters, _cycleSize, m_precomputedLogarithms))	{
        computeLogarithms(_intakeEvent, _parameters, times);
        m_cache.set(_intakeEvent.getInterval(), _parameters, _cycleSize, m_precomputedLogarithms);
    }

    if (!computeConcentrations(_inResiduals, _concentrations, _outResiduals)) {
        return Result::BadConcentration;
    }

    times = times.array() + _intakeEvent.getOffsetTime().toMilliseconds();
    _times.assign(times.data(), times.data() + times.size());

    return Result::Ok;

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


IntakeIntervalCalculator::Result IntakeIntervalCalculator::calculateIntakeSinglePoint(
    Concentrations& _concentrations,
    const IntakeEvent& _intakeEvent,
    const ParameterSetEvent& _parameters,
    const Residuals& _inResiduals,
    const int64& _atTime,
    Residuals& _outResiduals)
{
    if (!checkInputs(_intakeEvent, _parameters))
    {
        return Result::BadParameters;
    }

    prepareComputations(_intakeEvent, _parameters);
    
    // To reuse interface of computeLogarithms with multiple points, remaine time as a vector.
    Eigen::VectorXd times(2); 
    times << _atTime, (_intakeEvent.getInterval()).toMilliseconds();

    computeLogarithms(_intakeEvent, _parameters, times);

    if (!computeConcentration(_atTime, _inResiduals, _concentrations, _outResiduals)) {
        return Result::BadConcentration;
    }

    return Result::Ok;
}


bool IntakeIntervalCalculator::checkValue(bool _isOk, const std::string& _errMsg)
{
    if (!_isOk) {
        static Tucuxi::Common::LoggerHelper logger;
        logger.error(_errMsg);
    }
    return _isOk;
}

}
}

