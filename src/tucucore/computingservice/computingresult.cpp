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


#include <iostream>
#include <map>

#include "computingresult.h"

namespace Tucuxi {
namespace Core {

std::ostream& operator<<(std::ostream& _stream, const ComputingStatus& _e)
{
    return _stream << static_cast<typename std::underlying_type<ComputingStatus>::type>(_e);
}



std::string errorMessage(Tucuxi::Core::ComputingStatus _computingStatus)
{
    // Duplicate of what we have in the QueryResponseExport
    static std::map<Tucuxi::Core::ComputingStatus, std::pair<std::string, std::string>> m = {
            {Tucuxi::Core::ComputingStatus::Ok, {"Ok", "0"}},
            {Tucuxi::Core::ComputingStatus::Undefined, {"Undefined", "-1"}},
            {Tucuxi::Core::ComputingStatus::TooBig, {"TooBig", "1"}},
            {Tucuxi::Core::ComputingStatus::Aborted, {"Aborted", "2"}},
            {Tucuxi::Core::ComputingStatus::ParameterExtractionError, {"ParameterExtractionError", "3"}},
            {Tucuxi::Core::ComputingStatus::SampleExtractionError, {"SampleExtractionError", "4"}},
            {Tucuxi::Core::ComputingStatus::TargetExtractionError, {"TargetExtractionError", "5"}},
            {Tucuxi::Core::ComputingStatus::InvalidCandidate, {"InvalidCandidate", "6"}},
            {Tucuxi::Core::ComputingStatus::TargetEvaluationError, {"TargetEvaluationError", "7"}},
            {Tucuxi::Core::ComputingStatus::CovariateExtractionError, {"CovariateExtractionError", "8"}},
            {Tucuxi::Core::ComputingStatus::IntakeExtractionError, {"IntakeExtractionError", "9"}},
            {Tucuxi::Core::ComputingStatus::ErrorModelExtractionError, {"ErrorModelExtractionError", "10"}},
            {Tucuxi::Core::ComputingStatus::UnsupportedRoute, {"UnsupportedRoute", "11"}},
            {Tucuxi::Core::ComputingStatus::AnalyteConversionError, {"AnalyteConversionError", "12"}},
            {Tucuxi::Core::ComputingStatus::AposterioriPercentilesNoSamplesError,
             {"AposterioriPercentilesNoSamplesError", "13"}},
            {Tucuxi::Core::ComputingStatus::ConcentrationCalculatorNoParameters,
             {"ConcentrationCalculatorNoParameters", "14"}},
            {Tucuxi::Core::ComputingStatus::BadParameters, {"BadParameters", "15"}},
            {Tucuxi::Core::ComputingStatus::BadConcentration, {"BadConcentration", "16"}},
            {Tucuxi::Core::ComputingStatus::DensityError, {"DensityError", "17"}},
            {Tucuxi::Core::ComputingStatus::AposterioriEtasCalculationEmptyOmega,
             {"AposterioriEtasCalculationEmptyOmega", "18"}},
            {Tucuxi::Core::ComputingStatus::AposterioriEtasCalculationNoSquareOmega,
             {"AposterioriEtasCalculationNoSquareOmega", "19"}},
            {Tucuxi::Core::ComputingStatus::ComputingTraitStandardShouldNotBeCalled,
             {"ComputingTraitStandardShouldNotBeCalled", "20"}},
            {Tucuxi::Core::ComputingStatus::CouldNotFindSuitableFormulationAndRoute,
             {"CouldNotFindSuitableFormulationAndRoute", "21"}},
            {Tucuxi::Core::ComputingStatus::MultipleFormulationAndRoutesNotSupported,
             {"MultipleFormulationAndRoutesNotSupported", "22"}},
            {Tucuxi::Core::ComputingStatus::NoPkModelError, {"NoPkModelError", "23"}},
            {Tucuxi::Core::ComputingStatus::ComputingComponentExceptionError,
             {"ComputingComponentExceptionError", "24"}},
            {Tucuxi::Core::ComputingStatus::NoPkModels, {"NoPkModels", "25"}},
            {Tucuxi::Core::ComputingStatus::NoComputingTraits, {"NoComputingTraits", "26"}},
            {Tucuxi::Core::ComputingStatus::RecordedIntakesSizeError, {"RecordedIntakesSizeError", "27"}},
            {Tucuxi::Core::ComputingStatus::NoPercentilesCalculation, {"NoPercentilesCalculation", "28"}},
            {Tucuxi::Core::ComputingStatus::SelectedIntakesSizeError, {"SelectedIntakesSizeError", "29"}},
            {Tucuxi::Core::ComputingStatus::NoAvailableDose, {"NoAvailableDose", "30"}},
            {Tucuxi::Core::ComputingStatus::NoAvailableInterval, {"NoAvailableInterval", "31"}},
            {Tucuxi::Core::ComputingStatus::NoAvailableInfusionTime, {"NoAvailableInfusionTime", "32"}},
            {Tucuxi::Core::ComputingStatus::NoFormulationAndRouteForAdjustment,
             {"NoFormulationAndRouteForAdjustment", "33"}},
            {Tucuxi::Core::ComputingStatus::ConcentrationSizeError, {"ConcentrationSizeError", "34"}},
            {Tucuxi::Core::ComputingStatus::ActiveMoietyCalculationError, {"ActiveMoietyCalculationError", "35"}},
            {Tucuxi::Core::ComputingStatus::NoAnalytesGroup, {"NoAnalytesGroup", "36"}},
            {Tucuxi::Core::ComputingStatus::IncompatibleTreatmentModel, {"IncompatibleTreatmentModel", "37"}},
            {Tucuxi::Core::ComputingStatus::ComputingComponentNotInitialized,
             {"ComputingComponentNotInitialized", "38"}},
            {Tucuxi::Core::ComputingStatus::UncompatibleDrugDomain, {"UncompatibleDrugDomain", "39"}},
            {Tucuxi::Core::ComputingStatus::NoSteadyState, {"NoSteadyState", "40"}},
            {Tucuxi::Core::ComputingStatus::AposterioriPercentilesOutOfScopeSamplesError,
             {"AposterioriPercentilesOutOfScopeSamplesError", "41"}},
            {Tucuxi::Core::ComputingStatus::AdjustmentsInternalError, {"AdjustmentsInternalError", "42"}},
            {Tucuxi::Core::ComputingStatus::MultiComputingComponentExceptionError,
             {"MultiComputingComponentExceptionError", "43"}},
            {Tucuxi::Core::ComputingStatus::MultiComputingComponentNotInitialized,
             {"MultiComputingComponentNotInitialized", "44"}},
            {Tucuxi::Core::ComputingStatus::MultiActiveMoietyCalculationError,
             {"MultiActiveMoietyCalculationError", "45"}},
            {Tucuxi::Core::ComputingStatus::PercentilesNoValidPrediction, {"PercentilesNoValidPrediction", "46"}},
            {Tucuxi::Core::ComputingStatus::AposterioriPercentilesNoLikelySample,
             {"AposterioriPercentilesNoLikelySample", "47"}},
            {Tucuxi::Core::ComputingStatus::NoDosageHistory, {"NoDosageHistory", "48"}},
            {Tucuxi::Core::ComputingStatus::SampleBeforeTreatmentStart, {"SampleBeforeTreatmentStart", "49"}}};

    auto it = m.find(_computingStatus);
    if (it != m.end()) {
        std::pair<std::string, std::string> m2 = it->second;
        return m2.first;
    }

    return "Totally undefined internal error, with no error code";
}

} // namespace Core
} // namespace Tucuxi
