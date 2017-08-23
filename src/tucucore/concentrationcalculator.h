/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CONCENTRATIONCALCULATOR_H
#define TUCUXI_CORE_CONCENTRATIONCALCULATOR_H

#include "tucucore/definitions.h"
#include "tucucore/parameter.h"
#include "tucucore/dosage.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/sample.h"

// For ConcentrationPredictionPtr. This class should be declared somewhere else
#include "tucucore/iprocessingservices.h"

namespace Tucuxi {
namespace Core {



class ConcentrationCalculator
{
public:


    enum class ComputationResult { Success, Failure, Aborted };

    ///
    /// \brief computeConcentrations
    /// \param _prediction The calculated concentrations
    /// \param _nbPoints The number of points asked
    /// \param _intakes The intakes series
    /// \param _parameters The parameters series
    /// \param _etas The etas to apply to the parameters
    /// \param _residualErrorModel The residual error model to use with the epsilons
    /// \param _eps The epsilon to modify the final concentrations
    /// \param _isFixedDensity Indicates if the density of points could be changed within the method
    /// \return The status of computation
    ///
    static ComputationResult computeConcentrations(
        ConcentrationPredictionPtr &_prediction,
        int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas = Etas(0),
        const IResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
        const Deviation& _eps = 0,
        const bool _isFixedDensity = 0);

    ///
    /// \brief Calculates concentrations at specific times
    /// It just calculates the final residuals for the next cycle,
    /// and the values at the time points requested. If the eta vector is not empty, then
    /// the etas are applied to the parameters.
    /// \param _concentrations concentrations 2d vector allocated within
    /// \param _intakes intakes for entire curve
    /// \param _parameters parameters for entire curve
    /// \param _samples samples (measures) for the entire curve
    /// \param _etas vector of etas
    /// \return The status of computation
    ///
    static ComputationResult computeConcentrationsAtTimes(Concentrations& _concentrations,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const SampleSeries& _samples,
            const Etas& _etas = Etas(0));

};

}
}

#endif // TUCUXI_CORE_CONCENTRATIONCALCULATOR_H
