#ifndef BUILDIMATINIB_H
#define BUILDIMATINIB_H

#include "drugmodelbuilder.h"

#include "tucucore/drugmodel/drugmodel.h"


using namespace Tucuxi::Core;

class BuildImatinib
{
public:
    BuildImatinib() {}

    DrugModel *buildDrugModel()
    {
        DrugModel *model;
        model = new DrugModel();

        // The following constraint is for tests only. Needs to be modified according to the paper
        Operation *operationConstraint = new JSOperation(" \
                                                return (age > 0);",
        { OperationInput("age", InputType::DOUBLE)});

        Constraint *constraint = new Constraint();
        constraint->addRequiredCovariateId("age");
        constraint->setCheckOperation(std::unique_ptr<Operation>(operationConstraint));
        constraint->setType(ConstraintType::HARD);

        std::unique_ptr<DrugModelDomain> drugDomain(new DrugModelDomain(std::unique_ptr<Constraint>(constraint)));

        model->setDomain(std::move(drugDomain));

        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("weight", "70", nullptr, CovariateType::Standard)));
        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("gist", "0", nullptr, CovariateType::Standard)));
        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("sex", "0.5", nullptr, CovariateType::Standard)));
        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("age", "50", nullptr, CovariateType::Standard)));

        std::unique_ptr<AnalyteSet> analyteSet(new AnalyteSet());

        analyteSet->setPkModelId("linear.1comp.macro");

        ActiveSubstance *analyte = new ActiveSubstance();
        analyte->setAnalyteId("imatinib");

        SigmaResidualErrorModel* errorModel = new SigmaResidualErrorModel();
        //        std::unique_ptr<SigmaResidualErrorModel> errorModel(new SigmaResidualErrorModel());
        Sigma sigma(1);
        sigma(0) = 0.3138;
        errorModel->setErrorModel(SigmaResidualErrorModel::ResidualErrorType::PROPORTIONAL);
        errorModel->setSigma(sigma);

        std::unique_ptr<IResidualErrorModel> err(errorModel);

        analyte->setResidualErrorModel(err);

        // Add targets
        std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 750.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1500.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
        TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                        "imatinib",
                                                        std::move(cMin),
                                                        std::move(cMax),
                                                        std::move(cBest),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("cBest", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("cBest", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("cBest", 1000.0, nullptr)));

        ActiveMoiety *activeMoiety = new ActiveMoiety();

        activeMoiety->addTarget(std::unique_ptr<TargetDefinition>(target));
        model->addActiveMoiety(std::unique_ptr<ActiveMoiety>(activeMoiety));

        analyteSet->addAnalyte(std::unique_ptr<Analyte>(analyte));

        std::unique_ptr<ParameterSetDefinition> dispositionParameters(new ParameterSetDefinition());

        // Imatinib parameters, as in the XML drug file
        Operation *opV = new JSOperation("theta2=V_population; theta8=46.2; tvv = theta2+theta8*sex-theta8*(1-sex); V = tvv;",
                                         { OperationInput("V_population", InputType::DOUBLE),
                                           OperationInput("sex", InputType::DOUBLE)});

        std::unique_ptr<ParameterDefinition> PV(new Tucuxi::Core::ParameterDefinition("V", 347, opV, Tucuxi::Core::ParameterVariability(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
        dispositionParameters->addParameter(PV);
        Operation *opCl = new JSOperation(" \
                                            theta1=Cl_population; \
                                            theta4=5.42; \
                                            theta5=1.49; \
                                            theta6=-5.81; \
                                            theta7=-0.806; \
                                            \
                                            MEANBW=70; \
                                            FBW=(weight-MEANBW)/MEANBW; \
                                            \
                                            MEANAG=50; \
                                            FAGE=(age-MEANAG)/MEANAG; \
                                            \
                                            PATH = gist; \
                                            MALE = sex; \
                                            \
                                            Cl = theta1+theta4*FBW+theta5*MALE-theta5*(1-MALE)+theta6*FAGE + theta7*PATH-theta7*(1-PATH);",
                                            { OperationInput("Cl_population", InputType::DOUBLE),
                                              OperationInput("sex", InputType::DOUBLE),
                                              OperationInput("weight", InputType::DOUBLE),
                                              OperationInput("age", InputType::DOUBLE),
                                              OperationInput("gist", InputType::BOOL)});

        std::unique_ptr<ParameterDefinition> PCL(new Tucuxi::Core::ParameterDefinition("Cl", 15.106, opCl, Tucuxi::Core::ParameterVariability(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
        dispositionParameters->addParameter(PCL);

        dispositionParameters->addCorrelation(Correlation("Cl", "V", 0.798));
        analyteSet->setDispositionParameters(std::move(dispositionParameters));

        AnalyteSetToAbsorptionAssociation *association;
        association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
        association->setAbsorptionModel(AbsorptionModel::EXTRAVASCULAR);

        std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());
        std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
        absorptionParameters->addParameter(PKa);
        std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
        absorptionParameters->addParameter(PF);

        association->setAbsorptionParameters(std::move(absorptionParameters));
        FormulationAndRoute formulationSpecs(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::EXTRAVASCULAR, "No details");
        std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(new FullFormulationAndRoute(formulationSpecs, "extraId"));
        formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

        ValidDoses *validDoses = new ValidDoses(Unit("mg"), std::make_unique<PopulationValue>(400));
        std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
        specificDoses->addValue(DoseValue(100));
        specificDoses->addValue(DoseValue(200));
        specificDoses->addValue(DoseValue(300));
        specificDoses->addValue(DoseValue(400));
        specificDoses->addValue(DoseValue(600));
        specificDoses->addValue(DoseValue(800));

        validDoses->addValues(std::move(specificDoses));



        {
            std::unique_ptr<ValidDoses> doses(validDoses);

            formulationAndRoute->setValidDoses(std::move(doses));

            SpecificDurations* validIntervals = new SpecificDurations(24h);
            validIntervals->addDuration(Duration(12h));
            validIntervals->addDuration(Duration(24h));

            std::unique_ptr<ValidDurations> intervals(validIntervals);
            formulationAndRoute->setValidIntervals(std::move(intervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::INTRAVASCULAR);

            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

            std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(PKa);
            std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(PF);

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::INTRAVASCULAR, "No details");
            std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(new FullFormulationAndRoute(formulationSpecs, "intraId"));
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));


            std::unique_ptr<ValidDoses> validDoses = std::make_unique<ValidDoses>(Unit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            formulationAndRoute->setValidDoses(std::move(validDoses));

            std::unique_ptr<SpecificDurations> validIntervals(new SpecificDurations(24h));
            validIntervals->addDuration(Duration(12h));
            validIntervals->addDuration(Duration(24h));
            formulationAndRoute->setValidIntervals(std::move(validIntervals));

            // This is just here as an example.
            std::unique_ptr<SpecificDurations> validInfusionTimes(new SpecificDurations(1h));
            validInfusionTimes->addDuration(Duration(30min));
            validInfusionTimes->addDuration(Duration(1h));
            formulationAndRoute->setValidInfusionTimes(std::move(validInfusionTimes));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::INFUSION);

            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

            std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(PKa);
            std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(PF);

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(Formulation::ParenteralSolution, AdministrationRoute::IntravenousDrip, AbsorptionModel::INFUSION, "No details");
            std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(new FullFormulationAndRoute(formulationSpecs, "infuId"));
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

            std::unique_ptr<ValidDoses> validDoses = std::make_unique<ValidDoses>(Unit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            formulationAndRoute->setValidDoses(std::move(validDoses));

            std::unique_ptr<SpecificDurations> validIntervals(new SpecificDurations(24h));
            validIntervals->addDuration(Duration(12h));
            validIntervals->addDuration(Duration(24h));
            formulationAndRoute->setValidIntervals(std::move(validIntervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        model->setAnalyteSet(std::move(analyteSet));
        return model;
    }
};

#endif // BUILDIMATINIB_H
