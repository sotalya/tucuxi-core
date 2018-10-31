/*
* Copyright (C) 2017 Tucuxi SA
*/


#include <chrono>

#include "concentrationcalculator.h"

namespace Tucuxi {
namespace Core {

ComputationResult ConcentrationCalculator::computeConcentrations(ConcentrationPredictionPtr &_prediction,
    bool _isAll,
    const DateTime &_recordFrom,
    const DateTime &_recordTo,
    const IntakeSeries &_intakes,
    const ParameterSetSeries &_parameterSets,
    const Etas &_etas,
    const IResidualErrorModel &_residualErrorModel,
    const Deviations& _epsilons,
    bool _onlyAnalytes,
    bool _isFixedDensity)
{
    if (_recordFrom == DateTime()) {
        std::cout  << "Invalid record from" <<  std::endl;
    }
    if (_recordTo == DateTime()) {
        std::cout  << "Invalid record to" <<  std::endl;
    }
    TMP_UNUSED_PARAMETER(_onlyAnalytes);

    // First calculate the size of residuals
    unsigned int residualSize = 0;
    for (IntakeSeries::const_iterator it = _intakes.begin(); it != _intakes.end(); it++) {
        std::shared_ptr<IntakeIntervalCalculator> pCalculator = (*it).getCalculator();
        unsigned int s = pCalculator->getResidualSize();
        residualSize = std::max(residualSize, s);
    }

    Residuals inResiduals(residualSize);
    Residuals outResiduals(residualSize);

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

        DateTime intakeStartTime = intake.getEventTime();
        DateTime intakeEndTime = intakeStartTime + intake.getInterval();

        if ((intakeEndTime > _recordFrom) && (intakeStartTime < _recordTo)) {
            // Record data

            // Compute concentrations for the current cycle
            TimeOffsets times;
            std::vector<Concentrations> concentrations;
            concentrations.resize(residualSize);

            _prediction->allocate(residualSize, intake.getNbPoints(), times, concentrations);

    //        outResiduals.clear();

            IntakeIntervalCalculator::Result result = it->calculateIntakePoints(concentrations, times, intake, *parameters, inResiduals, _isAll, outResiduals, _isFixedDensity);

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
            if ((!_residualErrorModel.isEmpty()) && (_epsilons.size() != 0)) {
                _residualErrorModel.applyEpsToArray(concentrations[0], _epsilons);
            }

            // Append computed values to our prediction
            _prediction->appendConcentrations(times, concentrations[0]);
        }
        else {
            // Do not record data, only get residual

            // Compute concentrations for the current cycle
            TimeOffsets times;
            std::vector<Concentrations> concentrations;
            concentrations.resize(residualSize);

            _prediction->allocate(residualSize, intake.getNbPoints(), times, concentrations);

            IntakeIntervalCalculator::Result result = it->calculateIntakePoints(concentrations, times, intake, *parameters, inResiduals, _isAll, outResiduals, _isFixedDensity);

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



        }


        // Prepare residuals for the next cycle
        // NOTICE: "inResiduals = outResiduals" and "std::copy(outResiduals.begin(),
        // outResiduals.end(), inResiduals.begin())" are not working
        for(unsigned int i = 0; i < residualSize; i++)
            inResiduals[i] = outResiduals[i];
    }

    return ComputationResult::Success;
}

ComputationResult ConcentrationCalculator::computeConcentrationsAtTimes(Concentrations &_concentrations,
    bool _isAll,
    const IntakeSeries &_intakes,
    const ParameterSetSeries &_parameterSets,
    const SampleSeries &_samples,
    const Etas &_etas,
    bool _onlyAnalytes)
{
    TMP_UNUSED_PARAMETER(_onlyAnalytes);

    _concentrations.clear();

    // First calculate the size of residuals
    unsigned int residualSize = 0;
    for (IntakeSeries::const_iterator it = _intakes.begin(); it != _intakes.end(); it++) {
        residualSize = std::max(residualSize, (*it).getCalculator()->getResidualSize());
    }
    Residuals inResiduals(residualSize);
    Residuals outResiduals(residualSize);

    // Reset input residuals for the next cycle
    std::fill(inResiduals.begin(), inResiduals.end(), 0);


    std::vector<Concentrations> concentrations;
    concentrations.resize(residualSize);

    IntakeSeries::const_iterator it, intakeEnd, intakeNext;

    intakeEnd = _intakes.end();
    intakeNext = it = _intakes.begin();
    intakeNext++;

    // The size of residuals vectors equals the number of compartments. This shouldnt be hardcoded here.
    SampleSeries::const_iterator sampleEnd = _samples.end();
    SampleSeries::const_iterator sit = _samples.begin();

    DateTime nextSampleTime = sit->getEventTime();
    // double _nextsampletime = intakes.begin()->time.secsTo(sit->time)/3600.0;

    while (it != intakeEnd && sit != sampleEnd) {
        // If there are samples, calculate cycles until there are no more samples or no more intakes
        // Get the offset time of the current intake from the first dose
        // (redundant because IntakeEvent now has offsettime)

        DateTime currentIntakeTime = it->getEventTime();
        // double _current = _intakes.begin()->time.secsTo(it->time) / total_second_in_hour;

        // Get the offset time from the first dose
        DateTime nextIntakeTime = currentIntakeTime + it->getInterval();

        // Get parameters at intake start time
        ParameterSetEventPtr parameters = _parameterSets.getAtTime(it->getEventTime(), _etas);
        if (parameters == nullptr) {
            //m_logger.error("No parameters found!");
            return ComputationResult::Failure;
        }

        // If the next sample time greater than the next intake time, 
        // the sample doesnt occur during this cycle, so we only care about residuals
        // clear locally defined concentration
        for (unsigned int idx= 0; idx < residualSize; idx++) {
            concentrations[idx].clear();
        }

        if (nextSampleTime > nextIntakeTime) {

            IntakeIntervalCalculator::Result result = it->calculateIntakeSinglePoint(
                concentrations,
                *it,
                *parameters,
                inResiduals,
                // We only need the residuals, so we don't care about a specific time
                0.0,
                _isAll,
                outResiduals);

            if (result != IntakeIntervalCalculator::Result::Ok) {
                _concentrations.clear();
                return ComputationResult::Failure;
            }

            // Reset input residuals for the next cycle
            inResiduals = outResiduals;
        }

        // If the next sample time greater than the cycle start time 
        // and less than the next cycle start time, the sample occurs during this cycle.
        // We care about residuals and the value at the next sample time.
        if ((nextSampleTime >= currentIntakeTime) && (nextSampleTime <= nextIntakeTime)) {
            while ((nextSampleTime >= currentIntakeTime) && (nextSampleTime <= nextIntakeTime)) {

                Duration atTime = nextSampleTime - currentIntakeTime;

                // clear locally defined concentration
                for (unsigned int idx= 0; idx < residualSize; idx++) {
                    concentrations[idx].clear();
                }

                IntakeIntervalCalculator::Result result =
                it->calculateIntakeSinglePoint(concentrations, *it, *parameters, inResiduals, atTime.toHours(), _isAll, outResiduals);

                if (result != IntakeIntervalCalculator::Result::Ok) {
                    _concentrations.clear();
                    return ComputationResult::Failure;
                }

                _concentrations.push_back(concentrations[0][0]);

                // We processed a sample so increment samples and output concentrations iterators.
                sit++;

                if (sit == sampleEnd) {
                    return ComputationResult::Success;
                }

                // Reset the next sample time
                nextSampleTime = sit->getEventTime();
            }

            inResiduals = outResiduals;
        }
        it++; intakeNext++;
    }
    return ComputationResult::Success;
}

} // namespace Core
} // namespace Tucuxi
