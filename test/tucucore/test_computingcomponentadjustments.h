//@@license@@

#ifndef TEST_COMPUTINGCOMPONENTADJUSTMENTS_H
#define TEST_COMPUTINGCOMPONENTADJUSTMENTS_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <typeinfo>

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "drugmodels/buildimatinib.h"
#include "fructose/fructose.h"

using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

struct TestComputingComponentAdjusements : public fructose::test_base<TestComputingComponentAdjusements>
{
    TestComputingComponentAdjusements() {}

    std::unique_ptr<DrugTreatment> buildDrugTreatment(const FormulationAndRoute& _route, DoseValue _doseValue = 200)
    {
        auto drugTreatment = std::make_unique<DrugTreatment>();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(_doseValue, TucuUnit("mg"), _route, Duration(), Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        auto sept2018 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startSept2018, repeatedDose);

        drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);

        return drugTreatment;
    }

    void testImatinibLastFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        auto drugTreatment = buildDrugTreatment(route);

        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::WithinTreatmentTimeRange,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 7 valid adjustment candidates
        fructose_assert(resp->getAdjustments().size() == 7);

        for (auto const& adj : resp->getAdjustments()) {
            fructose_assert(adj.m_history.getDosageTimeRanges()[0]->getStartDate() == adjustmentTime);
            fructose_assert(adj.m_history.getLastFormulationAndRoute() == route);
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testImatinibDefaultFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        auto drugTreatment = buildDrugTreatment(route);

        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::WithinTreatmentTimeRange,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 7 valid adjustment candidates
        fructose_assert(resp->getAdjustments().size() == 7);

        for (auto const& adj : resp->getAdjustments()) {
            fructose_assert(
                    adj.m_history.getLastFormulationAndRoute()
                    == drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute());
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testImatinibAllFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        // TODO : Check this test. It does not work anymore because of multiple routes not supported by the PK model
        /*
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

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq( result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData *resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 7 valid adjustment candidates
        fructose_assert(resp->getAdjustments().size() != 7);

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
        */
    }

    void testImatinibLastFormulationAndRouteBestDosage(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        auto drugTreatment = buildDrugTreatment(route);

        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::WithinTreatmentTimeRange,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 7 valid adjustment candidates
        fructose_assert(resp->getAdjustments().size() == 1);

        for (auto const& adj : resp->getAdjustments()) {
            fructose_assert(adj.m_history.getLastFormulationAndRoute() == route);
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testImatinibDefaultFormulationAndRouteBestDosage(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        auto drugTreatment = buildDrugTreatment(route);

        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::WithinTreatmentTimeRange,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 7 valid adjustment candidates
        fructose_assert(resp->getAdjustments().size() == 1);

        for (auto const& adj : resp->getAdjustments()) {
            fructose_assert(
                    adj.m_history.getLastFormulationAndRoute()
                    == drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute());
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testImatinibAllFormulationAndRouteBestDosage(const std::string& /* _testName */)
    {
        // TODO : Check this test. It does not work anymore because of multiple routes not supported by the PK model
        /*
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

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq( result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData *resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 1 valid adjustment candidate
        fructose_assert(resp->getAdjustments().size() == 1);

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
        */
    }



    void testImatinibEmptyTreatmentDefaultFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        // We start with an empty treatment
        auto drugTreatment = std::make_unique<DrugTreatment>();

        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 4, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 10, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::WithinTreatmentTimeRange,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::DefaultFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);
        TMP_UNUSED_PARAMETER(resp);
        // We expect 7 valid adjustment candidates
        //fructose_assert(resp->getAdjustments().size() == 7);

        //            for (auto const & adj : resp->getAdjustments()) {
        //              fructose_assert(adj.m_history.getLastFormulationAndRoute() == drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute() );
        //        }

        // Delete all dynamically allocated objects
        delete component;
    }




    void testImatinibSteadyStateLastFormulationAndRouteAllDosages(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        auto drugTreatment = buildDrugTreatment(route);


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::AtSteadyState,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 4 valid adjustment candidates
        fructose_assert(resp->getAdjustments().size() == 4);

        for (auto const& adj : resp->getAdjustments()) {
            fructose_assert(adj.m_history.getLastFormulationAndRoute() == route);
        }

        // Delete all dynamically allocated objects
        delete component;


        // Tests with only a candidate per interval
        {

            IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

            fructose_assert(component != nullptr);

            BuildImatinib builder;
            auto drugModel = builder.buildDrugModel();
            fructose_assert(drugModel != nullptr);

            const FormulationAndRoute route(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

            auto drugTreatment = buildDrugTreatment(route);


            // Construct the adjustment traits object
            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
            Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
            BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosagePerInterval;
            std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                    requestResponseId,
                    start,
                    end,
                    nbPointsPerHour,
                    computingOption,
                    adjustmentTime,
                    adjustmentOption,
                    LoadingOption::NoLoadingDose,
                    RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::AtSteadyState,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::LastFormulationAndRoute);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);


            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
            const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

            // We expect 2 valid adjustment candidates
            fructose_assert(resp->getAdjustments().size() == 2);

            for (auto const& adj : resp->getAdjustments()) {
                fructose_assert(adj.m_history.getLastFormulationAndRoute() == route);
            }

            // Delete all dynamically allocated objects
            delete component;
        }
    }


    void testImatinibLastFormulationAndRouteAllDosagesAtSteadyState(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        auto drugTreatment = std::make_unique<DrugTreatment>();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startJun2018(
                date::year_month_day(date::year(2018), date::month(6), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0), TucuUnit("mg"), route, Duration(), Duration(std::chrono::hours(24)));
        DosageRepeat repeatedDose(periodicDose, 300);
        auto jun2018 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startJun2018, repeatedDose);

        drugTreatment->getModifiableDosageHistory().addTimeRange(*jun2018);

        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 4, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::AllDosages;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::NoRestPeriod,
                SteadyStateTargetOption::WithinTreatmentTimeRange,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::LastFormulationAndRoute);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 4 valid adjustment candidates
        fructose_assert(resp->getAdjustments().size() == 4);

        for (auto const& adj : resp->getAdjustments()) {
            fructose_assert(adj.m_history.getDosageTimeRanges()[0]->getStartDate() == adjustmentTime);
            fructose_assert(adj.m_history.getLastFormulationAndRoute() == route);
        }

        // Delete all dynamically allocated objects
        delete component;
    }

    void testImatinibAllFormulationAndRouteBestDosageLoadingDose(const std::string& /* _testName */)
    {
        // TODO : Check this test. It does not work anymore because of multiple routes not supported by the PK model
        /*
        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);

        DrugModel* drugModel;
        BuildImatinib builder;
        drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        buildDrugTreatment(drugTreatment, route, DoseValue(2));


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
                    adjustmentOption, LoadingOption::LoadingDoseAllowed, RestPeriodOption::NoRestPeriod,
                    SteadyStateTargetOption::WithinTreatmentTimeRange,
                    TargetExtractionOption::PopulationValues,
                    FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq( result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);
        const AdjustmentData *resp = dynamic_cast<const AdjustmentData*>(responseData);

        // We expect 2 dosage time range (loading dose)
        fructose_assert(resp->getAdjustments()[0].getDosageHistory().getDosageTimeRanges().size() == 2);

        // Delete all dynamically allocated objects
        delete drugModel;
        delete drugTreatment;
        delete component;
        */
    }

    void testImatinibAllFormulationAndRouteBestDosageRestPeriod(const std::string& /* _testName */)
    {
        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        BuildImatinib builder;
        auto drugModel = builder.buildDrugModel();
        fructose_assert(drugModel != nullptr);

        const FormulationAndRoute route(
                Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);

        auto drugTreatment = buildDrugTreatment(route, DoseValue(20000));


        // Construct the adjustment traits object
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        Tucuxi::Common::DateTime adjustmentTime(2018_y / sep / 3, 8h + 0min);
        BestCandidatesOption adjustmentOption = BestCandidatesOption::BestDosage;
        std::unique_ptr<ComputingTraitAdjustment> adjustmentsTraits = std::make_unique<ComputingTraitAdjustment>(
                requestResponseId,
                start,
                end,
                nbPointsPerHour,
                computingOption,
                adjustmentTime,
                adjustmentOption,
                LoadingOption::NoLoadingDose,
                RestPeriodOption::RestPeriodAllowed,
                SteadyStateTargetOption::AtSteadyState,
                TargetExtractionOption::PopulationValues,
                FormulationAndRouteSelectionOption::AllFormulationAndRoutes);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(adjustmentsTraits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        fructose_assert_eq(result, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        fructose_assert(dynamic_cast<const AdjustmentData*>(responseData) != nullptr);

        const AdjustmentData* resp = dynamic_cast<const AdjustmentData*>(responseData);

        if (resp) {
            // We expect 2 dosage time range (rest period)
            fructose_assert(resp->getAdjustments()[0].getDosageHistory().getDosageTimeRanges().size() == 2);
        }

        // Delete all dynamically allocated objects
        delete component;
    }
};

#endif // TEST_COMPUTINGCOMPONENTADJUSTMENTS_H
