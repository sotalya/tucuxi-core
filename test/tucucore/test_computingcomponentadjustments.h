/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_COMPUTINGCOMPONENTADJUSTMENTS_H
#define TEST_COMPUTINGCOMPONENTADJUSTMENTS_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <typeinfo>

#include "fructose/fructose.h"

#include "tucucommon/datetime.h"

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

struct TestComputingComponentAdjusements : public fructose::test_base<TestComputingComponentAdjusements>
{
    TestComputingComponentAdjusements() { }

    void buildDrugTreatment(DrugTreatment *&_drugTreatment, FormulationAndRoute _route)
    {
         _drugTreatment = new DrugTreatment();

         // List of time ranges that will be pushed into the history
         DosageTimeRangeList timeRangeList;

         // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
         // are handled correctly)
         DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


         //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
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

    void testImatinibLastFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 7 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() == 7);

            for (auto const & adj : resp->getAdjustments()) {
                fructose_assert(adj.m_history.getDosageTimeRanges().at(0)->getStartDate() == adjustmentTime);
                fructose_assert(adj.m_history.getLastFormulationAndRoute() == route );
            }
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }

    void testImatinibDefaultFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 7 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() == 7);

            for (auto const & adj : resp->getAdjustments()) {
                fructose_assert(adj.m_history.getLastFormulationAndRoute() == drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute() );
            }
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }

    void testImatinibAllFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 7 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() != 7);
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }

    void testImatinibLastFormulationAndRouteBestDosage(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 7 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() == 1);

            for (auto const & adj : resp->getAdjustments()) {
                fructose_assert(adj.m_history.getLastFormulationAndRoute() == route );
            }
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }

    void testImatinibDefaultFormulationAndRouteBestDosage(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 7 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() == 1);

            for (auto const & adj : resp->getAdjustments()) {
                fructose_assert(adj.m_history.getLastFormulationAndRoute() == drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute() );
            }
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }

    void testImatinibAllFormulationAndRouteBestDosage(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 1 valid adjustment candidate
            fructose_assert(resp->getAdjustments().size() == 1);

        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }



    void testImatinibEmptyTreatmentDefaultFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;

        // We start with an empty treatment
        drugTreatment = new DrugTreatment();




        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 4, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 10, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());
            TMP_UNUSED_PARAMETER(resp);
            // We expect 7 valid adjustment candidates
            //fructose_assert(resp->getAdjustments().size() == 7);

//            for (auto const & adj : resp->getAdjustments()) {
  //              fructose_assert(adj.m_history.getLastFormulationAndRoute() == drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute() );
    //        }
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }




    void testImatinibSteadyStateLastFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::AtSteadyState,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 4 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() == 4);

            for (auto const & adj : resp->getAdjustments()) {
                fructose_assert(adj.m_history.getLastFormulationAndRoute() == route );
            }
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;


        // Tests with only a candidate per interval
        {

            IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

            fructose_assert( component != nullptr);

            DrugModel* drugModel;
            BuildImatinib builder;
            drugModel = builder.buildDrugModel();
            fructose_assert(drugModel != nullptr);

            DrugTreatment *drugTreatment;
            const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            buildDrugTreatment(drugTreatment, route);


            // Construct the adjustment traits object
            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
            Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
            BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosagePerInterval;
            std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                    std::make_unique<ComputingTraitAdjustment>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                        adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                        SteadyStateTargetOption::AtSteadyState,
                        TargetExtractionOption::PopulationValues,
                        FormulationAndRouteSelectionOption::LastFormulationAndRoute);

            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
            computingTraits->addTrait(std::move(adjustmentsTraits));

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Ok);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
            for(std::size_t i = 0; i < responses.size(); i++) {
                fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
                const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

                // We expect 2 valid adjustment candidates
                fructose_assert(resp->getAdjustments().size() == 2);

                for (auto const & adj : resp->getAdjustments()) {
                    fructose_assert(adj.m_history.getLastFormulationAndRoute() == route );
                }
            }

            // Delete all dynamically allocated objects
            delete drugModel;
            delete drugTreatment;
            delete component;
        }
    }


    void testImatinibLastFormulationAndRouteAllDosagesAtSteadyState(const std::string& /* _testName */)
    {
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);


        drugTreatment = new DrugTreatment();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startJun2018(date::year_month_day(date::year(2018), date::month(6), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0),
                                 route,
                                 Duration(),
                                 Duration(std::chrono::hours(24)));
        DosageRepeat repeatedDose(periodicDose, 300);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> jun2018(new Tucuxi::Core::DosageTimeRange(startJun2018, repeatedDose));


        drugTreatment->getModifiableDosageHistory().addTimeRange(*jun2018);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits =
                std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption, adjustmentTime,
                    adjustmentOption, LoadingOption::NoLoadingDose, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
        computingTraits->addTrait(std::move(adjustmentsTraits));

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingResult result;
        result = component->compute(request, response);

        fructose_assert( result == ComputingResult::Ok);

        const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
        for(std::size_t i = 0; i < responses.size(); i++) {
            fructose_assert(dynamic_cast<AdjustmentResponse*>(responses[i].get()) != nullptr);
            const AdjustmentResponse *resp = dynamic_cast<AdjustmentResponse*>(responses[i].get());

            // We expect 4 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() == 4);

            for (auto const & adj : resp->getAdjustments()) {
                fructose_assert(adj.m_history.getDosageTimeRanges().at(0)->getStartDate() == adjustmentTime);
                fructose_assert(adj.m_history.getLastFormulationAndRoute() == route );
            }
        }

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
    }


};

#endif // TEST_COMPUTINGCOMPONENTADJUSTMENTS_H
