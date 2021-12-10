/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef APOSTERIORIETASCALCULATOR_H
#define APOSTERIORIETASCALCULATOR_H

#include "computingservice/computingresult.h"
#include "definitions.h"
#include "dosage.h"
#include "parameter.h"
#include "residualerrormodel.h"
#include "sampleevent.h"

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
            Etas& _aPosterioriEtas);
};

} // namespace Core
} // namespace Tucuxi

#endif // APOSTERIORIETASCALCULATOR_H
