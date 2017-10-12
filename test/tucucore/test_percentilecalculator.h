/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_PERCENTILECALCULATOR_H
#define TEST_PERCENTILECALCULATOR_H

#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/general.h"

#include "tucucore/montecarlopercentilecalculator.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/onecompartmentextra.h"
#include "tucucore/concentrationcalculator.h"



/*
* A fake A priori percentiles calculator
*/
class MockAprioriPercentileCalculator : public Tucuxi::Core::IAprioriPercentileCalculator {

public:

    MockAprioriPercentileCalculator() {}

    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _nbPoints Number of points asked for each cycle
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _initialEtas Set of initial Etas, used in case of a posteriori
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ///
    ProcessingResult calculate(
            Tucuxi::Core::PercentilesPrediction& _percentiles,
            const int _nbPoints,
            const Tucuxi::Core::IntakeSeries &_intakes,
            const Tucuxi::Core::ParameterSetSeries &_parameters,
            const Tucuxi::Core::OmegaMatrix& _omega,
            const Tucuxi::Core::IResidualErrorModel &_residualErrorModel,
            const Tucuxi::Core::Etas& _initialEtas,
            const Tucuxi::Core::PercentileRanks &_percentileRanks,
	    IConcentrationCalculator &_concentrationCalculator,
            Tucuxi::Core::ProcessingAborter *_aborter) override
    {
        TMP_UNUSED_PARAMETER(_intakes);
        UNUSED_PARAMETER(_parameters);
        UNUSED_PARAMETER(_omega);
        UNUSED_PARAMETER(_residualErrorModel);
        UNUSED_PARAMETER(_initialEtas);
        UNUSED_PARAMETER(_aborter);

        std::vector<double> times;
        for(int i = 0; i < _nbPoints; i++) {
            times.push_back(((double)i)*0.120603);
        }
        _percentiles.m_times.push_back(times);
        for(unsigned perc = 0; perc < _percentileRanks.size(); perc ++) {
            int percValue = _percentileRanks[perc];
            _percentiles.m_ranks.push_back(percValue);
            std::vector<double> vec;
            for(int i = 0; i < _nbPoints; i++) {
                vec.push_back(percValue + i);
            }
            std::vector<std::vector<double> > cycleVec;
            cycleVec.push_back(vec);
            _percentiles.m_values.push_back(cycleVec);

        }

        return ProcessingResult::Success;


    }


};

struct TestPercentileCalculator : public fructose::test_base<TestPercentileCalculator>
{

    TestPercentileCalculator() { }

    void test1(const std::string& /* _testName */)
    {
        // Simple test with imatinib values

        Tucuxi::Core::PercentilesPrediction percentiles;
        int nbPoints;
        Tucuxi::Core::IntakeSeries intakeSeries;
        Tucuxi::Core::ParameterSetSeries parametersSeries;
        Tucuxi::Core::OmegaMatrix omega;
        Tucuxi::Core::SigmaResidualErrorModel residualErrorModel;
        Tucuxi::Core::Etas etas;
        Tucuxi::Core::PercentileRanks percentileRanks;
        Tucuxi::Core::ProcessingAborter *aborter = nullptr;

        nbPoints = 200;


        // Build parameters as Imatinib ones
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("CL", 15.106, Tucuxi::Core::ParameterVariability(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356))));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterVariability(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629))));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
        parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);
        parametersSeries.addParameterSetEvent(parameters);




        DateTime now;
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = 24h;
        Tucuxi::Common::Duration infusionTime = 0h;
        double dose = 400;
        Tucuxi::Core::AbsorptionModel route = Tucuxi::Core::AbsorptionModel::EXTRAVASCULAR;


        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, dose, interval, route, infusionTime, nbPoints);
        Tucuxi::Core::OneCompartmentExtraMacro calculator2;
        intakeEvent.setCalculator(&calculator2);
        intakeSeries.push_back(intakeEvent);

        // std::cout << typeid(calculator).name() << std::endl;


        Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

            Tucuxi::Core::IConcentrationCalculator *concentrationCalculator = new Tucuxi::Core::ConcentrationCalculator();
            concentrationCalculator->computeConcentrations(
                        predictionPtr,
                        false,
                        nbPoints,
                        intakeSeries,
                        parametersSeries);
            delete concentrationCalculator;

            predictionPtr->streamToFile("values_imatinib_percentile.dat");
        }
/*
        for(int i = 0; i < _nbPoints; i++) {
            Tucuxi::Core::Concentrations concentration2;
            concentration2 = predictionPtr->getValues()[0];
            // std::cout << i <<  " :: " << concentrations[0][i] << " : " << concentration2[i] << std::endl;

            // compare concentrations of compartment 1
            fructose_assert_double_eq(concentrations[0][i], concentration2[i]);
        }
*/

        percentileRanks = {5, 10, 25, 50, 75, 90, 95};

        Sigma sigma(1);
        sigma(0) = 0.3138;
        residualErrorModel.setErrorModel(SigmaResidualErrorModel::ResidualErrorType::PROPORTIONAL);
        residualErrorModel.setSigma(sigma);


        omega = Tucuxi::Core::OmegaMatrix(2,2);
        omega(0,0) = 0.356 * 0.356; // Variance of CL
        omega(0,1) = 0.798 * 0.356 * 0.629; // Covariance of CL and V
        omega(1,1) = 0.629 * 0.629; // Variance of V
        omega(1,0) = 0.798 * 0.356 * 0.629; // Covariance of CL and V

        // Set initial etas to 0 for CL and V
        etas.push_back(0.0);
        etas.push_back(0.0);


        std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculator> calculator =
                std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculator>(
                    new MockAprioriPercentileCalculator());

//        std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculator> calculator =
//                std::unique_ptr<Tucuxi::Core::IAprioriPercentileCalculator>(
//                    new Tucuxi::Core::AprioriMonteCarloPercentileCalculator());

        Tucuxi::Core::IPercentileCalculator::ProcessingResult res;

        Tucuxi::Core::ConcentrationCalculator concentrationCalculator;
        res = calculator->calculate(
                    percentiles,
                    nbPoints,
                    intakeSeries,
                    parametersSeries,
                    omega,
                    residualErrorModel,
                    etas,
                    percentileRanks,
		    concentrationCalculator,
                    aborter);

        percentiles.streamToFile("percentiles_imatinib.dat");


        fructose_assert(res == Tucuxi::Core::IPercentileCalculator::ProcessingResult::Success);
    }


};

#endif // TEST_PERCENTILECALCULATOR_H
