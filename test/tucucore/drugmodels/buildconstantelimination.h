#ifndef BUILDCONSTANTELIMINATION_H
#define BUILDCONSTANTELIMINATION_H


#include "drugmodelbuilder.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/hardcodedoperation.h"

using namespace Tucuxi::Core;

class BuildConstantElimination
{
public:
    BuildConstantElimination() {}

    Tucuxi::Core::DrugModel *buildDrugModel()
    {
        Tucuxi::Core::DrugModel *model;
        model = new Tucuxi::Core::DrugModel();

        model->setDrugId("drugTest");
        model->setDrugModelId("constantEliminationBolus1");

        std::unique_ptr<AnalyteSet> analyteSet(new AnalyteSet());

        analyteSet->setId("analyteSet");
        analyteSet->setPkModelId("test.constantelimination");

        std::unique_ptr<ActiveSubstance> analyte = std::make_unique<ActiveSubstance>();
        analyte->setAnalyteId("analyte");

        ErrorModel* errorModel = new ErrorModel();

        errorModel->setErrorModel(ResidualErrorType::PROPORTIONAL);
        errorModel->addOriginalSigma(std::make_unique<PopulationValue>("sigma0", 0.3138));

        std::unique_ptr<ErrorModel> err(errorModel);

        analyte->setResidualErrorModel(std::move(err));
        analyteSet->addAnalyte(std::move(analyte));

        std::unique_ptr<ParameterSetDefinition> dispositionParameters(new ParameterSetDefinition());

        std::unique_ptr<ParameterDefinition> PV(new Tucuxi::Core::ParameterDefinition("V", 347, nullptr, std::make_unique<ParameterVariability>(Tucuxi::Core::ParameterVariabilityType::Proportional, 0.629)));
        dispositionParameters->addParameter(std::move(PV));

        analyteSet->setDispositionParameters(std::move(dispositionParameters));



        model->addAnalyteSet(std::move(analyteSet));

        std::unique_ptr<DrugModelDomain> drugDomain(new DrugModelDomain());

        model->setDomain(std::move(drugDomain));

        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::EXTRAVASCULAR);

            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());
            std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PKa));
            std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(std::move(PF));

            association->setAbsorptionParameters(std::move(absorptionParameters));
            FormulationAndRoute formulationSpecs(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::EXTRAVASCULAR, "No details");
            std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(new FullFormulationAndRoute(formulationSpecs, "extraId"));
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

            std::unique_ptr<AnalyteConversion> analyteConversion = std::make_unique<AnalyteConversion>("analyteId", 1.0);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion));

            ValidDoses *validDoses = new ValidDoses(Unit("mg"), std::make_unique<PopulationValue>(400));
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

            ValidDurations *validIntervals = new ValidDurations(Unit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::unique_ptr<IValidValues>(fixedIntervals));


            std::unique_ptr<ValidDurations> intervals(validIntervals);
            formulationAndRoute->setValidIntervals(std::move(intervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        std::unique_ptr<TimeConsiderations> timeConsiderations = std::make_unique<TimeConsiderations>();

        // Arbitrary 20h half life. To be checked. Multiplier of 20
        // With a multiplier of 10 it fails
        std::unique_ptr<HalfLife> halfLife = std::make_unique<HalfLife>("halflife", 20, Tucuxi::Core::Unit("h"), 20);
        timeConsiderations->setHalfLife(std::move(halfLife));
        std::unique_ptr<OutdatedMeasure> outdatedMeasure = std::make_unique<OutdatedMeasure>("id", 0.0, Unit());
        timeConsiderations->setOutdatedMeasure(std::move(outdatedMeasure));
        model->setTimeConsiderations(std::move(timeConsiderations));

        Tucuxi::Core::OperationCollection collection;
        collection.populate();
        std::shared_ptr<Operation> sharedOperation = collection.getOperationFromId("direct");

        std::unique_ptr<Operation> activeMoietyOperation = std::unique_ptr<Operation>(sharedOperation.get()->clone());

        std::vector<std::string> analyteList;
        analyteList.push_back("analyte");
        std::unique_ptr<ActiveMoiety> activeMoiety = std::make_unique<ActiveMoiety>("activeMoiety", Unit("mg/l"), analyteList, std::move(activeMoietyOperation));


        // Add targets
        std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 750.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1500.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
        TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                        Unit("ug/l"),
                                                        "imatinib",
                                                        std::move(cMin),
                                                        std::move(cMax),
                                                        std::move(cBest),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("mic", 0.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMin", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tMax", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("tBest", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("toxicity", 10000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("inefficacy", 000.0, nullptr)));


        activeMoiety->addTarget(std::unique_ptr<TargetDefinition>(target));


        model->addActiveMoiety(std::move(activeMoiety));

        return model;
    }
};

#endif // BUILDCONSTANTELIMINATION_H
