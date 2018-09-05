/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"

#include "tucucore/targetextractor.h"

namespace Tucuxi {
namespace Core {

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
            _series.push_back(
                        TargetEvent(
                            target.get()->m_activeMoietyId,
                            target.get()->m_targetType,
                            target.get()->m_valueMin,
                            target.get()->m_valueBest,
                            target.get()->m_valueMax,
                            target.get()->m_tMin,
                            target.get()->m_tBest,
                            target.get()->m_tMax));
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
                    _series.push_back(
                                TargetEvent(
                                    target.get()->m_activeMoietyId,
                                    target.get()->m_targetType,
                                    target.get()->m_valueMin,
                                    target.get()->m_valueBest,
                                    target.get()->m_valueMax,
                                    target.get()->m_tMin,
                                    target.get()->m_tBest,
                                    target.get()->m_tMax));

                }
            }
            if (!foundTarget) {
                // Then we create the TargetEvent with the definition

                _series.push_back(
                            // Here we consider times as minutes. This has to be fixed once
                            TargetEvent(
                                targetDefinition.get()->getActiveMoietyId(),
                                targetDefinition.get()->getTargetType(),
                                targetDefinition.get()->getCMin().getValue(),
                                targetDefinition.get()->getCBest().getValue(),
                                targetDefinition.get()->getCMax().getValue(),
                                Tucuxi::Common::Duration(
                                    std::chrono::minutes(static_cast<int>(targetDefinition.get()->getTMin().getValue()))),
                                Tucuxi::Common::Duration(
                                    std::chrono::minutes(static_cast<int>(targetDefinition.get()->getTBest().getValue()))),
                                Tucuxi::Common::Duration(
                                    std::chrono::minutes(static_cast<int>(targetDefinition.get()->getTMax().getValue())))));
            }

    } break;

    case TargetExtractionOption::PopulationValues :
    {
        for (const auto& targetDefinition : _targetDefinitions) {

            _series.push_back(
                        // Here we consider times as minutes. This has to be fixed once
                        TargetEvent(
                            targetDefinition.get()->getActiveMoietyId(),
                            targetDefinition.get()->getTargetType(),
                            targetDefinition.get()->getCMin().getValue(),
                            targetDefinition.get()->getCBest().getValue(),
                            targetDefinition.get()->getCMax().getValue(),
                            Tucuxi::Common::Duration(
                                std::chrono::minutes(static_cast<int>(targetDefinition.get()->getTMin().getValue()))),
                            Tucuxi::Common::Duration(
                                std::chrono::minutes(static_cast<int>(targetDefinition.get()->getTBest().getValue()))),
                            Tucuxi::Common::Duration(
                                std::chrono::minutes(static_cast<int>(targetDefinition.get()->getTMax().getValue())))));
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
