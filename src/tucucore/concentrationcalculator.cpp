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
    const ResidualErrorModel &_residualErrorModel,
    const Deviation& _eps,
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
            _residualErrorModel.applyEpsToArray(values, _eps);
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
        const ParameterSetSeries &_parameters,
        const SampleSeries &_samples,
        const Etas &_etas)
{
    TMP_UNUSED_PARAMETER(_concentrations);
    TMP_UNUSED_PARAMETER(_intakes);
    TMP_UNUSED_PARAMETER(_parameters);
    TMP_UNUSED_PARAMETER(_samples);
    TMP_UNUSED_PARAMETER(_etas);

    return ComputationResult::Failure;
}

}
}
