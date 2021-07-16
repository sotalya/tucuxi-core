/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_TARGETEXTRACTOR_H
#define TUCUXI_CORE_TARGETEXTRACTOR_H

#include "tucucore/drugtreatment/target.h"
#include "tucucore/targetevent.h"
#include "tucucore/covariateevent.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/targetevent.h"

struct TestTargetExtractor;
struct TestTargetEvaluator;

namespace Tucuxi {
namespace Core {

class TargetExtractor
{
public:

    ///
    /// \brief extract
    /// \param _covariates
    /// \param _targetDefinitions
    /// \param _targets
    /// \param _start
    /// \param _end
    /// \param _concentrationUnit
    /// \param _extractionOption
    /// \param _series
    /// \return
    /// TODO : The policy still has to be managed correctly:
    /// - Only population values
    /// - A priori values
    /// - Priority to targets over targetDefinitions
    /// - Only individual targets
    ComputingStatus extract(
            ActiveMoietyId _activeMoietyId,
            const CovariateSeries &_covariates,
            const TargetDefinitions& _targetDefinitions,
            const Targets &_targets,
            const DateTime &_start,
            const DateTime &_end,
            const TucuUnit &_concentrationUnit,
            TargetExtractionOption _extractionOption,
            TargetSeries &_series);

protected:

    TargetEvent targetEventFromTarget(const Target *_target, const TucuUnit &_concentrationUnit);

    TargetEvent targetEventFromTarget(const Target *_target, const TargetDefinition *_targetDefinition, const TucuUnit &_concentrationUnit);

    TargetEvent targetEventFromTargetDefinition(const TargetDefinition *_target, const TucuUnit &_concentrationUnit);

    friend TestTargetExtractor;
    friend TestTargetEvaluator;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGETEXTRACTOR_H
