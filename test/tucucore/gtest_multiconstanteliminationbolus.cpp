//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"

#include "gtest_core.h"
#include "pkmodels/multiconstanteliminationbolus.h"

using namespace Tucuxi::Core;

TEST(Core_TestMultiConstantEliminationBolus, MultiConstantEliminationBolus)
{
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA0", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR0", 2.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS0", 0.03, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM0", 4.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA1", 10.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR1", 20.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS1", 0.003, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM1", 40.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    //I think that I need to add here an object of multiconstanteliminationbolus, and call the method calculateIntakePoints(), creating all the arguments to send to the method (navigating through the code)

    MultiConstantEliminationBolus aux;
    Tucuxi::Core::ComputingStatus res;
    MultiCompConcentrations concentrations;
    concentrations.push_back(Concentrations(CYCLE_SIZE, -1));
    concentrations.push_back(Concentrations(CYCLE_SIZE, -1));

    TimeOffsets times;

    // First calculate the size of residuals
    unsigned int residualSize = aux.getResidualSize();
    Residuals inResiduals(residualSize);
    Residuals outResiduals(residualSize);
    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    double dose = 200;
    Tucuxi::Common::Duration interval = 12h;
    Tucuxi::Common::Duration infusionTime = 0h;



    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(Tucuxi::Core::AbsorptionModel::Intravascular),
            Tucuxi::Core::AbsorptionModel::Intravascular,
            infusionTime,
            CYCLE_SIZE);

    bool isAll = false;
    res = aux.calculateIntakePoints(
            concentrations, times, intakeEvent, parameters, inResiduals, isAll, outResiduals, true);

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);

    // The intake calculator implements these functions:
    // C0(t) = max(0.0 , (D + residual * R0) * ( 1 - t * S0) * M0 + A0)
    // C1(t) = max(0.0 , (D + residual * R1) * ( 1 - t * S1) * M1 + A1)
    //
    // So:
    // C0(t) = max(0.0 , (D + residual * 2) * ( 1 - t * 0.03) * 4 + 1)
    // C1(t) = max(0.0 , (D + residual * 20) * ( 1 - t * 0.003) * 40 + 10)
    //

    for (int i = 0; i < CYCLE_SIZE; i++) {
        ASSERT_DOUBLE_EQ(
                concentrations[0][i],
                std::max(0.0, (200.0 + inResiduals[0] * 2.0) * (1.0 - times[i] * 0.03) * 4.0 + 1.0));
        ASSERT_DOUBLE_EQ(
                concentrations[1][i],
                std::max(0.0, (200.0 + inResiduals[0] * 20.0) * (1.0 - times[i] * 0.003) * 40.0 + 10.0));
    }

    testCalculator<Tucuxi::Core::MultiConstantEliminationBolus>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}
