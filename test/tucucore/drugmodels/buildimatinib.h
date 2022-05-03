//@@license@@

#ifndef BUILDIMATINIB_H
#define BUILDIMATINIB_H

#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/drugmodel/drugmodel.h"

#include "drugmodelbuilder.h"

using namespace Tucuxi::Core;

class BuildImatinib
{
public:
    BuildImatinib() {}

    std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel()
    {
        auto model = std::make_unique<Tucuxi::Core::DrugModel>();

        // The following constraint is for tests only. Needs to be modified according to the paper
        auto operationConstraint = std::make_unique<Tucuxi::Core::JSOperation>(
                "return (age > 0);", OperationInputList{OperationInput("age", InputType::DOUBLE)});

        auto constraint = std::make_unique<Constraint>();
        constraint->addRequiredCovariateId("age");
        constraint->setCheckOperation(std::move(operationConstraint));
        constraint->setType(ConstraintType::HARD);

        auto drugDomain = std::make_unique<DrugModelDomain>(std::move(constraint));

        model->setDomain(std::move(drugDomain));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "bodyweight", "70", nullptr, CovariateType::Standard));
        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("gist", "0", nullptr, CovariateType::Standard));
        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("sex", "0.5", nullptr, CovariateType::Standard));
        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("age", "50", nullptr, CovariateType::Standard));

        auto analyteSet = std::make_unique<AnalyteSet>();

        analyteSet->setId("imatinib");
        analyteSet->setPkModelId("linear.1comp.macro");

        auto analyte = std::make_unique<ActiveSubstance>();
        analyte->setAnalyteId("imatinib");

        auto errorModel = std::make_unique<ErrorModel>();

        errorModel->setErrorModel(ResidualErrorType::PROPORTIONAL);
        errorModel->addOriginalSigma(std::make_unique<PopulationValue>("sigma0", 0.3138));

        analyte->setResidualErrorModel(std::move(errorModel));

        // Add targets
        auto target = std::make_unique<TargetDefinition>(
                TargetType::Residual,
                TucuUnit("ug/l"),
                ActiveMoietyId("imatinib"),
                std::make_unique<SubTargetDefinition>("cMin", 750.0, nullptr),
                std::make_unique<SubTargetDefinition>("cMax", 1500.0, nullptr),
                std::make_unique<SubTargetDefinition>("cBest", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("mic", 0.0, nullptr),
                std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("tMax", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("tBest", 1000.0, nullptr),
                std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
                std::make_unique<SubTargetDefinition>("inefficacy", 000.0, nullptr));

        std::vector<AnalyteId> analyteList;
        analyteList.push_back(AnalyteId("imatinib"));
        std::unique_ptr<ActiveMoiety> activeMoiety =
                std::make_unique<ActiveMoiety>(ActiveMoietyId("imatinib"), TucuUnit("ug/l"), analyteList, nullptr);
        activeMoiety->addTarget(std::move(target));
        model->addActiveMoiety(std::move(activeMoiety));

        analyteSet->addAnalyte(std::move(analyte));

        auto dispositionParameters = std::make_unique<ParameterSetDefinition>();

        // Imatinib parameters, as in the XML drug file
        auto opV = std::make_unique<JSOperation>(
                "theta2=V_population; theta8=46.2; tvv = theta2+theta8*sex-theta8*(1-sex); V = tvv; return V;",
                OperationInputList{
                        OperationInput("V_population", InputType::DOUBLE), OperationInput("sex", InputType::DOUBLE)});

        auto PV = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "V",
                347,
                std::move(opV),
                std::make_unique<ParameterVariability>(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629));
        dispositionParameters->addParameter(std::move(PV));
        auto opCl = std::make_unique<JSOperation>(
                " \
                                            theta1=CL_population; \
                                            theta4=5.42; \
                                            theta5=1.49; \
                                            theta6=-5.81; \
                                            theta7=-0.806; \
                                            \
                                            MEANBW=70; \
                                            FBW=(bodyweight-MEANBW)/MEANBW; \
                                            \
                                            MEANAG=50; \
                                            FAGE=(age-MEANAG)/MEANAG; \
                                            \
                                            PATH = gist; \
                                            MALE = sex; \
                                            \
                                            Cl = theta1+theta4*FBW+theta5*MALE-theta5*(1-MALE)+theta6*FAGE + theta7*PATH-theta7*(1-PATH);\
                                            return Cl;",
                OperationInputList{
                        OperationInput("CL_population", InputType::DOUBLE),
                        OperationInput("sex", InputType::DOUBLE),
                        OperationInput("bodyweight", InputType::DOUBLE),
                        OperationInput("age", InputType::DOUBLE),
                        OperationInput("gist", InputType::BOOL)});

        auto PCL = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "CL",
                14.3,
                std::move(opCl),
                std::make_unique<ParameterVariability>(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356));
        dispositionParameters->addParameter(std::move(PCL));

        dispositionParameters->addCorrelation(std::make_unique<Correlation>("CL", "V", 0.798));
        analyteSet->setDispositionParameters(std::move(dispositionParameters));


        std::unique_ptr<TimeConsiderations> timeConsiderations = std::make_unique<TimeConsiderations>();

        // Arbitrary 20h half life. To be checked. Multiplier of 20
        // With a multiplier of 10 it fails
        std::unique_ptr<HalfLife> halfLife =
                std::make_unique<HalfLife>("halflife", 20, Tucuxi::Common::TucuUnit("h"), 20);
        timeConsiderations->setHalfLife(std::move(halfLife));
        model->setTimeConsiderations(std::move(timeConsiderations));




        {
            auto association = std::make_unique<AnalyteSetToAbsorptionAssociation>(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::Extravascular);

            auto absorptionParameters = std::make_unique<ParameterSetDefinition>();
            auto PKa = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None);
            absorptionParameters->addParameter(std::move(PKa));
            auto PF = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "F", 1, Tucuxi::Core::ParameterVariabilityType::None);
            absorptionParameters->addParameter(std::move(PF));

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "No details");
            auto formulationAndRoute = std::make_unique<FullFormulationAndRoute>(formulationSpecs, "extraId");
            formulationAndRoute->addAssociation(std::move(association));

            auto validDoses = std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            formulationAndRoute->setValidDoses(std::move(validDoses));

            auto fixedIntervals = std::make_unique<ValidValuesFixed>();
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            auto validIntervals =
                    std::make_unique<ValidDurations>(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::move(fixedIntervals));

            formulationAndRoute->setValidIntervals(std::move(validIntervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        {
            auto association = std::make_unique<AnalyteSetToAbsorptionAssociation>(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::Intravascular);

            auto absorptionParameters = std::make_unique<ParameterSetDefinition>();

            auto PKa = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None);
            absorptionParameters->addParameter(std::move(PKa));
            auto PF = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "F", 1, Tucuxi::Core::ParameterVariabilityType::None);
            absorptionParameters->addParameter(std::move(PF));

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(
                    Formulation::ParenteralSolution,
                    AdministrationRoute::IntravenousBolus,
                    AbsorptionModel::Intravascular,
                    "No details");
            auto formulationAndRoute = std::make_unique<FullFormulationAndRoute>(formulationSpecs, "intraId");
            formulationAndRoute->addAssociation(std::move(association));


            std::unique_ptr<ValidDoses> validDoses =
                    std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            formulationAndRoute->setValidDoses(std::move(validDoses));


            auto fixedIntervals = std::make_unique<ValidValuesFixed>();
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            auto validIntervals =
                    std::make_unique<ValidDurations>(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::move(fixedIntervals));
            formulationAndRoute->setValidIntervals(std::move(validIntervals));


            // This is just here as an example.
            auto fixedInfusions = std::make_unique<ValidValuesFixed>();
            fixedInfusions->addValue(0.5);
            fixedInfusions->addValue(1);

            auto validInfusionTimes =
                    std::make_unique<ValidDurations>(TucuUnit("h"), std::make_unique<PopulationValue>("", 1));
            validInfusionTimes->addValues(std::move(fixedInfusions));
            formulationAndRoute->setValidInfusionTimes(std::move(validInfusionTimes));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        {
            auto association = std::make_unique<AnalyteSetToAbsorptionAssociation>(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::Infusion);

            auto absorptionParameters = std::make_unique<ParameterSetDefinition>();

            auto PKa = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None);
            absorptionParameters->addParameter(std::move(PKa));
            auto PF = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "F", 1, Tucuxi::Core::ParameterVariabilityType::None);
            absorptionParameters->addParameter(std::move(PF));

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(
                    Formulation::ParenteralSolution,
                    AdministrationRoute::IntravenousDrip,
                    AbsorptionModel::Infusion,
                    "No details");
            auto formulationAndRoute = std::make_unique<FullFormulationAndRoute>(formulationSpecs, "infuId");
            formulationAndRoute->addAssociation(std::move(association));

            std::unique_ptr<ValidDoses> validDoses =
                    std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            formulationAndRoute->setValidDoses(std::move(validDoses));

            auto fixedIntervals = std::make_unique<ValidValuesFixed>();
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            ValidDurations* validIntervals =
                    new ValidDurations(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::move(fixedIntervals));
            formulationAndRoute->setValidIntervals(std::unique_ptr<ValidDurations>(validIntervals));


            // This is just here as an example.
            auto fixedInfusions = std::make_unique<ValidValuesFixed>();
            fixedInfusions->addValue(0.5);
            fixedInfusions->addValue(1);

            auto validInfusionTimes =
                    std::make_unique<ValidDurations>(TucuUnit("h"), std::make_unique<PopulationValue>("", 1));
            validInfusionTimes->addValues(std::move(fixedInfusions));
            formulationAndRoute->setValidInfusionTimes(std::move(validInfusionTimes));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        model->addAnalyteSet(std::move(analyteSet));
        return model;
    }
};

#endif // BUILDIMATINIB_H
