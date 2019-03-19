/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_CONSTANTELIMINATIONBOLUS_H
#define TEST_CONSTANTELIMINATIONBOLUS_H

#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "fructose/fructose.h"

#include "tucucore/drugmodelimport.h"
#include "tucucore/pkmodel.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"

#include "buildconstantelimination.h"
#include "../pkmodels/constanteliminationbolus.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;


struct TestConstantEliminationBolus : public fructose::test_base<TestConstantEliminationBolus>
{
    TestConstantEliminationBolus() { }

    void buildDrugTreatment(DrugTreatment *&_drugTreatment, FormulationAndRoute _route)
    {

        _drugTreatment = new DrugTreatment();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::INTRAVASCULAR);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0),
                                 _route,
                                 Duration(),
                                 Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> sept2018(new Tucuxi::Core::DosageTimeRange(startSept2018, repeatedDose));


        _drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);
    }


    /// \brief Check that objects are correctly constructed by the constructor.
    void testConstantElimination(const std::string& /* _testName */)
    {

        BuildConstantElimination builder;
        DrugModel *drugModel = builder.buildDrugModel();

        fructose_assert(drugModel != nullptr);

        fructose_assert(drugModel->checkInvariants());



        {
            DrugModelChecker checker;

            std::shared_ptr<PkModel> sharedPkModel;
            sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

            bool addResult = sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::INTRAVASCULAR, ConstantEliminationBolus::getCreator());
            fructose_assert(addResult);

            PkModelCollection *collection = new PkModelCollection();
            collection->addPkModel(sharedPkModel);
            DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel, collection);

            fructose_assert(checkerResult.ok);

            if (!checkerResult.ok) {
                std::cout << checkerResult.errorMessage << std::endl;
            }

            // Now the drug model is ready to be used


            IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

            fructose_assert( component != nullptr);

            static_cast<ComputingComponent *>(component)->setPkModelCollection(collection);

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::INTRAVASCULAR);

            buildDrugTreatment(drugTreatment, route);


            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);

            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
            computingTraits->addTrait(std::move(traits));

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Success);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
            for(std::size_t i = 0; i < responses.size(); i++) {
                fructose_assert(dynamic_cast<SinglePredictionResponse*>(responses[i].get()) != nullptr);
                const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[i].get());
                std::vector<CycleData> data = resp->getData();
                fructose_assert(data.size() == 16);
                fructose_assert(data[0].m_concentrations.size() == 1);
                fructose_assert(data[0].m_concentrations[0][0] == 200000.0);
                DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                       Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

                fructose_assert(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0 == startSept2018.toSeconds());
                fructose_assert(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0 == startSept2018.toSeconds() + 3600.0 * 6.0);
            }


            {
                // Ask for 15 intakes, without the first one.
                RequestResponseId requestResponseId = "1";
                Tucuxi::Common::DateTime start(2018_y / sep / 1, 14h + 0min);
                Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
                double nbPointsPerHour = 10.0;
                ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
                std::unique_ptr<ComputingTraitConcentration> traits =
                        std::make_unique<ComputingTraitConcentration>(
                            requestResponseId, start, end, nbPointsPerHour, computingOption);

                std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
                computingTraits->addTrait(std::move(traits));

                ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

                std::unique_ptr<ComputingResponse> partialResponse = std::make_unique<ComputingResponse>(requestResponseId);

                ComputingResult result;
                result = component->compute(request, partialResponse);

                fructose_assert( result == ComputingResult::Success);

                const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = partialResponse.get()->getResponses();
                for(std::size_t i = 0; i < responses.size(); i++) {
                    fructose_assert(dynamic_cast<SinglePredictionResponse*>(responses[i].get()) != nullptr);
                    const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[i].get());
                    std::vector<CycleData> data = resp->getData();
                    fructose_assert(data.size() == 15);
                    fructose_assert(data[0].m_concentrations.size() == 1);
                    fructose_assert(data[0].m_concentrations[0][0] == 200000);
                    DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                           Duration(std::chrono::hours(14), std::chrono::minutes(0), std::chrono::seconds(0)));

                    fructose_assert(data[0].m_start.toSeconds() + data[0].m_times[0][0] * 3600.0 == startSept2018.toSeconds());
                    fructose_assert(data[1].m_start.toSeconds() + data[1].m_times[0][0] * 3600.0 == startSept2018.toSeconds() + 3600.0 * 6.0);
                }
            }

            delete drugTreatment;
            delete component;
        }

        // Delete all dynamically allocated objects
        delete drugModel;

    }
};



#endif // TEST_CONSTANTELIMINATIONBOLUS_H
