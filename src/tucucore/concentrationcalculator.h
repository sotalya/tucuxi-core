/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CONCENTRATIONCALCULATOR_H
#define TUCUXI_CORE_CONCENTRATIONCALCULATOR_H

#include "tucucore/definitions.h"
#include "tucucore/parameter.h"
#include "tucucore/dosage.h"
#include "tucucore/residualerrormodel.h"

// For ConcentrationPredictionPtr. This class should be declared somewhere else
#include "tucucore/iprocessingservices.h"

namespace Tucuxi {
namespace Core {



class ConcentrationCalculator
{
public:


    enum class ComputationResult { Success, Failure, Aborted };

    static ComputationResult computeConcentrations(
        ConcentrationPredictionPtr &_prediction,
        int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas = Etas(0),
        const ResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
        const Deviation& _eps = 0,
        const bool _isFixedDensity = 0);

};

}
}

#endif // TUCUXI_CORE_CONCENTRATIONCALCULATOR_H
