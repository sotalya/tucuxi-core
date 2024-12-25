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


#ifndef TUCUXI_CORE_COMPUTINGRESULT_H
#define TUCUXI_CORE_COMPUTINGRESULT_H

#include <iostream>


namespace Tucuxi {
namespace Core {

///
/// \brief The ComputingStatus enum
/// Describes the return value of a computation
enum class [[nodiscard]] ComputingStatus{
        /// Not yet defined
        Undefined = -1,
        /// Everything went well
        Ok = 0,
        /// Too Big computation required
        TooBig,
        /// Computation aborted by the external agent
        Aborted,
        /// Error in the extraction of parameters
        ParameterExtractionError,
        /// Error in the extraction of samples
        SampleExtractionError,
        /// Error in target extraction
        TargetExtractionError,
        /// The candidate is outside the target range. This is not an error, but more a status
        InvalidCandidate,
        /// There was an internal error during target evaluation
        TargetEvaluationError,
        /// There was an error during covariate extraction
        CovariateExtractionError,
        /// There was an error during intake extraction
        IntakeExtractionError,
        /// There was an error during residual error model extraction
        ErrorModelExtractionError,
        /// The IntakeToCalculatorAssociator could not find a suitable route
        UnsupportedRoute,
        /// Error during conversion of analytes
        AnalyteConversionError,
        /// A posteriori percentiles calculation, but no samples
        AposterioriPercentilesNoSamplesError,
        /// The ConcentrationCalculator did not find parameters
        ConcentrationCalculatorNoParameters,
        /// Bad paramters in the intake calculator
        BadParameters,
        /// Bad concentration detected in the intake calculator
        BadConcentration,
        /// Density error in the intake calculator
        DensityError,
        /// In a posteriori Etas calculation, Omega is empty
        AposterioriEtasCalculationEmptyOmega,
        /// In a posteriori Etas calculation, Omega is not a square matrix
        AposterioriEtasCalculationNoSquareOmega,
        /// ComputingTraitStandard::compute() should not be called
        ComputingTraitStandardShouldNotBeCalled,
        /// Could not find a suitable formulation and route
        CouldNotFindSuitableFormulationAndRoute,
        /// Multiple formulation and routes are not yet supported
        MultipleFormulationAndRoutesNotSupported,
        /// Could not find a suitable Pk Model
        NoPkModelError,
        /// An exception was raised in ComputingComponent
        ComputingComponentExceptionError,
        /// No Pk Models in the collection
        NoPkModels,
        /// The ComputingTraits sent for computation are nullptr
        NoComputingTraits,
        /// The recorded intakes size does not fit the predictions
        RecordedIntakesSizeError,
        /// No percentile calculation was performed
        NoPercentilesCalculation,
        /// The selected intakes size is not correct
        SelectedIntakesSizeError,
        /// No available dose
        NoAvailableDose,
        /// No available interval
        NoAvailableInterval,
        /// No available infusion time
        NoAvailableInfusionTime,
        /// No formulation and route available for adjustments
        NoFormulationAndRouteForAdjustment,
        /// The concentration vector has not a correct size
        ConcentrationSizeError,
        /// Error during calculation of active moiety concentration value
        ActiveMoietyCalculationError,
        /// There are no analytes groups
        NoAnalytesGroup,
        /// The drug model is incompatible with the drug treatment
        IncompatibleTreatmentModel,
        /// The computing component has not been initialized
        ComputingComponentNotInitialized,
        /// The drug domain is uncompatible with the patient covariates
        UncompatibleDrugDomain,
        /// Cannot attain steady state
        NoSteadyState,
        /// A posteriori percentiles calculation, but samples are out of scope
        AposterioriPercentilesOutOfScopeSamplesError,
        /// Something went wrong with adjustments
        AdjustmentsInternalError,
        /// An exception was raised in MultiComputingComponent
        MultiComputingComponentExceptionError,
        /// The multicomputing component has not been initialized
        MultiComputingComponentNotInitialized,
        /// Error during calculation of multi active moiety concentration values
        MultiActiveMoietyCalculationError,
        /// Error during percentiles calculation. Caused by no valid prediction being computed
        PercentilesNoValidPrediction,
        /// There is no likely samples, so the a posteriori percentiles can not be computed
        AposterioriPercentilesNoLikelySample,
        /// The dosage history is empty, but should not be
        NoDosageHistory,
        /// There is at least one sample before the treatment start
        SampleBeforeTreatmentStart,
        /// At least one percentile rank is out of bounds
        OutOfBoundsPercentileRank,
};

std::ostream& operator<<(std::ostream& _stream, const ComputingStatus& _e);

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGRESULT_H
