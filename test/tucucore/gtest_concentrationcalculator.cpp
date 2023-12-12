//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "gtest_core.h"

#include "tucucore/dosage.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"

#include "pkmodels/constanteliminationbolus.h"

using namespace Tucuxi::Core;

TEST (Core_TestConcentrationCalculator, ConstantEliminationBolus){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestA", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestR", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestS", 0.0, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "TestM", 1.0, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::ConstantEliminationBolus>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, oneCompBolus){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, oneCompExtra){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0435331, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, oneCompInfusion){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V", 347, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 22.97, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::OneCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, twoCompBolus){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, twoCompExtra){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, twoCompInfusion){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::TwoCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}

// TODO Active following test after fixing input parameters
TEST (Core_TestConcentrationCalculator, DISABLED_threeCompBolus){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::ThreeCompartmentBolusMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Intravascular, 12h, 0s, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, DISABLED_threeCompExtra){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::ThreeCompartmentExtraMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Extravascular, 12h, 0s, CYCLE_SIZE);
}

TEST (Core_TestConcentrationCalculator, DISABLED_threeCompInfusion){
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 2, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V1", 340, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ke", 0.0444294, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K12", 0.0588235, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K21", 0.0584795, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K13", 0.0882353, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "K31", 0.0877193, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters);

    testCalculator<Tucuxi::Core::ThreeCompartmentInfusionMicro>(
            parametersSeries, 400.0, Tucuxi::Core::AbsorptionModel::Infusion, 12h, 1h, CYCLE_SIZE);
}
