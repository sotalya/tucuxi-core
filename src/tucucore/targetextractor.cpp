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


#include "tucucore/targetextractor.h"

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"


using Tucuxi::Common::UnitManager;

namespace Tucuxi {
namespace Core {

TucuUnit getConcentrationTimeUnit(const TucuUnit& _concentrationUnit)
{
    static const std::map<std::string, std::string> sm_conversionMap = // NOLINT(readability-identifier-naming)
            {{"ug/l", "ug*h/l"},
             {"mg/l", "mg*h/l"},
             {"g/l", "h*g/l"},
             {"g/ml", "g*h/ml"},
             {"mg/ml", "mg*h/ml"},
             {"ug/ml", "ug*h/ml"}};

    std::string key = _concentrationUnit.toString();
    if (sm_conversionMap.count(key) > 0) {
        return TucuUnit(sm_conversionMap.at(key));
    }
    throw std::invalid_argument("Error in concentration unit conversion");
}

TargetEvent TargetExtractor::targetEventFromTarget(const Target* _target, const TucuUnit& _concentrationUnit)
{

    switch (_target->m_targetType) {

    case TargetType::Mean:
    case TargetType::Residual:
    case TargetType::Peak: {
        //TODO : VERIFY VALUE

        return TargetEvent::createTargetEventWithTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                _concentrationUnit,
                _target->m_unit,
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_valueMin, _target->m_unit, _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_valueBest, _target->m_unit, _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_valueMax, _target->m_unit, _concentrationUnit),
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);
    } break;

    case TargetType::Auc:
    case TargetType::Auc24:
    case TargetType::CumulativeAuc:
    case TargetType::AucOverMic:
    case TargetType::Auc24OverMic: {
        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                getConcentrationTimeUnit(_concentrationUnit),
                _target->m_unit,
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->m_valueMin, _target->m_unit, getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->m_valueBest, _target->m_unit, getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->m_valueMax, _target->m_unit, getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_mic, _target->m_micUnit, _concentrationUnit),
                _target->m_micUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);

    } break;

    case TargetType::PeakDividedByMic:
    case TargetType::ResidualDividedByMic:
    case TargetType::FractionTimeOverMic:

    {
        //TODO : VERIFY VALUE

        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                TucuUnit(""),
                _target->m_unit,
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->m_valueMin, _target->m_unit, TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->m_valueBest, _target->m_unit, TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->m_valueMax, _target->m_unit, TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_mic, _target->m_micUnit, _concentrationUnit),
                _target->m_micUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);
    } break;

    case TargetType::TimeOverMic:
    case TargetType::AucDividedByMic:
    case TargetType::Auc24DividedByMic: {

        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                TucuUnit("h"),
                _target->m_unit,
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->m_valueMin, _target->m_unit, TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->m_valueBest, _target->m_unit, TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->m_valueMax, _target->m_unit, TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_mic, _target->m_micUnit, _concentrationUnit),
                _target->m_micUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);

    } break;

    case TargetType::UnknownTarget:
    default: {
        Tucuxi::Common::LoggerHelper logger;
        logger.error("A target of an unkown type was given to the TargetExtractor");

        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                TucuUnit(""),
                _target->m_unit,
                _target->m_valueMin,
                _target->m_valueBest,
                _target->m_valueMax,
                _target->m_mic,
                _target->m_micUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);

    } break;
    }
}



TargetEvent TargetExtractor::targetEventFromTarget(
        const Target* _target, const TargetDefinition* _targetDefinition, const TucuUnit& _concentrationUnit)
{

    switch (_targetDefinition->m_targetType) {

    case TargetType::Peak:
    case TargetType::Residual:
    case TargetType::Mean: {


        return TargetEvent::createTargetEventWithTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                _concentrationUnit,
                _targetDefinition->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_valueMin, _target->m_unit, _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_valueBest, _target->m_unit, _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_valueMax, _target->m_unit, _concentrationUnit),
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);

    } break;
    case TargetType::Auc:
    case TargetType::Auc24:
    case TargetType::CumulativeAuc:
    case TargetType::AucOverMic:
    case TargetType::Auc24OverMic: {



        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                getConcentrationTimeUnit(_concentrationUnit),
                _targetDefinition->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->m_valueMin, _target->m_unit, getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->m_valueBest, _target->m_unit, getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->m_valueMax, _target->m_unit, getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_mic, _target->m_micUnit, _concentrationUnit),
                _concentrationUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);

    } break;
    case TargetType::AucDividedByMic:
    case TargetType::Auc24DividedByMic:
    case TargetType::TimeOverMic: {



        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                TucuUnit("h"),
                _targetDefinition->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->m_valueMin, _target->m_unit, TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->m_valueBest, _target->m_unit, TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->m_valueMax, _target->m_unit, TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_mic, _target->m_micUnit, _concentrationUnit),
                _concentrationUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);

    } break;

    case TargetType::PeakDividedByMic:
    case TargetType::ResidualDividedByMic:
    case TargetType::FractionTimeOverMic: {


        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                TucuUnit(""),
                _targetDefinition->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->m_valueMin, _target->m_unit, TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->m_valueBest, _target->m_unit, TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->m_valueMax, _target->m_unit, TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->m_mic, _target->m_micUnit, _concentrationUnit),
                _concentrationUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);
    } break;

    case TargetType::UnknownTarget:
    default: {
        Tucuxi::Common::LoggerHelper logger;
        logger.error("A target of an unkown type was given to the TargetExtractor");

        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->m_targetType,
                TucuUnit(""),
                _targetDefinition->getUnit(),
                _target->m_valueMin,
                _target->m_valueBest,
                _target->m_valueMax,
                _target->m_mic,
                _target->m_micUnit,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);
    } break;
    }
}


TargetEvent TargetExtractor::targetEventFromTargetDefinition(
        const TargetDefinition* _target, const TucuUnit& _concentrationUnit)
{


    switch (_target->m_targetType) {

    case TargetType::Residual:
    case TargetType::Mean:

        //TODO : VERIFY VALUE

        return TargetEvent::createTargetEventWithoutTimeAndMic(
                _target->getActiveMoietyId(),
                _target->getTargetType(),
                _concentrationUnit,
                _target->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getCMin().getValue(), _target->getUnit(), _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getCBest().getValue(), _target->getUnit(), _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getCMax().getValue(), _target->getUnit(), _concentrationUnit));

    case TargetType::Peak:

        //TODO : VERIFY VALUE

        return TargetEvent::createTargetEventWithTime(
                _target->getActiveMoietyId(),
                _target->getTargetType(),
                _concentrationUnit,
                _target->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getCMin().getValue(), _target->getUnit(), _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getCBest().getValue(), _target->getUnit(), _concentrationUnit),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getCMax().getValue(), _target->getUnit(), _concentrationUnit),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));

    case TargetType::Auc:
    case TargetType::Auc24:
    case TargetType::CumulativeAuc:



        return TargetEvent::createTargetEventWithTime(
                _target->getActiveMoietyId(),
                _target->getTargetType(),
                getConcentrationTimeUnit(_concentrationUnit),
                _target->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->getCMin().getValue(),
                        _target->getUnit(),
                        getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->getCBest().getValue(),
                        _target->getUnit(),
                        getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->getCMax().getValue(),
                        _target->getUnit(),
                        getConcentrationTimeUnit(_concentrationUnit)),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));



    case TargetType::AucOverMic:
    case TargetType::Auc24OverMic:



        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->getTargetType(),
                getConcentrationTimeUnit(_concentrationUnit),
                _target->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->getCMin().getValue(),
                        _target->getUnit(),
                        getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->getCBest().getValue(),
                        _target->getUnit(),
                        getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->getCMax().getValue(),
                        _target->getUnit(),
                        getConcentrationTimeUnit(_concentrationUnit)),
                UnitManager::convertToUnit<UnitManager::UnitType::ConcentrationTime>(
                        _target->getMic().getValue(), _target->getMicUnit(), _concentrationUnit),
                _concentrationUnit,
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));



    case TargetType::AucDividedByMic:
    case TargetType::Auc24DividedByMic:
    case TargetType::TimeOverMic:


        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->getTargetType(),
                TucuUnit("h"),
                _target->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->getCMin().getValue(), _target->getUnit(), TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->getCBest().getValue(), _target->getUnit(), TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Time>(
                        _target->getCMax().getValue(), _target->getUnit(), TucuUnit("h")),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getMic().getValue(), _target->getMicUnit(), _concentrationUnit),
                _concentrationUnit,
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));


    case TargetType::PeakDividedByMic:
    case TargetType::ResidualDividedByMic:
    case TargetType::FractionTimeOverMic:


        return TargetEvent::createTargetEventWithMicAndTime(
                _target->getActiveMoietyId(),
                _target->getTargetType(),
                TucuUnit(""),
                _target->getUnit(),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->getCMin().getValue(), _target->getUnit(), TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->getCBest().getValue(), _target->getUnit(), TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::NoUnit>(
                        _target->getCMax().getValue(), _target->getUnit(), TucuUnit("")),
                UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                        _target->getMic().getValue(), _target->getMicUnit(), _concentrationUnit),
                _concentrationUnit,
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));

    case TargetType::UnknownTarget:
    default:

        Tucuxi::Common::LoggerHelper logger;
        logger.error("A target of an unkown type was given to the TargetExtractor");

        return TargetEvent::createTargetEventWithTime(
                _target->getActiveMoietyId(),
                _target->getTargetType(),
                TucuUnit(""),
                _target->getUnit(),
                _target->getCMin().getValue(),
                _target->getCBest().getValue(),
                _target->getCMax().getValue(),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                Tucuxi::Common::Duration(std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));
    }
}

ComputingStatus TargetExtractor::extract(
        const ActiveMoietyId& _activeMoietyId,
        const CovariateSeries& _covariates,
        const TargetDefinitions& _targetDefinitions,
        const Targets& _targets,
        const DateTime& _start,
        const DateTime& _end,
        const TucuUnit& _concentrationUnit,
        TargetExtractionOption _extractionOption,
        TargetSeries& _series)
{
    switch (_extractionOption) {
    case TargetExtractionOption::AprioriValues: {
        // Not yet implemented
        return ComputingStatus::TargetExtractionError;

    } break;


    case TargetExtractionOption::IndividualTargets: {
        for (const auto& target : _targets) {
            if (target->m_activeMoietyId == _activeMoietyId) {
                _series.push_back(targetEventFromTarget(target.get(), _concentrationUnit));
            }
        }
    } break;

    case TargetExtractionOption::IndividualTargetsIfDefinitionExists: {
        for (const auto& target : _targets) {

            bool foundDefinition = false;
            // First find the corresponding target definition
            for (const auto& targetDefinition : _targetDefinitions) {
                if ((targetDefinition->getActiveMoietyId() == target->m_activeMoietyId)
                    && (targetDefinition->getTargetType() == target->m_targetType)) {
                    // We create the TargetEvent with the target
                    _series.push_back(targetEventFromTarget(target.get(), targetDefinition.get(), _concentrationUnit));
                    foundDefinition = true;
                    break;
                }
            }
            if (!foundDefinition) {
                // TODO : Log, something went wrong
                return ComputingStatus::TargetExtractionError;
            }
        }
    } break;

    case TargetExtractionOption::DefinitionIfNoIndividualTarget: {
        for (const auto& target : _targets) {
            if ((_activeMoietyId == target->m_activeMoietyId)) {
                bool foundTarget = false;

                for (const auto& targetDefinition : _targetDefinitions) {
                    if ((targetDefinition->getActiveMoietyId() == target->m_activeMoietyId)
                        && (targetDefinition->getTargetType() == target->m_targetType)) {
                        foundTarget = true;
                        // We create the TargetEvent with the target
                        _series.push_back(
                                targetEventFromTarget(target.get(), targetDefinition.get(), _concentrationUnit));
                    }
                }

                if (!foundTarget) {
                    // We create the TargetEvent with the target, without the target definition
                    _series.push_back(targetEventFromTarget(target.get(), _concentrationUnit));
                }
            }
        }
        for (const auto& targetDefinition : _targetDefinitions) {
            bool foundTarget = false;
            for (const auto& target : _targets) {
                if ((targetDefinition->getActiveMoietyId() == target->m_activeMoietyId)
                    && (targetDefinition->getTargetType() == target->m_targetType)) {
                    foundTarget = true;
                }
            }
            if (!foundTarget) {
                // Then we create the TargetEvent with the definition

                _series.push_back(targetEventFromTargetDefinition(targetDefinition.get(), _concentrationUnit));
            }
        }
    } break;

    case TargetExtractionOption::IndividualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget: {
        for (const auto& target : _targets) {
            if ((_activeMoietyId == target->m_activeMoietyId)) {
                for (const auto& targetDefinition : _targetDefinitions) {
                    if ((targetDefinition->getActiveMoietyId() == target->m_activeMoietyId)
                        && (targetDefinition->getTargetType() == target->m_targetType)) {
                        // We create the TargetEvent with the target
                        _series.push_back(
                                targetEventFromTarget(target.get(), targetDefinition.get(), _concentrationUnit));
                    }
                }
            }
        }
        for (const auto& targetDefinition : _targetDefinitions) {
            bool foundTarget = false;
            for (const auto& target : _targets) {
                if ((targetDefinition->getActiveMoietyId() == target->m_activeMoietyId)
                    && (targetDefinition->getTargetType() == target->m_targetType)) {
                    foundTarget = true;
                }
            }
            if (!foundTarget) {
                // Then we create the TargetEvent with the definition

                _series.push_back(targetEventFromTargetDefinition(targetDefinition.get(), _concentrationUnit));
            }
        }
    } break;

    case TargetExtractionOption::PopulationValues: {
        for (const auto& targetDefinition : _targetDefinitions) {

            _series.push_back(targetEventFromTargetDefinition(targetDefinition.get(), _concentrationUnit));
        }

    } break;
    }
    TMP_UNUSED_PARAMETER(_covariates);
    TMP_UNUSED_PARAMETER(_start);
    TMP_UNUSED_PARAMETER(_end);

    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
