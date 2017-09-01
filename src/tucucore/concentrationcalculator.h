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



class IConcentrationCalculatorNew
{

public:

    enum class ComputationResult { Success, Failure, Aborted };

    ///
    /// \brief computeConcentrations
    /// \param _prediction The calculated concentrations
    /// \param _nbPoints The number of points asked
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes The intakes series
    /// \param _parameters The parameters series
    /// \param _etas The etas to apply to the parameters
    /// \param _residualErrorModel The residual error model to use with the epsilons
    /// \param _eps The epsilon to modify the final concentrations
    /// \param _isFixedDensity Indicates if the density of points could be changed within the method
    /// \param _onlyAnalytes If true, only fill the concentration of the analyte, no other compartment
    /// \return The status of computation
    ///
    virtual ComputationResult computeConcentrations(
            ConcentrationPredictionPtr &_prediction,
            int _nbPoints,
            const DateTime &_recordFrom,
            const DateTime &_recordTo,
            const IntakeSeries &_intakes,
            const ParameterSetSeries& _parameters,
            const Etas& _etas = Etas(0),
            const IResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
            const Deviations& _epsilons = Deviations(0),
            const bool _onlyAnalytes = true,
            const bool _isFixedDensity = 0) = 0;

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
    /// \param _onlyAnalytes If true, only fill the concentration of the analyte, no other compartment
    /// \return The status of computation
    ///
    virtual ComputationResult computeConcentrationsAtTimes(
            Concentrations& _concentrations,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const SampleSeries& _samples,
            const Etas& _etas = Etas(0),
            const bool _onlyAnalytes = true) = 0;

    /// \brief virtual empty destructor
    virtual ~IConcentrationCalculatorNew() {}
};








#define total_second_in_hour 3600.0

class IConcentrationCalculator
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
    virtual ComputationResult computeConcentrations(
            ConcentrationPredictionPtr &_prediction,
            int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries& _parameters,
            const Etas& _etas = Etas(0),
            const IResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
            const Deviations& _epsilons = Deviations(0),
            const bool _isFixedDensity = 0) = 0;

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
    virtual ComputationResult computeConcentrationsAtTimes(
            Concentrations& _concentrations,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const SampleSeries& _samples,
            const Etas& _etas = Etas(0)) = 0;

    /// \brief virtual empty destructor
    virtual ~IConcentrationCalculator() {}
};

class ConcentrationCalculator : public IConcentrationCalculator
{
public:

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
    virtual ComputationResult computeConcentrations(
            ConcentrationPredictionPtr &_prediction,
            int _nbPoints,
            const IntakeSeries &_intakes,
            const ParameterSetSeries& _parameters,
            const Etas& _etas = Etas(0),
            const IResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
            const Deviations& _epsilons = Deviations(0),
            const bool _isFixedDensity = 0) override;

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
    virtual ComputationResult computeConcentrationsAtTimes(
            Concentrations& _concentrations,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const SampleSeries& _samples,
            const Etas& _etas = Etas(0)) override;

};

}
}

#endif // TUCUXI_CORE_CONCENTRATIONCALCULATOR_H
