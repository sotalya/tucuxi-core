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

#include <memory>

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/sampleevent.h"


namespace Tucuxi {
namespace Core {

class OccasionIovContext;

class APosterioriEtasCalculator
{
public:
    APosterioriEtasCalculator();

    ///
    /// \brief Compute the a-posteriori etas from the samples.
    /// \param[in] _intakes : Intake series.
    /// \param[in] _parameters : Parameter set series.
    /// \param[in] _omega : Variance-covariance matrix sizing the eta vector.
    ///            With IOV it is the BSV block extended by one diagonal block
    ///            per occasion.
    /// \param[in] _residualErrorModel : Residual error model.
    /// \param[in] _samples : Samples to fit.
    /// \param[out] _aPosterioriEtas : Fitted etas (BSV then per-occasion IOV
    ///             blocks when a context is given).
    /// \param[out] _negativeLogLikelihood : Resulting negative log likelihood.
    /// \param[in] _iovContext : Optional IOV occasion context. When non-null
    ///            it is attached to the parameter series so that etas are
    ///            applied during the fit. Null by default (no IOV).
    ///
    /// \return Ok on success, an error status otherwise.
    ComputingStatus computeAposterioriEtas(
            const IntakeSeries& _intakes,
            const ParameterSetSeries& _parameters,
            const OmegaMatrix& _omega,
            const IResidualErrorModel& _residualErrorModel,
            const SampleSeries& _samples,
            Etas& _aPosterioriEtas,
            Value& _negativeLogLikelihood,
            std::shared_ptr<OccasionIovContext const> _iovContext = nullptr);
};

} // namespace Core
} // namespace Tucuxi

#endif // APOSTERIORIETASCALCULATOR_H
