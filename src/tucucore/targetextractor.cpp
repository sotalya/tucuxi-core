/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/targetextractor.h"


using Tucuxi::Common::UnitManager;

namespace Tucuxi {
namespace Core {

TargetEvent TargetExtractor::targetEventFromTarget(const Target *_target) {

    switch (_target->m_targetType) {

    case TargetType::Mean :
    case TargetType::Residual :
    case TargetType::Peak :
    {
        //TODO : VERIFY VALUE

        return TargetEvent::createTargetEventWithTime(_target->getActiveMoietyId(),
                                       _target->m_targetType,
                                       Unit("ug/l"),
                                       _target->m_unit,
                                       UnitManager::translateToUnit(_target->m_valueMin, _target->m_unit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                       UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                       UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug/l"), UnitManager::UnitType::Concentration),
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
        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
                                       _target->m_targetType,
                                       Unit("ug*h/l"),
                                       _target->m_unit,
                                       UnitManager::translateToUnit(_target->m_valueMin, _target->m_unit, Unit("ug*h/l"), UnitManager::UnitType::ConcentrationTime),
                                       UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug*h/l"), UnitManager::UnitType::ConcentrationTime),
                                       UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug*h/l"), UnitManager::UnitType::ConcentrationTime),
                                       UnitManager::translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug*h/l"), UnitManager::UnitType::ConcentrationTime),
                                       _target->m_micUnit,
                                       _target->m_tMin,
                                       _target->m_tBest,
                                       _target->m_tMax);

    } break;

    case TargetType::PeakDividedByMic :

    {
        //TODO : VERIFY VALUE

        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
                                       _target->m_targetType,
                                       Unit(""),
                                       _target->m_unit,
                                       UnitManager::translateToUnit(_target->m_valueMin, _target->m_unit, Unit(""), UnitManager::UnitType::NoUnit),
                                       UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit(""), UnitManager::UnitType::NoUnit),
                                       UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit(""), UnitManager::UnitType::NoUnit),
                                       UnitManager::translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                       _target->m_micUnit,
                                       _target->m_tMin,
                                       _target->m_tBest,
                                       _target->m_tMax);
    } break;

    case TargetType::TimeOverMic :
    case TargetType::AucDividedByMic :
    case TargetType::Auc24DividedByMic :
    {

        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
                                       _target->m_targetType,
                                       Unit("h"),
                                       _target->m_unit,
                                       UnitManager::translateToUnit(_target->m_valueMin, _target->m_unit, Unit("h"), UnitManager::UnitType::Time),
                                       UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit("h"), UnitManager::UnitType::Time),
                                       UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit("h"), UnitManager::UnitType::Time),
                                       UnitManager::translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                       _target->m_micUnit,
                                       _target->m_tMin,
                                       _target->m_tBest,
                                       _target->m_tMax);

    } break;

    case TargetType::UnknownTarget :
    default:
    {
        Tucuxi::Common::LoggerHelper logger;
        logger.error("A target of an unkown type was given to the TargetExtractor");

        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
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


        return TargetEvent::createTargetEventWithTime(_target->getActiveMoietyId(),
                                              _target->m_targetType,
                                              Unit("ug/l"),
                                              _targetDefinition->getUnit(),
                                              UnitManager::translateToUnit(_target->m_valueMin, _target->m_unit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                              UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                              UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug/l"), UnitManager::UnitType::Concentration),
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



        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
                                              _target->m_targetType,
                                              Unit("ug*h/l"),
                                              _targetDefinition->getUnit(),
                                              UnitManager::translateToUnit(_target->m_valueMin,_target->m_unit, Unit("ug*h/l"), UnitManager::UnitType::ConcentrationTime),
                                              UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit("ug*h/l"), UnitManager::UnitType::ConcentrationTime),
                                              UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit("ug*h/l"), UnitManager::UnitType::ConcentrationTime),
                                              UnitManager::translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                              Unit("ug/l"),
                                              _target->m_tMin,
                                              _target->m_tBest,
                                              _target->m_tMax);

    } break;
    case TargetType::AucDividedByMic :
    case TargetType::Auc24DividedByMic :
    case TargetType::TimeOverMic :
    {



        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
                                              _target->m_targetType,
                                              Unit("h"),
                                              _targetDefinition->getUnit(),
                                              UnitManager::translateToUnit(_target->m_valueMin, _target->m_unit, Unit("h"), UnitManager::UnitType::Time),
                                              UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit("h"), UnitManager::UnitType::Time),
                                              UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit("h"), UnitManager::UnitType::Time),
                                              UnitManager::translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l"), UnitManager::UnitType::Concentration),
                                              Unit("ug/l"),
                                              _target->m_tMin,
                                              _target->m_tBest,
                                              _target->m_tMax);

    } break;

    case TargetType::PeakDividedByMic :
    {


        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
                                              _target->m_targetType,
                                              Unit(""),
                                              _targetDefinition->getUnit(),
                                              UnitManager::translateToUnit(_target->m_valueMin, _target->m_unit, Unit(""), UnitManager::UnitType::NoUnit),
                                              UnitManager::translateToUnit(_target->m_valueBest, _target->m_unit, Unit(""), UnitManager::UnitType::NoUnit),
                                              UnitManager::translateToUnit(_target->m_valueMax, _target->m_unit, Unit(""), UnitManager::UnitType::NoUnit),
                                              UnitManager::translateToUnit(_target->m_mic, _target->m_micUnit, Unit("ug/l"), UnitManager::UnitType::Concentration),
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

        return TargetEvent::createTargetEventWithMicAndTime(_target->getActiveMoietyId(),
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

        //TODO : VERIFY VALUE

        return TargetEvent::createTargetEventWithoutTimeAndMic(_target->getActiveMoietyId(),
                                       _target->getTargetType(),
                                       Unit("ug/l"),
                                       _target->getUnit(),
                                       UnitManager::translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug/l"), UnitManager::UnitType::Concentration),
                                       UnitManager::translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug/l"),  UnitManager::UnitType::Concentration),
                                       UnitManager::translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug/l"),  UnitManager::UnitType::Concentration));

    case TargetType::Mean :



        return TargetEvent::createTargetEventWithTime(_target->getActiveMoietyId(),
                                              _target->getTargetType(),
                                              Unit("ug/l"),
                                              _target->getUnit(),
                                              UnitManager::translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug/l"),  UnitManager::UnitType::Concentration),
                                              UnitManager::translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug/l"),  UnitManager::UnitType::Concentration),
                                              UnitManager::translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug/l"),  UnitManager::UnitType::Concentration),
                                              Tucuxi::Common::Duration(
                                                  std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                                              Tucuxi::Common::Duration(
                                                  std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                                              Tucuxi::Common::Duration(
                                                  std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));

    case TargetType::Auc :
    case TargetType::Auc24 :
    case TargetType::CumulativeAuc :



        return TargetEvent::createTargetEventWithTime(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit("ug*h/l"),
                    _target->getUnit(),
                    UnitManager::translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug*h/l"),  UnitManager::UnitType::ConcentrationTime),
                    UnitManager::translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug*h/l"),  UnitManager::UnitType::ConcentrationTime),
                    UnitManager::translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug*h/l"),  UnitManager::UnitType::ConcentrationTime),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));



    case TargetType::AucOverMic :
    case TargetType::Auc24OverMic :



        return TargetEvent::createTargetEventWithMicAndTime(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit("ug*h/l"),
                    _target->getUnit(),
                    UnitManager::translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug*h/l"),  UnitManager::UnitType::ConcentrationTime),
                    UnitManager::translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug*h/l"),  UnitManager::UnitType::ConcentrationTime),
                    UnitManager::translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug*h/l"),  UnitManager::UnitType::ConcentrationTime),
                    UnitManager::translateToUnit(_target->getMic().getValue(), _target->getMicUnit(), Unit("ug/l"),  UnitManager::UnitType::ConcentrationTime),
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


        return TargetEvent::createTargetEventWithMicAndTime(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit("h"),
                    _target->getUnit(),
                    UnitManager::translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("h"),  UnitManager::UnitType::Time),
                    UnitManager::translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("h"), UnitManager::UnitType::Time),
                    UnitManager::translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("h"), UnitManager::UnitType::Time),
                    UnitManager::translateToUnit(_target->getMic().getValue(), _target->getMicUnit(), Unit("ug/l"), UnitManager::UnitType::Concentration),
                    Unit("ug/l"),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));


    case TargetType::PeakDividedByMic :


        return TargetEvent::createTargetEventWithMicAndTime(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit(""),
                    _target->getUnit(),
                    UnitManager::translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit(""), UnitManager::UnitType::NoUnit),
                    UnitManager::translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit(""), UnitManager::UnitType::NoUnit),
                    UnitManager::translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit(""), UnitManager::UnitType::NoUnit),
                    UnitManager::translateToUnit(_target->getMic().getValue(), _target->getMicUnit(), Unit("ug/l"), UnitManager::UnitType::Concentration),
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

        return TargetEvent::createTargetEventWithTime(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
                    Unit(""),
                    _target->getUnit(),
                    _target->getCMin().getValue(),
                    _target->getCBest().getValue(),
                    _target->getCMax().getValue(),
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
