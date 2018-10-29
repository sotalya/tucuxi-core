/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"

#include "tucucore/targetextractor.h"

namespace Tucuxi {
namespace Core {

TargetEvent TargetExtractor::targetEventFromTarget(const Target *_target) {
    return  TargetEvent(
                _target->m_activeMoietyId,
                _target->m_targetType,
                _target->m_valueMin,
                _target->m_valueBest,
                _target->m_valueMax,
                _target->m_tMin,
                _target->m_tBest,
                _target->m_tMax);
}


TargetEvent TargetExtractor::targetEventFromTargetDefinition(const TargetDefinition *_target) {
    if ((_target->m_targetType == TargetType::Peak) ||
            (_target->m_targetType == TargetType::Residual)) {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),

                    translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug/l")),
                    translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug/l")),
                    translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug/l")),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));
    }
    else if ((_target->m_targetType == TargetType::Auc) ||
             (_target->m_targetType == TargetType::CumulativeAuc)) {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),

                    translateToUnit(_target->getCMin().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    translateToUnit(_target->getCBest().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    translateToUnit(_target->getCMax().getValue(), _target->getUnit(), Unit("ug*h/l")),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMin().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTBest().getValue()))),
                    Tucuxi::Common::Duration(
                        std::chrono::minutes(static_cast<int>(_target->getTMax().getValue()))));
    }
    else {

        // Here we consider times as minutes. This has to be fixed once
        return TargetEvent(
                    _target->getActiveMoietyId(),
                    _target->getTargetType(),
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

TargetExtractor::Result TargetExtractor::extract(
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
        return Result::ExtractionError;

    }break;


    case TargetExtractionOption::IndividualTargets :
    {
        for (const auto& target : _targets) {
            // We create the TargetEvent with the target
            _series.push_back(targetEventFromTarget(target.get()));
        }
    } break;


    case TargetExtractionOption::DefinitionIfNoIndividualTarget :
    {
        for (const auto& targetDefinition : _targetDefinitions) {
            bool foundTarget = false;
            for (const auto& target : _targets) {
                if ((targetDefinition.get()->getActiveMoietyId() == target.get()->m_activeMoietyId) &&
                        (targetDefinition.get()->getTargetType() == target.get()->m_targetType)){
                    foundTarget = true;
                    // We create the TargetEvent with the target
                    _series.push_back(targetEventFromTarget(target.get()));

                }
            }
            if (!foundTarget) {
                // Then we create the TargetEvent with the definition

                _series.push_back(targetEventFromTargetDefinition(targetDefinition.get()));
            }

    } break;

    case TargetExtractionOption::PopulationValues :
    {
        for (const auto& targetDefinition : _targetDefinitions) {

            _series.push_back(targetEventFromTargetDefinition(targetDefinition.get()));
        }

    } break;
}
}
TMP_UNUSED_PARAMETER(_covariates);
TMP_UNUSED_PARAMETER(_start);
TMP_UNUSED_PARAMETER(_end);

return Result::Ok;
}

} // namespace Core
} // namespace Tucuxi
