/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <chrono>

#include "concentrationcalculator.h"

#include "tucucommon/loggerhelper.h"

namespace Tucuxi {
namespace Core {

ComputingStatus ConcentrationCalculator::computeConcentrations(
        const ConcentrationPredictionPtr& _prediction,
        bool _isAll,
        const DateTime& _recordFrom,
        const DateTime& _recordTo,
        const IntakeSeries& _intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas,
        const IResidualErrorModel& _residualErrorModel,
        const Deviations& _epsilons,
        bool _onlyAnalytes,
        bool _isFixedDensity)
{
    if (_recordFrom.isUndefined()) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Invalid record from");
    }
    if (_recordTo.isUndefined()) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Invalid record to");
    }

    TMP_UNUSED_PARAMETER(_onlyAnalytes);

    // First calculate the size of residuals
    unsigned int residualSize = 0;
    for (const auto& intake : _intakes) {
        std::shared_ptr<IntakeIntervalCalculator> pCalculator = intake.getCalculator();
        unsigned int s = pCalculator->getResidualSize();
        residualSize = std::max(residualSize, s);
    }

    Residuals inResiduals(residualSize);
    Residuals outResiduals(residualSize);

    // Go through all intakes
    for (const auto& intake : _intakes) {

        // Get parameters at intake start time
        // For population calculation, could be done only once at the beginning
        ParameterSetEventPtr parameters = _parameters.getAtTime(intake.getEventTime(), _etas);
        if (parameters == nullptr) {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("No parameters found!");
            return ComputingStatus::ConcentrationCalculatorNoParameters;
        }

        DateTime intakeStartTime = intake.getEventTime();
        DateTime intakeEndTime = intakeStartTime + intake.getInterval();

        if ((intakeEndTime > _recordFrom) && (intakeStartTime < _recordTo)) {
            // Record data

            // Compute concentrations for the current cycle
            TimeOffsets times;
            MultiCompConcentrations concentrations;
            concentrations.resize(residualSize);

            _prediction->allocate(residualSize, intake.getNbPoints(), times, concentrations);

            //        outResiduals.clear();

            ComputingStatus result = intake.calculateIntakePoints(
                    concentrations, times, intake, *parameters, inResiduals, _isAll, outResiduals, _isFixedDensity);

            switch (result) {
            case ComputingStatus::Ok:
                break;
                // case ComputingStatus::DensityError:
                // Restart computation with more points...

                // If nbpoints has changed (initial density was not the final density), change the density in the intakeevent
                // so it can be used if this intake is recalculated.
                //if (it->getNbPoints() != ink.getNbPoints()) {
                //    intake_series_t::index<tags::times>::type& intakes_times_index = intakes.get<tags::times>();
                //    intakes_times_index.modify(it, IntakeEvent::change_density(ink.nbPoints));
                //}
            //    break;
            default:
                //m_logger.error("Failed in calculation with given parameter values.");
                return result;
            }


            // Apply the intra-individual error
            if ((!_residualErrorModel.isEmpty()) && (!_epsilons.empty())) {
                _residualErrorModel.applyEpsToArray(concentrations[0], _epsilons);
            }

            // Append computed values to our prediction
            _prediction->appendConcentrations(times, concentrations[0]);
        }
        else {
            // Do not record data, only get residual

            // Compute concentrations for the current cycle
            TimeOffsets times;
            MultiCompConcentrations concentrations;
            concentrations.resize(residualSize);

            _prediction->allocate(residualSize, intake.getNbPoints(), times, concentrations);

            ComputingStatus result = intake.calculateIntakePoints(
                    concentrations, times, intake, *parameters, inResiduals, _isAll, outResiduals, _isFixedDensity);

            switch (result) {
            case ComputingStatus::Ok:
                break;
                // case ComputingStatus::DensityError:
                // Restart computation with more points...

                // If nbpoints has changed (initial density was not the final density), change the density in the intakeevent
                // so it can be used if this intake is recalculated.
                //if (it->getNbPoints() != ink.getNbPoints()) {
                //    intake_series_t::index<tags::times>::type& intakes_times_index = intakes.get<tags::times>();
                //    intakes_times_index.modify(it, IntakeEvent::change_density(ink.nbPoints));
                //}
            //    break;
            default:
                //m_logger.error("Failed in calculation with given parameter values.");
                return result;
            }
        }


        // Prepare residuals for the next cycle
        // NOTICE: "inResiduals = outResiduals" and "std::copy(outResiduals.begin(),
        // outResiduals.end(), inResiduals.begin())" are not working
        for (unsigned int i = 0; i < residualSize; i++) {
            inResiduals[i] = outResiduals[i];
        }
    }

    return ComputingStatus::Ok;
}

ComputingStatus ConcentrationCalculator::computeConcentrationsAtSteadyState(
        const ConcentrationPredictionPtr& _prediction,
        bool _isAll,
        const DateTime& _recordFrom,
        const DateTime& _recordTo,
        const IntakeSeries& _intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas,
        const IResidualErrorModel& _residualErrorModel,
        const Deviations& _epsilons,
        bool _onlyAnalytes,
        bool _isFixedDensity)
{
    if (_recordFrom.isUndefined()) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Invalid record from");
    }
    if (_recordTo.isUndefined()) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("Invalid record to");
    }

    TMP_UNUSED_PARAMETER(_onlyAnalytes);

    // First calculate the size of residuals
    unsigned int residualSize = 0;
    for (const auto& intake : _intakes) {
        std::shared_ptr<IntakeIntervalCalculator> pCalculator = intake.getCalculator();
        unsigned int s = pCalculator->getResidualSize();
        residualSize = std::max(residualSize, s);
    }

    Residuals inResiduals(residualSize);
    Residuals outResiduals(residualSize);

    bool reachedSteadyState = false;
    bool finished = false;

    int nbIterations = 0;

    while (!finished) {
        // Go through all intakes
        for (const auto& intake : _intakes) {

            // Get parameters at intake start time
            // For population calculation, could be done only once at the beginning
            ParameterSetEventPtr parameters = _parameters.getAtTime(intake.getEventTime(), _etas);
            if (parameters == nullptr) {
                Tucuxi::Common::LoggerHelper logHelper;
                logHelper.error("No parameters found!");
                return ComputingStatus::ConcentrationCalculatorNoParameters;
            }

            if (reachedSteadyState) {

                // Compute concentrations for the current cycle
                TimeOffsets times;
                MultiCompConcentrations concentrations;
                concentrations.resize(residualSize);

                _prediction->allocate(residualSize, intake.getNbPoints(), times, concentrations);

                ComputingStatus result = intake.calculateIntakePoints(
                        concentrations, times, intake, *parameters, inResiduals, _isAll, outResiduals, _isFixedDensity);

                switch (result) {
                case ComputingStatus::Ok:
                    break;
                    //case ComputingStatus::DensityError:
                    // Restart computation with more points...

                    // If nbpoints has changed (initial density was not the final density), change the density in the intakeevent
                    // so it can be used if this intake is recalculated.
                    //if (it->getNbPoints() != ink.getNbPoints()) {
                    //    intake_series_t::index<tags::times>::type& intakes_times_index = intakes.get<tags::times>();
                    //    intakes_times_index.modify(it, IntakeEvent::change_density(ink.nbPoints));
                    //}
                //    break;
                default:
                    //m_logger.error("Failed in calculation with given parameter values.");
                    return result;
                }


                // Apply the intra-individual error
                if ((!_residualErrorModel.isEmpty()) && (!_epsilons.empty())) {
                    _residualErrorModel.applyEpsToArray(concentrations[0], _epsilons);
                }

                // Append computed values to our prediction
                _prediction->appendConcentrations(times, concentrations[0]);
            }
            else {
                // Do not record data, only get residual

                // Compute concentrations for the current cycle
                TimeOffsets times;
                MultiCompConcentrations concentrations;
                concentrations.resize(residualSize);

                _prediction->allocate(residualSize, intake.getNbPoints(), times, concentrations);

                ComputingStatus result = intake.calculateIntakePoints(
                        concentrations, times, intake, *parameters, inResiduals, _isAll, outResiduals, _isFixedDensity);

                switch (result) {
                case ComputingStatus::Ok:
                    break;
                    //case ComputingStatus::DensityError:
                    // Restart computation with more points...

                    // If nbpoints has changed (initial density was not the final density), change the density in the intakeevent
                    // so it can be used if this intake is recalculated.
                    //if (it->getNbPoints() != ink.getNbPoints()) {
                    //    intake_series_t::index<tags::times>::type& intakes_times_index = intakes.get<tags::times>();
                    //    intakes_times_index.modify(it, IntakeEvent::change_density(ink.nbPoints));
                    //}
                //    break;
                default:
                    //m_logger.error("Failed in calculation with given parameter values.");
                    return result;
                }
            }

            if (reachedSteadyState) {
                finished = true;
            }

            reachedSteadyState = true;
            for (unsigned int i = 0; i < residualSize; i++) {
                if (std::fabs(inResiduals[i] - outResiduals[i]) > 0.0001) {
                    reachedSteadyState = false;
                }
            }

            // Prepare residuals for the next cycle
            // NOTICE: "inResiduals = outResiduals" and "std::copy(outResiduals.begin(),
            // outResiduals.end(), inResiduals.begin())" are not working
            for (unsigned int i = 0; i < residualSize; i++) {
                inResiduals[i] = outResiduals[i];
            }
        }

        nbIterations++;
        if (nbIterations > 1000) {
            return ComputingStatus::NoSteadyState;
        }
    }


    return ComputingStatus::Ok;
}


ComputingStatus ConcentrationCalculator::computeConcentrationsAtTimes(
        Concentrations& _concentrations,
        bool _isAll,
        const IntakeSeries& _intakes,
        const ParameterSetSeries& _parameters,
        const SampleSeries& _samples,
        const Etas& _etas,
        bool _onlyAnalytes)
{
    TMP_UNUSED_PARAMETER(_onlyAnalytes);

    _concentrations.clear();

    size_t nbSamplesOut = 0;
    for (const auto& sample : _samples) {
        if ((sample.getEventTime() < _intakes.at(0).getEventTime())
            || (sample.getEventTime()
                > _intakes.at(_intakes.size() - 1).getEventTime() + _intakes.at(_intakes.size() - 1).getInterval())) {
            nbSamplesOut++;
        }
    }
    if (nbSamplesOut == _samples.size()) {
        // If there is no sample on the intake series time, then something goes wrong
        return ComputingStatus::AposterioriPercentilesOutOfScopeSamplesError;
    }

    // First calculate the size of residuals
    unsigned int residualSize = 0;
    for (const auto& intake : _intakes) {
        residualSize = std::max(residualSize, intake.getCalculator()->getResidualSize());
    }
    Residuals inResiduals(residualSize);
    Residuals outResiduals(residualSize);

    // Reset input residuals for the next cycle
    std::fill(inResiduals.begin(), inResiduals.end(), 0);


    MultiCompConcentrations concentrations;
    concentrations.resize(residualSize);

    IntakeSeries::const_iterator it;
    IntakeSeries::const_iterator intakeEnd;

    intakeEnd = _intakes.end();
    it = _intakes.begin();

    // The size of residuals vectors equals the number of compartments. This shouldnt be hardcoded here.
    auto sampleEnd = _samples.end();
    auto sit = _samples.begin();

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
        ParameterSetEventPtr parameters = _parameters.getAtTime(it->getEventTime(), _etas);
        if (parameters == nullptr) {
            //m_logger.error("No parameters found!");
            return ComputingStatus::ConcentrationCalculatorNoParameters;
        }

        // If the next sample time greater than the next intake time,
        // the sample doesnt occur during this cycle, so we only care about residuals
        // clear locally defined concentration
        for (unsigned int idx = 0; idx < residualSize; idx++) {
            concentrations[idx].clear();
        }

        if (nextSampleTime > nextIntakeTime) {

            ComputingStatus result = it->calculateIntakeSinglePoint(
                    concentrations,
                    *it,
                    *parameters,
                    inResiduals,
                    // We only need the residuals, so we don't care about a specific time
                    0.0,
                    _isAll,
                    outResiduals);

            if (result != ComputingStatus::Ok) {
                _concentrations.clear();
                return result;
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
                for (unsigned int idx = 0; idx < residualSize; idx++) {
                    concentrations[idx].clear();
                }

                ComputingStatus result = it->calculateIntakeSinglePoint(
                        concentrations, *it, *parameters, inResiduals, atTime.toHours(), _isAll, outResiduals);

                if (result != ComputingStatus::Ok) {
                    _concentrations.clear();
                    return result;
                }

                _concentrations.push_back(concentrations[0][0]);

                // We processed a sample so increment samples and output concentrations iterators.
                sit++;

                if (sit == sampleEnd) {
                    return ComputingStatus::Ok;
                }

                // Reset the next sample time
                nextSampleTime = sit->getEventTime();
            }

            inResiduals = outResiduals;
        }
        it++;
    }
    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
