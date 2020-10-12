#ifndef BUILDIMATINIB_H
#define BUILDIMATINIB_H

#include "drugmodelbuilder.h"

#include "tucucore/drugmodel/drugmodel.h"

using namespace Tucuxi::Core;

class BuildImatinib
{
public:
    BuildImatinib() {}

    Tucuxi::Core::DrugModel *buildDrugModel()
    {
        Tucuxi::Core::DrugModel *model;
        model = new Tucuxi::Core::DrugModel();

        // The following constraint is for tests only. Needs to be modified according to the paper
        Tucuxi::Core::Operation *operationConstraint = new Tucuxi::Core::JSOperation(" \
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
                        new Tucuxi::Core::CovariateDefinition("bodyweight", "70", nullptr, CovariateType::Standard)));
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

        analyteSet->setId("imatinib");
        analyteSet->setPkModelId("linear.1comp.macro");

        ActiveSubstance *analyte = new ActiveSubstance();
        analyte->setAnalyteId("imatinib");

        ErrorModel* errorModel = new ErrorModel();

        errorModel->setErrorModel(ResidualErrorType::PROPORTIONAL);
        errorModel->addOriginalSigma(std::make_unique<PopulationValue>("sigma0", 0.3138));

        std::unique_ptr<ErrorModel> err(errorModel);

        analyte->setResidualErrorModel(std::move(err));

        // Add targets
        std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 750.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1500.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
        TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                        TucuUnit("ug/l"),
                                                        ActiveMoietyId("imatinib"),
                                                        std::move(cMin),
                                                        std::move(cMax),
                                                        std::move(cBest),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("mic", 0.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMin", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMax", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tBest", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("toxicity", 10000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("inefficacy", 000.0, nullptr)));

        ActiveMoiety *activeMoiety = new ActiveMoiety();

        activeMoiety->addTarget(std::unique_ptr<TargetDefinition>(target));
        model->addActiveMoiety(std::unique_ptr<ActiveMoiety>(activeMoiety));

        analyteSet->addAnalyte(std::unique_ptr<Analyte>(analyte));

        std::unique_ptr<ParameterSetDefinition> dispositionParameters(new ParameterSetDefinition());

        // Imatinib parameters, as in the XML drug file
        Operation *opV = new JSOperation("theta2=V_population; theta8=46.2; tvv = theta2+theta8*sex-theta8*(1-sex); V = tvv; return V;",
                                         { OperationInput("V_population", InputType::DOUBLE),
                                           OperationInput("sex", InputType::DOUBLE)});

        std::unique_ptr<ParameterDefinition> PV(new Tucuxi::Core::ParameterDefinition("V", 347, opV, std::make_unique<ParameterVariability>(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
        dispositionParameters->addParameter(std::move(PV));
        Operation *opCl = new JSOperation(" \
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
                                            { OperationInput("CL_population", InputType::DOUBLE),
                                              OperationInput("sex", InputType::DOUBLE),
                                              OperationInput("bodyweight", InputType::DOUBLE),
                                              OperationInput("age", InputType::DOUBLE),
                                              OperationInput("gist", InputType::BOOL)});

        std::unique_ptr<ParameterDefinition> PCL(new Tucuxi::Core::ParameterDefinition("CL", 14.3, opCl, std::make_unique<ParameterVariability>(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.356)));
        dispositionParameters->addParameter(std::move(PCL));

        dispositionParameters->addCorrelation(Correlation("CL", "V", 0.798));
        analyteSet->setDispositionParameters(std::move(dispositionParameters));


        std::unique_ptr<TimeConsiderations> timeConsiderations = std::make_unique<TimeConsiderations>();

        // Arbitrary 20h half life. To be checked. Multiplier of 20
        // With a multiplier of 10 it fails
        std::unique_ptr<HalfLife> halfLife = std::make_unique<HalfLife>("halflife", 20, Tucuxi::Common::TucuUnit("h"), 20);
        timeConsiderations->setHalfLife(std::move(halfLife));
        model->setTimeConsiderations(std::move(timeConsiderations));




        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::Extravascular);

            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());
            std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PKa));
            std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PF));

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "No details");
            std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(new FullFormulationAndRoute(formulationSpecs, "extraId"));
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

            ValidDoses *validDoses = new ValidDoses(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            std::unique_ptr<ValidDoses> doses(validDoses);

            formulationAndRoute->setValidDoses(std::move(doses));

            ValidValuesFixed *fixedIntervals = new ValidValuesFixed();
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            ValidDurations *validIntervals = new ValidDurations(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::unique_ptr<IValidValues>(fixedIntervals));


            std::unique_ptr<ValidDurations> intervals(validIntervals);
            formulationAndRoute->setValidIntervals(std::move(intervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::Intravascular);

            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

            std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PKa));
            std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PF));

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular, "No details");
            std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(new FullFormulationAndRoute(formulationSpecs, "intraId"));
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));


            std::unique_ptr<ValidDoses> validDoses = std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            formulationAndRoute->setValidDoses(std::move(validDoses));


            ValidValuesFixed *fixedIntervals = new ValidValuesFixed();
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            ValidDurations *validIntervals = new ValidDurations(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::unique_ptr<IValidValues>(fixedIntervals));
            formulationAndRoute->setValidIntervals(std::unique_ptr<ValidDurations>(validIntervals));


            // This is just here as an example.
            ValidValuesFixed *fixedInfusions = new ValidValuesFixed();
            fixedInfusions->addValue(0.5);
            fixedInfusions->addValue(1);

            ValidDurations *validInfusionTimes = new ValidDurations(TucuUnit("h"), std::make_unique<PopulationValue>("", 1));
            validInfusionTimes->addValues(std::unique_ptr<IValidValues>(fixedInfusions));
            formulationAndRoute->setValidInfusionTimes(std::unique_ptr<ValidDurations>(validInfusionTimes));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::Infusion);

            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

            std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PKa));
            std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PF));

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(Formulation::ParenteralSolution, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion, "No details");
            std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(new FullFormulationAndRoute(formulationSpecs, "infuId"));
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

            std::unique_ptr<ValidDoses> validDoses = std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(800));

            validDoses->addValues(std::move(specificDoses));

            formulationAndRoute->setValidDoses(std::move(validDoses));

            /*
            std::unique_ptr<SpecificDurations> validIntervals(new SpecificDurations(24h));
            validIntervals->addDuration(Duration(12h));
            validIntervals->addDuration(Duration(24h));
            formulationAndRoute->setValidIntervals(std::move(validIntervals));

            std::unique_ptr<SpecificDurations> validInfusions(new SpecificDurations(1h));
            validInfusions->addDuration(Duration(30min));
            validInfusions->addDuration(Duration(1h));
            formulationAndRoute->setValidInfusionTimes(std::move(validInfusions));
*/


            ValidValuesFixed *fixedIntervals = new ValidValuesFixed();
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            ValidDurations *validIntervals = new ValidDurations(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::unique_ptr<IValidValues>(fixedIntervals));
            formulationAndRoute->setValidIntervals(std::unique_ptr<ValidDurations>(validIntervals));


            // This is just here as an example.
            ValidValuesFixed *fixedInfusions = new ValidValuesFixed();
            fixedInfusions->addValue(0.5);
            fixedInfusions->addValue(1);

            ValidDurations *validInfusionTimes = new ValidDurations(TucuUnit("h"), std::make_unique<PopulationValue>("", 1));
            validInfusionTimes->addValues(std::unique_ptr<IValidValues>(fixedInfusions));
            formulationAndRoute->setValidInfusionTimes(std::unique_ptr<ValidDurations>(validInfusionTimes));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        model->addAnalyteSet(std::move(analyteSet));
        return model;
    }
};

#endif // BUILDIMATINIB_H
