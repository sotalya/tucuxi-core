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


#ifndef TUCUXI_CORE_MULTICONCENTRATIONCALCULATOR_H
#define TUCUXI_CORE_MULTICONCENTRATIONCALCULATOR_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/multiconcentrationprediction.h"
#include "tucucore/parameter.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/sampleevent.h"

namespace Tucuxi {
namespace Core {

class IMultiConcentrationCalculator
{
public:
    ///
    /// \brief computeConcentrations
    /// \param _prediction The calculated concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes The intakes series
    /// \param _parameters The parameters series
    /// \param _etas The etas to apply to the parameters
    /// \param _residualErrorModels The residual error models to use with the epsilons
    /// \param _eps The epsilon to modify the final concentrations
    /// \param _isFixedDensity Indicates if the density of points could be changed within the method
    /// \param _onlyAnalytes If true, only fill the concentration of the analyte, no other compartment
    /// \return The status of computation
    ///
    /// The predictions are composed of a vector containing intakes calculations.
    /// Each intake calculation is a vector of compartments predictions,
    /// and each compartment prediction is a vector of concentrations.
    ///
    /// The _residualErrorModels is a vector of IResidualErrorModel. If not empty, they will be applied
    /// to the first compartments (0, 1, ...). The size of this vector should be equal to the size
    /// of _espsilons.
    ///
    virtual ComputingStatus computeConcentrations(
            const MultiConcentrationPredictionPtr& _prediction,
            bool _isAll,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const Etas& _etas = Etas(0),
            const std::vector<std::unique_ptr<IResidualErrorModel> >& _residualErrorModels = {},
            const std::vector<Deviations>& _epsilons = {},
            bool _onlyAnalytes = true,
            bool _isFixedDensity = false) = 0;

    ///
    /// \brief computeConcentrationsAtSteadyState
    /// \param _prediction The calculated concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes The intakes series
    /// \param _parameters The parameters series
    /// \param _etas The etas to apply to the parameters
    /// \param _residualErrorModels The residual error models to use with the epsilons
    /// \param _eps The epsilon to modify the final concentrations
    /// \param _isFixedDensity Indicates if the density of points could be changed within the method
    /// \param _onlyAnalytes If true, only fill the concentration of the analyte, no other compartment
    /// \return The status of computation
    ///
    /// The predictions are composed of a vector containing intakes calculations.
    /// Each intake calculation is a vector of compartments predictions,
    /// and each compartment prediction is a vector of concentrations.
    ///
    /// The _residualErrorModels is a vector of IResidualErrorModel. If not empty, they will be applied
    /// to the first compartments (0, 1, ...). The size of this vector should be equal to the size
    /// of _espsilons.
    ///
    virtual ComputingStatus computeConcentrationsAtSteadyState(
            const MultiConcentrationPredictionPtr& _prediction,
            bool _isAll,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const Etas& _etas = Etas(0),
            const std::vector<std::unique_ptr<IResidualErrorModel> >& _residualErrorModels = {},
            const std::vector<Deviations>& _epsilons = {},
            bool _onlyAnalytes = true,
            bool _isFixedDensity = false) = 0;

    ///
    /// \brief Calculates concentrations at specific times
    /// It just calculates the final residuals for the next cycle,
    /// and the values at the time points requested. If the eta vector is not empty, then
    /// the etas are applied to the parameters.
    /// \param _concentrations concentrations 2d vector allocated within
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _intakes intakes for entire curve
    /// \param _parameters parameters for entire curve
    /// \param _samples samples (measures) for the entire curve
    /// \param _etas vector of etas
    /// \param _onlyAnalytes If true, only fill the concentration of the analyte, no other compartment
    /// \return The status of computation
    ///
    virtual ComputingStatus computeConcentrationsAtTimes(
            MultiCompConcentrations& _concentrations,
            bool _isAll,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const SampleSeries& _samples,
            const Etas& _etas = Etas(0),
            bool _onlyAnalytes = true) = 0;

    /// \brief virtual empty destructor
    virtual ~IMultiConcentrationCalculator() = default;
};



class MultiConcentrationCalculator : public IMultiConcentrationCalculator
{
public:
    ///
    /// \brief computeConcentrations
    /// \param _prediction The calculated concentrations
    /// \param _intakes The intakes series
    /// \param _parameters The parameters series
    /// \param _etas The etas to apply to the parameters
    /// \param _residualErrorModels The residual error models to use with the epsilons
    /// \param _eps The epsilon to modify the final concentrations
    /// \param _isFixedDensity Indicates if the density of points could be changed within the method
    /// \return The status of computation
    ///
    /// The predictions are composed of a vector containing intakes calculations.
    /// Each intake calculation is a vector of compartments predictions,
    /// and each compartment prediction is a vector of concentrations.
    ///
    /// The _residualErrorModels is a vector of IResidualErrorModel. If not empty, they will be applied
    /// to the first compartments (0, 1, ...). The size of this vector should be equal to the size
    /// of _espsilons.
    ///
    ComputingStatus computeConcentrations(
            const MultiConcentrationPredictionPtr& _prediction,
            bool _isAll,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const Etas& _etas = Etas(0),
            const std::vector<std::unique_ptr<IResidualErrorModel> >& _residualErrorModels = {},
            const std::vector<Deviations>& _epsilons = {},
            bool _onlyAnalytes = true,
            bool _isFixedDensity = false) override;

    ///
    /// \brief computeConcentrationsAtSteadyState
    /// \param _prediction The calculated concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes The intakes series
    /// \param _parameters The parameters series
    /// \param _etas The etas to apply to the parameters
    /// \param _residualErrorModels The residual error models to use with the epsilons
    /// \param _eps The epsilon to modify the final concentrations
    /// \param _isFixedDensity Indicates if the density of points could be changed within the method
    /// \param _onlyAnalytes If true, only fill the concentration of the analyte, no other compartment
    /// \return The status of computation
    ///
    /// The predictions are composed of a vector containing intakes calculations.
    /// Each intake calculation is a vector of compartments predictions,
    /// and each compartment prediction is a vector of concentrations.
    ///
    /// The _residualErrorModels is a vector of IResidualErrorModel. If not empty, they will be applied
    /// to the first compartments (0, 1, ...). The size of this vector should be equal to the size
    /// of _espsilons.
    ///
    ComputingStatus computeConcentrationsAtSteadyState(
            const MultiConcentrationPredictionPtr& _prediction,
            bool _isAll,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const Etas& _etas = Etas(0),
            const std::vector<std::unique_ptr<IResidualErrorModel> >& _residualErrorModels = {},
            const std::vector<Deviations>& _epsilons = {},
            bool _onlyAnalytes = true,
            bool _isFixedDensity = false) override;


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
    ComputingStatus computeConcentrationsAtTimes(
            MultiCompConcentrations& _concentrations,
            bool _isAll,
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const SampleSeries& _samples,
            const Etas& _etas = Etas(0),
            bool _onlyAnalytes = true) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_MULTICONCENTRATIONCALCULATOR_H
