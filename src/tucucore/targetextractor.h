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


#ifndef TUCUXI_CORE_TARGETEXTRACTOR_H
#define TUCUXI_CORE_TARGETEXTRACTOR_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/covariateevent.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/drugtreatment/target.h"
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
            const ActiveMoietyId& _activeMoietyId,
            const CovariateSeries& _covariates,
            const TargetDefinitions& _targetDefinitions,
            const Targets& _targets,
            const DateTime& _start,
            const DateTime& _end,
            const TucuUnit& _concentrationUnit,
            TargetExtractionOption _extractionOption,
            TargetSeries& _series);

protected:
    TargetEvent targetEventFromTarget(const Target* _target, const TucuUnit& _concentrationUnit);

    TargetEvent targetEventFromTarget(
            const Target* _target, const TargetDefinition* _targetDefinition, const TucuUnit& _concentrationUnit);

    TargetEvent targetEventFromTargetDefinition(const TargetDefinition* _target, const TucuUnit& _concentrationUnit);

    friend TestTargetExtractor;
    friend TestTargetEvaluator;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TARGETEXTRACTOR_H
