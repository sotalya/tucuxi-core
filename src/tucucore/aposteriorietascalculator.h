//@@license@@

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
