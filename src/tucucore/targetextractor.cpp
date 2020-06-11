/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/targetextractor.h"

namespace Tucuxi {
namespace Core {

TargetEvent TargetExtractor::targetEventFromTarget(const Target *_target) {

    switch (_target->m_targetType) {

    case TargetType::Peak :
    case TargetType::Residual :
    case TargetType::Mean :
    {
        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit("ug/l"),
                    _target->m_unit,
                    translateToUnit(_target->m_valueMin, _target->m_unit, Unit("ug/l")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug/l")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug/l")),
                    0.0, // we do not need the MIC
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;
    case TargetType::Auc :
    case TargetType::Auc24 :
    case TargetType::CumulativeAuc :
    case TargetType::AucOverMic :
    case TargetType::Auc24OverMic :
    {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit("ug*h/l"),
                    _target->m_unit,
                    translateToUnit(_target->m_valueMin,_target->m_unit, Unit("ug*h/l")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug*h/l")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug*h/l")),
                    translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l")),
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;
    case TargetType::AucDividedByMic :
    case TargetType::Auc24DividedByMic :
    case TargetType::TimeOverMic :
    {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit("h"),
                    _target->m_unit,
                    translateToUnit(_target->m_valueMin, _target->m_unit, Unit("h")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("h")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("h")),
                    translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l")),
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;

    case TargetType::PeakDividedByMic :
    {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit(""),
                    _target->m_unit,
                    translateToUnit(_target->m_valueMin, _target->m_unit, Unit("")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("")),
                    translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l")),
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;

    case TargetType::UnknownTarget :
    default:
    {
        Tucuxi::Common::LoggerHelper logger;
        logger.error("A target of an unkown type was given to the TargetExtractor");

        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit(""),
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



TargetEvent TargetExtractor::targetEventFromTarget(const Target *_target, const TargetDefinition *_targetDefinition) {

    switch (_targetDefinition->m_targetType) {

    case TargetType::Peak :
    case TargetType::Residual :
    case TargetType::Mean :
    {
        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit("ug/l"),
                    _targetDefinition->getUnit(),
                    translateToUnit(_target->m_valueMin, _target->m_unit, Unit("ug/l")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug/l")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug/l")),
                    0.0, // we do not need the MIC
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;
    case TargetType::Auc :
    case TargetType::Auc24 :
    case TargetType::CumulativeAuc :
    case TargetType::AucOverMic :
    case TargetType::Auc24OverMic :
    {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit("ug*h/l"),
                    _targetDefinition->getUnit(),
                    translateToUnit(_target->m_valueMin,_target->m_unit, Unit("ug*h/l")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug*h/l")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug*h/l")),
                    translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l")),
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;
    case TargetType::AucDividedByMic :
    case TargetType::Auc24DividedByMic :
    case TargetType::TimeOverMic :
    {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit("h"),
                    _targetDefinition->getUnit(),
                    translateToUnit(_target->m_valueMin, _target->m_unit, Unit("h")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("h")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("h")),
                    translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l")),
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;

    case TargetType::PeakDividedByMic :
    {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit(""),
                    _targetDefinition->getUnit(),
                    translateToUnit(_target->m_valueMin, _target->m_unit, Unit("")),
                    translateToUnit(_target->m_valueBest, _target->m_unit, Unit("")),
                    translateToUnit(_target->m_valueMax, _target->m_unit, Unit("")),
                    translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l")),
                    Unit("ug/l"),
                    _target->m_tMin,
                    _target->m_tBest,
                    _target->m_tMax);
    } break;

    case TargetType::UnknownTarget :
    default:
    {
        Tucuxi::Common::LoggerHelper logger;
        logger.error("A target of an unkown type was given to the TargetExtractor");

        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->m_targetType,
                    Unit(""),
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


TargetEvent TargetExtractor::targetEventFromTargetDefinition(const TargetDefinition *_target) {


    switch (_target->m_targetType) {

    case TargetType::Peak :
    case TargetType::Residual :
    case TargetType::Mean :

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit("ug/l"),
                    _target->getUnit(),
                    translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug/l")),
                    translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug/l")),
                    translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug/l")),
                    // Do not use Mic, so no unit translation
                    _target->getMic().getValue(),
                    Unit("ug/l"),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));

    case TargetType::Auc :
    case TargetType::Auc24 :
    case TargetType::CumulativeAuc :

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit("ug*h/l"),
                    _target->getUnit(),
                    translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    0.0,
                    Unit("ug/l"),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));


    case TargetType::AucOverMic :
    case TargetType::Auc24OverMic :

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit("ug*h/l"),
                    _target->getUnit(),
                    translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    translateToUnit(_target->getMic().getValue(), _target->getMicUnit(), Unit("ug/l")),
                    Unit("ug/l"),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));


    case TargetType::AucDividedByMic :
    case TargetType::Auc24DividedByMic :
    case TargetType::TimeOverMic :

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit("h"),
                    _target->getUnit(),
                    translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("h")),
                    translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("h")),
                    translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("h")),
                    translateToUnit(_target->getMic().getValue(), _target->getMicUnit(), Unit("ug/l")),
                    Unit("ug/l"),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));

    case TargetType::PeakDividedByMic :

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit(""),
                    _target->getUnit(),
                    translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("")),
                    translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("")),
                    translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("")),
                    translateToUnit(_target->getMic().getValue(), _target->getMicUnit(), Unit("ug/l")),
                    Unit("ug/l"),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));

    case TargetType::UnknownTarget :
    default:

        Tucuxi::Common::LoggerHelper logger;
        logger.error("A target of an unkown type was given to the TargetExtractor");

        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit(""),
                    _target->getUnit(),
                    _target->getCMin().getValue(),
                    _target->getCBest().getValue(),
                    _target->getCMax().getValue(),
                    // No use of MIC, so no unit translation
                    _target->getMic().getValue(),
                    Unit("ug/l"),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));
    }
}

ComputingStatus TargetExtractor::extract(
        ActiveMoietyId _activeMoietyId,
        const CovariateSeries &_covariates,
        const TargetDefinitions& _targetDefinitions,
        const Targets &_targets,
        const DateTime &_start,
        const DateTime &_end,
        TargetExtractionOption _extractionOption,
        TargetSeries &_series)
{
    switch (_extractionOption) {
    case TargetExtractionOption::AprioriValues :
    {
        // Not yet implemented
        return ComputingStatus::TargetExtractionError;

    } break;


    case TargetExtractionOption::IndividualTargets :
    {
        for (const auto& target : _targets) {
            if (target->m_activeMoietyId == _activeMoietyId) {
                _series.push_back(targetEventFromTarget(target.get()));
            }
        }
    } break;

    case TargetExtractionOption::IndividualTargetsIfDefinitionExists :
    {
        for (const auto& target : _targets) {

            bool foundDefinition = false;
            // First find the corresponding target definition
            for (const auto& targetDefinition : _targetDefinitions) {
                if ((targetDefinition->getActiveMoietyId() == target.get()->m_activeMoietyId) &&
                        (targetDefinition.get()->getTargetType() == target.get()->m_targetType)){
                    // We create the TargetEvent with the target
                    _series.push_back(targetEventFromTarget(target.get(), targetDefinition.get()));
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

    case TargetExtractionOption::DefinitionIfNoIndividualTarget :
    {
        for (const auto& target : _targets) {
            if ((_activeMoietyId == target.get()->m_activeMoietyId)){
                bool foundTarget = false;

                for (const auto& targetDefinition : _targetDefinitions) {
                    if ((targetDefinition.get()->getActiveMoietyId() == target.get()->m_activeMoietyId) &&
                            (targetDefinition.get()->getTargetType() == target.get()->m_targetType)){
                        foundTarget = true;
                        // We create the TargetEvent with the target
                        _series.push_back(targetEventFromTarget(target.get(), targetDefinition.get()));
                    }
                }

                if (!foundTarget) {
                    // We create the TargetEvent with the target, without the target definition
                    _series.push_back(targetEventFromTarget(target.get()));
                }
            }
        }
        for (const auto& targetDefinition : _targetDefinitions) {
            bool foundTarget = false;
            for (const auto& target : _targets) {
                if ((targetDefinition.get()->getActiveMoietyId() == target.get()->m_activeMoietyId) &&
                        (targetDefinition.get()->getTargetType() == target.get()->m_targetType)){
                    foundTarget = true;
                }
            }
            if (!foundTarget) {
                // Then we create the TargetEvent with the definition

                _series.push_back(targetEventFromTargetDefinition(targetDefinition.get()));
            }
        }
    } break;

    case TargetExtractionOption::IndividualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget :
    {
        for (const auto& target : _targets) {
            if ((_activeMoietyId == target.get()->m_activeMoietyId)){
                for (const auto& targetDefinition : _targetDefinitions) {
                    if ((targetDefinition.get()->getActiveMoietyId() == target.get()->m_activeMoietyId) &&
                            (targetDefinition.get()->getTargetType() == target.get()->m_targetType)){
                        // We create the TargetEvent with the target
                        _series.push_back(targetEventFromTarget(target.get(), targetDefinition.get()));
                    }
                }
            }
        }
        for (const auto& targetDefinition : _targetDefinitions) {
            bool foundTarget = false;
            for (const auto& target : _targets) {
                if ((targetDefinition.get()->getActiveMoietyId() == target.get()->m_activeMoietyId) &&
                        (targetDefinition.get()->getTargetType() == target.get()->m_targetType)){
                    foundTarget = true;
                }
            }
            if (!foundTarget) {
                // Then we create the TargetEvent with the definition

                _series.push_back(targetEventFromTargetDefinition(targetDefinition.get()));
            }
        }
    } break;

    case TargetExtractionOption::PopulationValues :
    {
        for (const auto& targetDefinition : _targetDefinitions) {

            _series.push_back(targetEventFromTargetDefinition(targetDefinition.get()));
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
