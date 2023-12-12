//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "gtest_core.h"

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/concentrationcalculator.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/montecarlopercentilecalculator.h"
#include "tucucore/percentilesprediction.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/residualerrormodel.h"

TEST (Core_TestPercentileCalculator, AprioriInvalidParameters){
    // Apriori Monte Carlo Percentile with invalid parameters

    // Simple test with imatinib values
    // Here the volume is set to 0 with proportional variability, so all predictions should
    // fail, and the calculator should handle that correctly

    Tucuxi::Core::PercentilesPrediction percentiles;
    Tucuxi::Core::IntakeSeries intakeSeries;
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::ComputingAborter* aborter = nullptr;

    // Build parameters as Imatinib ones
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL",
            15.106,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V",
            0.0,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    parametersSeries.addParameterSetEvent(parameters);

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = 24h;
    Tucuxi::Common::Duration infusionTime = 0h;
    double dose = 400;
    Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;

    DateTime recordFrom = now;
    DateTime recordTo = now + 96h;

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 =
            std::make_shared<Tucuxi::Core::OneCompartmentExtraMacro>();
    intakeEvent.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent);

    Tucuxi::Core::IntakeEvent intakeEvent2(
            now + 24h,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    intakeEvent2.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent2);

    Tucuxi::Core::IntakeEvent intakeEvent3(
            now + 48h,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    intakeEvent3.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent3);

    Tucuxi::Core::IntakeEvent intakeEvent4(
            now + 72h,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    intakeEvent4.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent4);

    // std::cout << typeid(calculator).name() << std::endl;

    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
    {
        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
        auto status = concentrationCalculator->computeConcentrations(
                predictionPtr, false, recordFrom, recordTo, intakeSeries, parametersSeries);

        ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::BadParameters);
    }

    percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);


    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;         // Variance of CL
    omega(0, 1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1, 1) = 0.629 * 0.629;         // Variance of V
    omega(1, 0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    auto calculator = std::make_unique<Tucuxi::Core::AprioriMonteCarloPercentileCalculator>();

    Tucuxi::Core::ComputingStatus res;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
    res = calculator->calculate(
            percentiles,
            recordFrom,
            recordTo,
            intakeSeries,
            parametersSeries,
            omega,
            residualErrorModel,
            etas,
            percentileRanks,
            concentrationCalculator,
            aborter);

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::PercentilesNoValidPrediction);
}

TEST (Core_TestPercentileCalculator, Apriori){
    // Apriori Monte Carlo Percentile

    // Simple test with imatinib values

    Tucuxi::Core::PercentilesPrediction percentiles;
    Tucuxi::Core::IntakeSeries intakeSeries;
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::ComputingAborter* aborter = nullptr;

    // Build parameters as Imatinib ones
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL",
            15.106,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V",
            347,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    parametersSeries.addParameterSetEvent(parameters);

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = 24h;
    Tucuxi::Common::Duration infusionTime = 0h;
    double dose = 400;
    Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;

    DateTime recordFrom = now;
    DateTime recordTo = now + 96h;

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 =
            std::make_shared<Tucuxi::Core::OneCompartmentExtraMacro>();
    intakeEvent.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent);

    Tucuxi::Core::IntakeEvent intakeEvent2(
            now + 24h,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    intakeEvent2.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent2);

    Tucuxi::Core::IntakeEvent intakeEvent3(
            now + 48h,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    intakeEvent3.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent3);

    Tucuxi::Core::IntakeEvent intakeEvent4(
            now + 72h,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    intakeEvent4.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent4);

    // std::cout << typeid(calculator).name() << std::endl;

    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
    {
        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
        auto status = concentrationCalculator->computeConcentrations(
                predictionPtr, false, recordFrom, recordTo, intakeSeries, parametersSeries);

        ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::Ok);

        predictionPtr->streamToFile("values_imatinib_percentile.dat");
    }
    /*
        for(int i = 0; i < _nbPoints; i++) {
            Tucuxi::Core::Concentrations concentration2;
            concentration2 = predictionPtr->getValues()[0];
            // std::cout << i <<  " :: " << concentrations[0][i] << " : " << concentration2[i] << std::endl;

            // compare concentrations of compartment 1
            ASSERT_DOUBLE_EQ(concentrations[0][i], concentration2[i]);
        }
*/

    percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);


    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;         // Variance of CL
    omega(0, 1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1, 1) = 0.629 * 0.629;         // Variance of V
    omega(1, 0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    auto calculator = std::make_unique<Tucuxi::Core::AprioriMonteCarloPercentileCalculator>();

    Tucuxi::Core::ComputingStatus res;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
    res = calculator->calculate(
            percentiles,
            recordFrom,
            recordTo,
            intakeSeries,
            parametersSeries,
            omega,
            residualErrorModel,
            etas,
            percentileRanks,
            concentrationCalculator,
            aborter);

    // percentiles.streamToFile("apriori_percentiles_imatinib.dat");

    // std::cout << "Apriori Percentile result is saved" << std::endl;

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
}

TEST (Core_TestPercentileCalculator, AposterioriNormal){
    // Aposteriori Normal Monte Carlo Percentile

    Tucuxi::Core::IntakeSeries intakeSeries;
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::PercentilesPrediction percentiles;
    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::ComputingAborter* aborter = nullptr;

    // Build parameters as Imatinib ones
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL",
            15.106,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V",
            347,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    parametersSeries.addParameterSetEvent(parameters);

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = 24h;
    Tucuxi::Common::Duration infusionTime = 0h;
    double dose = 400;
    Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;

    DateTime recordFrom = now;
    DateTime recordTo = now + 24h;

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 =
            std::make_shared<Tucuxi::Core::OneCompartmentExtraMacro>();
    intakeEvent.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent);

    // std::cout << typeid(calculator).name() << std::endl;

    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
    {
        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
        auto status = concentrationCalculator->computeConcentrations(
                predictionPtr, false, recordFrom, recordTo, intakeSeries, parametersSeries);
        ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::Ok);
        predictionPtr->streamToFile("values_imatinib_percentile.dat");
    }

    percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;         // Variance of CL
    omega(0, 1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1, 1) = 0.629 * 0.629;         // Variance of V
    omega(1, 0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    Tucuxi::Common::Duration sampleOffset = 2h;
    Tucuxi::Core::SampleEvent sampleEvent(now + sampleOffset, 200);
    Tucuxi::Core::SampleSeries sampleSeries;
    sampleSeries.push_back(sampleEvent);

    std::unique_ptr<Tucuxi::Core::IAposterioriNormalApproximationMonteCarloPercentileCalculator> calculator =
            std::make_unique<Tucuxi::Core::AposterioriNormalApproximationMonteCarloPercentileCalculator>();

    Tucuxi::Core::ComputingStatus res;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
    res = calculator->calculate(
            percentiles,
            recordFrom,
            recordTo,
            intakeSeries,
            parametersSeries,
            omega,
            residualErrorModel,
            etas,
            sampleSeries,
            percentileRanks,
            concentrationCalculator,
            aborter);

    // percentiles.streamToFile("aposteriori_normal_percentiles_imatinib.dat");

    // std::cout << "Aposteriori Normal Percentile result is saved" << std::endl;

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
}

TEST (Core_TestPercentileCalculator, AposterioriMatrixCache){
    // Aposteriori Matrix cache

    // Here the volume is set to 0 with proportional variability, so all predictions should
    // fail, and the calculator should handle that correctly

    Tucuxi::Core::IntakeSeries intakeSeries;
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::PercentilesPrediction percentiles;
    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::ComputingAborter* aborter = nullptr;

    // Build parameters as Imatinib ones
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL",
            15.106,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V",
            0,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    parametersSeries.addParameterSetEvent(parameters);

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = 24h;
    Tucuxi::Common::Duration infusionTime = 0h;
    double dose = 400;
    Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;

    DateTime recordFrom = now;
    DateTime recordTo = now + 24h;

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 =
            std::make_shared<Tucuxi::Core::OneCompartmentExtraMacro>();
    intakeEvent.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent);

    // std::cout << typeid(calculator).name() << std::endl;

    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
    {
        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
        auto status = concentrationCalculator->computeConcentrations(
                predictionPtr, false, recordFrom, recordTo, intakeSeries, parametersSeries);
        ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::BadParameters);
    }

    percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;         // Variance of CL
    omega(0, 1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1, 1) = 0.629 * 0.629;         // Variance of V
    omega(1, 0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    Tucuxi::Common::Duration sampleOffset = 2h;
    Tucuxi::Core::SampleEvent sampleEvent(now + sampleOffset, 1000);
    Tucuxi::Core::SampleSeries sampleSeries;
    sampleSeries.push_back(sampleEvent);

    std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator =
            std::make_unique<Tucuxi::Core::AposterioriMonteCarloPercentileCalculator>();

    Tucuxi::Core::ComputingStatus res;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
    res = calculator->calculate(
            percentiles,
            recordFrom,
            recordTo,
            intakeSeries,
            parametersSeries,
            omega,
            residualErrorModel,
            etas,
            sampleSeries,
            percentileRanks,
            concentrationCalculator,
            aborter);

    // percentiles.streamToFile("aposteriori_percentiles_imatinib.dat");

    // std::cout << "Aposteriori Percentile result is saved" << std::endl;

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::PercentilesNoValidPrediction);
}

TEST (Core_TestPercentileCalculator, Aposteriori){
    // Aposteriori Monte Carlo Percentile

    Tucuxi::Core::IntakeSeries intakeSeries;
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::PercentilesPrediction percentiles;
    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::ComputingAborter* aborter = nullptr;

    // Build parameters as Imatinib ones
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL",
            15.106,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V",
            347,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    parametersSeries.addParameterSetEvent(parameters);

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = 24h;
    Tucuxi::Common::Duration infusionTime = 0h;
    double dose = 400;
    Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;

    DateTime recordFrom = now;
    DateTime recordTo = now + 24h;

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 =
            std::make_shared<Tucuxi::Core::OneCompartmentExtraMacro>();
    intakeEvent.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent);

    // std::cout << typeid(calculator).name() << std::endl;

    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
    {
        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
        auto status = concentrationCalculator->computeConcentrations(
                predictionPtr, false, recordFrom, recordTo, intakeSeries, parametersSeries);
        ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::Ok);
        predictionPtr->streamToFile("values_imatinib_percentile.dat");
    }

    percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;         // Variance of CL
    omega(0, 1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1, 1) = 0.629 * 0.629;         // Variance of V
    omega(1, 0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    Tucuxi::Common::Duration sampleOffset = 2h;
    Tucuxi::Core::SampleEvent sampleEvent(now + sampleOffset, 1.000);
    Tucuxi::Core::SampleSeries sampleSeries;
    sampleSeries.push_back(sampleEvent);

    std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator =
            std::make_unique<Tucuxi::Core::AposterioriMonteCarloPercentileCalculator>();

    Tucuxi::Core::ComputingStatus res;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
    res = calculator->calculate(
            percentiles,
            recordFrom,
            recordTo,
            intakeSeries,
            parametersSeries,
            omega,
            residualErrorModel,
            etas,
            sampleSeries,
            percentileRanks,
            concentrationCalculator,
            aborter);

    // percentiles.streamToFile("aposteriori_percentiles_imatinib.dat");

    // std::cout << "Aposteriori Percentile result is saved" << std::endl;

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
}

TEST (Core_TestPercentileCalculator, AposterioriUnlikelySample){
    // Aposteriori Monte Carlo Percentile Unlikely Sample

    Tucuxi::Core::IntakeSeries intakeSeries;
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::PercentilesPrediction percentiles;
    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::ComputingAborter* aborter = nullptr;

    // Build parameters as Imatinib ones
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL",
            15.106,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V",
            347,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    parametersSeries.addParameterSetEvent(parameters);

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = 24h;
    Tucuxi::Common::Duration infusionTime = 0h;
    double dose = 400;
    Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;

    DateTime recordFrom = now;
    DateTime recordTo = now + 24h;

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 =
            std::make_shared<Tucuxi::Core::OneCompartmentExtraMacro>();
    intakeEvent.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent);

    // std::cout << typeid(calculator).name() << std::endl;

    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
    {
        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
        auto status = concentrationCalculator->computeConcentrations(
                predictionPtr, false, recordFrom, recordTo, intakeSeries, parametersSeries);
        ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::Ok);
        predictionPtr->streamToFile("values_imatinib_percentile.dat");
    }

    percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;         // Variance of CL
    omega(0, 1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1, 1) = 0.629 * 0.629;         // Variance of V
    omega(1, 0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    Tucuxi::Common::Duration sampleOffset = 2h;
    Tucuxi::Core::SampleEvent sampleEvent(now + sampleOffset, 1000);
    Tucuxi::Core::SampleSeries sampleSeries;
    sampleSeries.push_back(sampleEvent);

    std::unique_ptr<Tucuxi::Core::IAposterioriPercentileCalculator> calculator =
            std::make_unique<Tucuxi::Core::AposterioriMonteCarloPercentileCalculator>();

    Tucuxi::Core::ComputingStatus res;

    Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
    res = calculator->calculate(
            percentiles,
            recordFrom,
            recordTo,
            intakeSeries,
            parametersSeries,
            omega,
            residualErrorModel,
            etas,
            sampleSeries,
            percentileRanks,
            concentrationCalculator,
            aborter);

    // percentiles.streamToFile("aposteriori_percentiles_imatinib.dat");

    // std::cout << "Aposteriori Percentile result is saved" << std::endl;

    ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::AposterioriPercentilesNoLikelySample);
}

TEST (Core_TestPercentileCalculator, AposterioriInvalidParameters){
    // Aposteriori Monte Carlo Percentile with invalid parameters

    Tucuxi::Core::IntakeSeries intakeSeries;
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    Tucuxi::Core::OmegaMatrix omega;
    Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
    Tucuxi::Core::Etas etas;
    Tucuxi::Core::PercentilesPrediction percentiles;
    Tucuxi::Core::PercentileRanks percentileRanks;
    Tucuxi::Core::ComputingAborter* aborter = nullptr;

    // Build parameters as Imatinib ones
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "CL",
            15.106,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "V",
            347,
            std::make_unique<Tucuxi::Core::ParameterVariability>(
                    Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None));
    parameterDefs.push_back(std::make_unique<Tucuxi::Core::ParameterDefinition>(
            "F", 1, Tucuxi::Core::ParameterVariabilityType::None));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
    parametersSeries.addParameterSetEvent(parameters);

    DateTime now = DateTime::now();
    Tucuxi::Common::Duration offsetTime = 0s;
    Tucuxi::Common::Duration interval = 24h;
    Tucuxi::Common::Duration infusionTime = 0h;
    double dose = 400;
    Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::Extravascular;

    DateTime recordFrom = now;
    DateTime recordTo = now + 24h;

    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(
            now,
            offsetTime,
            dose,
            Tucuxi::Common::TucuUnit("mg"),
            interval,
            Tucuxi::Core::FormulationAndRoute(route),
            route,
            infusionTime,
            CYCLE_SIZE);
    std::shared_ptr<Tucuxi::Core::IntakeIntervalCalculator> calculator2 =
            std::make_shared<Tucuxi::Core::OneCompartmentExtraMacro>();
    intakeEvent.setCalculator(calculator2);
    intakeSeries.push_back(intakeEvent);

    // std::cout << typeid(calculator).name() << std::endl;

    Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
    {
        predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

        auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
        auto status = concentrationCalculator->computeConcentrations(
                predictionPtr, false, recordFrom, recordTo, intakeSeries, parametersSeries);
        ASSERT_EQ(status, Tucuxi::Core::ComputingStatus::Ok);
        predictionPtr->streamToFile("values_imatinib_percentile.dat");
    }

    percentileRanks = {5, 10, 25, 50, 75, 90, 95};

    Tucuxi::Core::Sigma sigma(1);
    sigma(0) = 0.3138;
    residualErrorModel.setErrorModel(Tucuxi::Core::ResidualErrorType::PROPORTIONAL);
    residualErrorModel.setSigma(sigma);

    omega = Tucuxi::Core::OmegaMatrix(2, 2);
    omega(0, 0) = 0.356 * 0.356;         // Variance of CL
    omega(0, 1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
    omega(1, 1) = 0.629 * 0.629;         // Variance of V
    omega(1, 0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

    // Set initial etas to 0 for CL and V
    etas.push_back(0.0);
    etas.push_back(0.0);

    {
        // The sample is way too early compared with the intake
        Tucuxi::Common::Duration sampleOffset = -365 * 2h;
        Tucuxi::Core::SampleEvent sampleEvent(now + sampleOffset, 1000);
        Tucuxi::Core::SampleSeries sampleSeries;
        sampleSeries.push_back(sampleEvent);

        auto calculator = std::make_unique<Tucuxi::Core::AposterioriMonteCarloPercentileCalculator>();

        Tucuxi::Core::ComputingStatus res;

        Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
        res = calculator->calculate(
                percentiles,
                recordFrom,
                recordTo,
                intakeSeries,
                parametersSeries,
                omega,
                residualErrorModel,
                etas,
                sampleSeries,
                percentileRanks,
                concentrationCalculator,
                aborter);

        // percentiles.streamToFile("aposteriori_percentiles_imatinib.dat");

        // std::cout << "Aposteriori Percentile result is saved" << std::endl;

        ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::AposterioriPercentilesOutOfScopeSamplesError);
    }
    {

        Tucuxi::Core::ParameterSetSeries parametersSeries;
        Tucuxi::Core::ParameterSetEvent parameters(DateTime::now(), parameterDefs);
        parametersSeries.addParameterSetEvent(parameters);


        // The sample is way too far away compared with the intake
        Tucuxi::Common::Duration sampleOffset = 365 * 2h;
        Tucuxi::Core::SampleEvent sampleEvent(now + sampleOffset, 1000);
        Tucuxi::Core::SampleSeries sampleSeries;
        sampleSeries.push_back(sampleEvent);

        auto calculator = std::make_unique<Tucuxi::Core::AposterioriMonteCarloPercentileCalculator>();

        Tucuxi::Core::ComputingStatus res;

        Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
        res = calculator->calculate(
                percentiles,
                recordFrom,
                recordTo,
                intakeSeries,
                parametersSeries,
                omega,
                residualErrorModel,
                etas,
                sampleSeries,
                percentileRanks,
                concentrationCalculator,
                aborter);

        // percentiles.streamToFile("aposteriori_percentiles_imatinib.dat");

        // std::cout << "Aposteriori Percentile result is saved" << std::endl;

        ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::AposterioriPercentilesOutOfScopeSamplesError);
    }
}

TEST (Core_TestPercentileCalculator, AposterioriFarSamples){
    // Aposteriori percentiles with samples far away in time

    Tucuxi::Core::AposterioriMatrixCache cache;

    const Tucuxi::Core::EigenMatrix& matrix0 = cache.getAvecs(10, 4);
    ASSERT_EQ(matrix0.rows(), static_cast<Eigen::Index>(10));
    ASSERT_EQ(matrix0.cols(), static_cast<Eigen::Index>(4));
    double value0 = matrix0(1, 3);

    const Tucuxi::Core::EigenMatrix& matrix1 = cache.getAvecs(12, 6);
    ASSERT_EQ(matrix1.rows(), static_cast<Eigen::Index>(12));
    ASSERT_EQ(matrix1.cols(), static_cast<Eigen::Index>(6));
    double value1 = matrix1(2, 3);

    const Tucuxi::Core::EigenMatrix& matrix0a = cache.getAvecs(10, 4);
    ASSERT_EQ(matrix0a.rows(), static_cast<Eigen::Index>(10));
    ASSERT_EQ(matrix0a.cols(), static_cast<Eigen::Index>(4));

    ASSERT_EQ(value0, matrix0a(1, 3));

    // As we have a const reference, both matrices share the address space
    ASSERT_EQ(matrix0(0, 0), matrix0a(0, 0));
    const Tucuxi::Core::EigenMatrix& matrix1a = cache.getAvecs(12, 6);
    ASSERT_EQ(matrix1a.rows(), static_cast<Eigen::Index>(12));
    ASSERT_EQ(matrix1a.cols(), static_cast<Eigen::Index>(6));

    ASSERT_EQ(value1, matrix1a(2, 3));
}
