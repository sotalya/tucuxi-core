/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "concentrationcalculator.h"

namespace Tucuxi {
namespace Core {

ConcentrationCalculator::ComputationResult ConcentrationCalculator::computeConcentrations(
    ConcentrationPredictionPtr &_prediction,
    const int _nbPoints,
    const IntakeSeries &_intakes,
    const ParameterSetSeries &_parameterSets,
    const Etas &_etas,
    const IResidualErrorModel &_residualErrorModel,
    const Deviations& _epsilons,
    const bool _isFixedDensity)
{
    TMP_UNUSED_PARAMETER(_nbPoints);

    // First calculate the size of residuals
    unsigned int residualSize = 0;
    for (IntakeSeries::const_iterator it = _intakes.begin(); it != _intakes.end(); it++) {
        residualSize = std::max(residualSize, (*it).getCalculator()->getResidualSize());
    }
    Residuals r1(residualSize);
    Residuals r2(residualSize);

    // Go through all intakes
    for (IntakeSeries::const_iterator it = _intakes.begin(); it != _intakes.end(); it++) {
        const IntakeEvent &intake = *it;

        // Get parameters at intake start time
        // For population calculation, could be done only once at the beginning
        ParameterSetEventPtr parameters = _parameterSets.getAtTime(intake.getEventTime(), _etas);
        if (parameters == nullptr) {
            //m_logger.error("No parameters found!");
            return ComputationResult::Failure;
        }

        // Use nbpoints for the minimum pt density
        // Must use an odd number
        const int density = static_cast<int>((int(floor(intake.getNbPoints())) % 2 != 0) ? floor(intake.getNbPoints()) : ceil(intake.getNbPoints()));

        // Compute concentrations for the current cycle
        TimeOffsets times;
        Concentrations values;
        _prediction->allocate(density, times, values);
        r2.clear();
        IntakeIntervalCalculator::Result result = it->calculateIntakePoints(values, times, intake, *parameters, r1, intake.getNbPoints(), r2, _isFixedDensity);
        switch (result)
        {
            case IntakeIntervalCalculator::Result::Ok:
                break;
            case IntakeIntervalCalculator::Result::DensityError:
                // Restart computation with more points...

                // If nbpoints has changed (initial density was not the final density), change the density in the intakeevent
                // so it can be used if this intake is recalculated.
                //if (it->getNbPoints() != ink.getNbPoints()) {
                //    intake_series_t::index<tags::times>::type& intakes_times_index = intakes.get<tags::times>();
                //    intakes_times_index.modify(it, IntakeEvent::change_density(ink.nbPoints));
                //}
                break;
            default:
                //m_logger.error("Failed in calculation with given parameter values.");
                return ComputationResult::Failure;
        }


        // Apply the intra-individual error
        if (!_residualErrorModel.isEmpty()) {
            _residualErrorModel.applyEpsToArray(values, _epsilons);
        }

        // Append computed values to our prediction
        _prediction->appendConcentrations(times, values);

        // Prepare residuals for the next cycle
        r1 = r2;
    }

    return ComputationResult::Success;
}

ConcentrationCalculator::ComputationResult ConcentrationCalculator::computeConcentrationsAtTimes(
        Concentrations &_concentrations,
        const IntakeSeries &_intakes,
        const ParameterSetSeries &_parameterSets,
        const SampleSeries &_samples,
        const Etas &_etas)
{
    // First calculate the size of residuals
    unsigned int residualSize = 0;
    for (IntakeSeries::const_iterator it = _intakes.begin(); it != _intakes.end(); it++) {
        residualSize = std::max(residualSize, (*it).getCalculator()->getResidualSize());
    }
    Residuals r1(residualSize);
    Residuals r2(residualSize);

    //Allocates according to the number of samples
    _concentrations.reserve(_samples.size());

    IntakeSeries::const_iterator it, intakeEnd, intakeNext;

    intakeEnd = _intakes.end();
    intakeNext = it = _intakes.begin();
    intakeNext++;

    // The size of residuals vectors equals the number of compartments. This shouldnt be hardcoded here.
    SampleSeries::const_iterator sampleEnd = _samples.end();
    SampleSeries::const_iterator sit = _samples.begin();

    Value nextSampleTime = ((sit->getEventTime()).second()) / total_second_in_hour;
    // double _nextsampletime = intakes.begin()->time.secsTo(sit->time)/3600.0;

    Concentrations concentrations(residualSize);
    Concentrations::iterator cit = _concentrations.begin();

    while (it != intakeEnd && sit != sampleEnd) {

        // If there are samples, calculate cycles until there are no more samples or no more intakes
        // Get the offset time of the current intake from the first dose
	// (redundant because IntakeEvent now has offsettime)

        Value currentIntakeTime = ((it->getEventTime()).second()) / total_second_in_hour;
        // double _current = _intakes.begin()->time.secsTo(it->time) / total_second_in_hour;

        // Get the offset time from the first dose
        Value nextIntakeTime = currentIntakeTime + (it->getInterval()).toHours();

        // Get parameters at intake start time
        ParameterSetEventPtr parameters = _parameterSets.getAtTime(it->getEventTime(), _etas);
        if (parameters == nullptr) {
            //m_logger.error("No parameters found!");
            return ComputationResult::Failure;
        }

        // If the next sample time greater than the next intake time, 
	// the sample doesnt occur during this cycle, so we only care about residuals
        if (nextSampleTime > nextIntakeTime) {
            IntakeIntervalCalculator::Result result = it->calculateIntakeSinglePoint(concentrations, *it, *parameters, r1, currentIntakeTime, r2);

            if (result != IntakeIntervalCalculator::Result::Ok) {
                _concentrations.clear();

		return ComputationResult::Failure;
            }
	    // Prepare residuals for the next cycle
            r1 = r2;
        }

        while ((nextSampleTime >= currentIntakeTime) && (nextSampleTime <= nextIntakeTime)) {

            // If the next sample time greater than the cycle start time 
	    // and less than the next cycle start time, the sample occurs during this cycle. 
	    // We care about residuals and the value at the next sample time.
            IntakeIntervalCalculator::Result result = it->calculateIntakeSinglePoint(concentrations, *it, *parameters, r1, (nextSampleTime - currentIntakeTime), r2);

            if (result != IntakeIntervalCalculator::Result::Ok) {
                _concentrations.clear();

		return ComputationResult::Failure;
            }
	    // Prepare residuals for the next cycle
            r1 = r2;

            // Set the output concentration
            *cit = concentrations[0];

            // We processed a sample so increment samples and output concentrations iterators.
            cit++; sit++;

            if (sit == sampleEnd) {
                return ComputationResult::Success;
            }
            // Reset the next sample time
            nextSampleTime = ((sit->getEventTime()).second()) / total_second_in_hour;
        }
        it++; intakeNext++;
    }
    return ComputationResult::Success;
}

}
}
