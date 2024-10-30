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


#ifndef APOSTERIORIETASCALCULATOR_H
#define APOSTERIORIETASCALCULATOR_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/sampleevent.h"


namespace Tucuxi {
namespace Core {


class APosterioriEtasCalculator
{
public:
    APosterioriEtasCalculator();

    ComputingStatus computeAposterioriEtas(
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel& _residualErrorModel,
            const SampleSeries& _samples,
            Etas& _aPosterioriEtas,
            Value& _negativeLogLikelihood);
};

} // namespace Core
} // namespace Tucuxi

#endif // APOSTERIORIETASCALCULATOR_H
