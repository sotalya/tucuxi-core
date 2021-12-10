#ifndef BUILDMULTIANALYTESSINGLAACTIVEMOIETY_H
#define BUILDMULTIANALYTESSINGLAACTIVEMOIETY_H


#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/hardcodedoperation.h"

#include "drugmodelbuilder.h"

using namespace Tucuxi::Core;

class BuildMultiAnalytesSingleActiveMoiety
{
public:
    BuildMultiAnalytesSingleActiveMoiety() {}

    Tucuxi::Core::DrugModel* buildDrugModel(
            double _conversionFactor0 = 1.0,
            double _conversionFactor1 = 1.0,
            ResidualErrorType _errorModelType = ResidualErrorType::NONE,
            std::vector<Value> _sigmas = {0.0},
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeA = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeM = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeR = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeS = Tucuxi::Core::ParameterVariabilityType::None,
            Value _variabilityValueA = 0.0,
            Value _variabilityValueM = 1.0,
            Value _variabilityValueR = 0.0,
            Value _variabilityValueS = 0.0)
    {
        Tucuxi::Core::DrugModel* model;
        model = new Tucuxi::Core::DrugModel();

        model->setDrugId("drugTestMultiAnalytesSingleActiveMoiety");
        model->setDrugModelId("MultiAnalytesSingleActiveMoiety");

        std::unique_ptr<DrugModelMetadata> metaData = std::make_unique<DrugModelMetadata>();
        metaData->addAtc("fake Atc");
        Tucuxi::Common::TranslatableString drugName;
        drugName.setString("Multi analytes single active moiety test");
        metaData->setDrugName(drugName);
        metaData->setAuthorName("The authors");
        model->setMetadata(std::move(metaData));



        model->addCovariate(std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                new Tucuxi::Core::CovariateDefinition("covS", "0.0", nullptr, CovariateType::Standard)));

        model->addCovariate(std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                new Tucuxi::Core::CovariateDefinition("covA", "0.0", nullptr, CovariateType::Standard)));

        model->addCovariate(std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                new Tucuxi::Core::CovariateDefinition("covR", "0.0", nullptr, CovariateType::Standard)));

        model->addCovariate(std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                new Tucuxi::Core::CovariateDefinition("covM", "1.0", nullptr, CovariateType::Standard)));

        // Build analyteSet0
        {
            std::unique_ptr<AnalyteSet> analyteSet0(new AnalyteSet());

            analyteSet0->setId("analyteSet0");
            analyteSet0->setPkModelId("test.constantelimination");

            std::unique_ptr<Analyte> analyte0 = std::make_unique<Analyte>(
                    "analyte0", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));



            ErrorModel* errorModel = new ErrorModel();

            errorModel->setErrorModel(_errorModelType);
            for (size_t i = 0; i < _sigmas.size(); i++) {
                std::string sigmaName = "sigma" + std::to_string(i);
                errorModel->addOriginalSigma(std::make_unique<PopulationValue>(sigmaName, _sigmas[i]));
            }

            std::unique_ptr<ErrorModel> err(errorModel);

            analyte0->setResidualErrorModel(std::move(err));
            analyteSet0->addAnalyte(std::move(analyte0));

            std::unique_ptr<ParameterSetDefinition> dispositionParameters(new ParameterSetDefinition());

            Operation* opS = new JSOperation(
                    " \
                                             return covS;",
                    {OperationInput("covS", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PS(new Tucuxi::Core::ParameterDefinition(
                    "TestS", 0.0, opS, std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS)));
            dispositionParameters->addParameter(std::move(PS));
            Operation* opA = new JSOperation(
                    " \
                                             return covA;",
                    {OperationInput("covA", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PA(new Tucuxi::Core::ParameterDefinition(
                    "TestA", 0.0, opA, std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA)));
            dispositionParameters->addParameter(std::move(PA));
            Operation* opR = new JSOperation(
                    " \
                                             return covR;",
                    {OperationInput("covR", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PR(new Tucuxi::Core::ParameterDefinition(
                    "TestR", 0.0, opR, std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR)));
            dispositionParameters->addParameter(std::move(PR));
            Operation* opM = new JSOperation(
                    " \
                                             return covM;",
                    {OperationInput("covM", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PM(new Tucuxi::Core::ParameterDefinition(
                    "TestM", 1.0, opM, std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM)));
            dispositionParameters->addParameter(std::move(PM));

            analyteSet0->setDispositionParameters(std::move(dispositionParameters));



            model->addAnalyteSet(std::move(analyteSet0));
        }

        // Build analyteSet1
        {
            std::unique_ptr<AnalyteSet> analyteSet1(new AnalyteSet());

            analyteSet1->setId("analyteSet1");
            analyteSet1->setPkModelId("test.constantelimination");

            std::unique_ptr<Analyte> analyte1 = std::make_unique<Analyte>(
                    "analyte1", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));



            ErrorModel* errorModel = new ErrorModel();

            errorModel->setErrorModel(_errorModelType);
            for (size_t i = 0; i < _sigmas.size(); i++) {
                std::string sigmaName = "sigma" + std::to_string(i);
                errorModel->addOriginalSigma(std::make_unique<PopulationValue>(sigmaName, _sigmas[i]));
            }

            std::unique_ptr<ErrorModel> err(errorModel);

            analyte1->setResidualErrorModel(std::move(err));
            analyteSet1->addAnalyte(std::move(analyte1));

            std::unique_ptr<ParameterSetDefinition> dispositionParameters(new ParameterSetDefinition());

            Operation* opS = new JSOperation(
                    " \
                                             return covS;",
                    {OperationInput("covS", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PS(new Tucuxi::Core::ParameterDefinition(
                    "TestS", 0.0, opS, std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS)));
            dispositionParameters->addParameter(std::move(PS));
            Operation* opA = new JSOperation(
                    " \
                                             return covA;",
                    {OperationInput("covA", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PA(new Tucuxi::Core::ParameterDefinition(
                    "TestA", 0.0, opA, std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA)));
            dispositionParameters->addParameter(std::move(PA));
            Operation* opR = new JSOperation(
                    " \
                                             return covR;",
                    {OperationInput("covR", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PR(new Tucuxi::Core::ParameterDefinition(
                    "TestR", 0.0, opR, std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR)));
            dispositionParameters->addParameter(std::move(PR));
            Operation* opM = new JSOperation(
                    " \
                                             return covM;",
                    {OperationInput("covM", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PM(new Tucuxi::Core::ParameterDefinition(
                    "TestM", 1.0, opM, std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM)));
            dispositionParameters->addParameter(std::move(PM));

            analyteSet1->setDispositionParameters(std::move(dispositionParameters));



            model->addAnalyteSet(std::move(analyteSet1));
        }

        std::unique_ptr<DrugModelDomain> drugDomain(new DrugModelDomain());

        model->setDomain(std::move(drugDomain));

        {
            FormulationAndRoute formulationSpecs(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "No details");
            std::unique_ptr<FullFormulationAndRoute> formulationAndRoute(
                    new FullFormulationAndRoute(formulationSpecs, "extraId"));
            {
                AnalyteSetToAbsorptionAssociation* association0;
                const AnalyteSet* a = model->getAnalyteSets()[0].get();
                association0 = new AnalyteSetToAbsorptionAssociation(*a);
                association0->setAbsorptionModel(AbsorptionModel::Extravascular);

                std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

                association0->setAbsorptionParameters(std::move(absorptionParameters));
                formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association0));
            }

            {
                AnalyteSetToAbsorptionAssociation* association1;
                const AnalyteSet* a = model->getAnalyteSets()[0].get();
                association1 = new AnalyteSetToAbsorptionAssociation(*a);
                association1->setAbsorptionModel(AbsorptionModel::Extravascular);

                std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

                association1->setAbsorptionParameters(std::move(absorptionParameters));
                formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association1));
            }

            std::unique_ptr<AnalyteConversion> analyteConversion0 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte0"), _conversionFactor0);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion0));
            std::unique_ptr<AnalyteConversion> analyteConversion1 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte1"), _conversionFactor1);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion1));

            ValidDoses* validDoses = new ValidDoses(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue(100));
            specificDoses->addValue(DoseValue(200));
            specificDoses->addValue(DoseValue(300));
            specificDoses->addValue(DoseValue(400));
            specificDoses->addValue(DoseValue(500));
            specificDoses->addValue(DoseValue(600));
            specificDoses->addValue(DoseValue(700));
            specificDoses->addValue(DoseValue(800));
            specificDoses->addValue(DoseValue(900));
            specificDoses->addValue(DoseValue(1000));

            validDoses->addValues(std::move(specificDoses));

            std::unique_ptr<ValidDoses> doses(validDoses);

            formulationAndRoute->setValidDoses(std::move(doses));

            ValidValuesFixed* fixedIntervals = new ValidValuesFixed();
            fixedIntervals->addValue(6);
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            ValidDurations* validIntervals =
                    new ValidDurations(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::unique_ptr<IValidValues>(fixedIntervals));


            std::unique_ptr<ValidDurations> intervals(validIntervals);
            formulationAndRoute->setValidIntervals(std::move(intervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        std::unique_ptr<TimeConsiderations> timeConsiderations = std::make_unique<TimeConsiderations>();

        // Arbitrary 20h half life. To be checked. Multiplier of 20
        // With a multiplier of 10 it fails
        std::unique_ptr<HalfLife> halfLife =
                std::make_unique<HalfLife>("halflife", 20, Tucuxi::Common::TucuUnit("h"), 20);
        timeConsiderations->setHalfLife(std::move(halfLife));
        std::unique_ptr<OutdatedMeasure> outdatedMeasure = std::make_unique<OutdatedMeasure>("id", 0.0, TucuUnit());
        timeConsiderations->setOutdatedMeasure(std::move(outdatedMeasure));
        model->setTimeConsiderations(std::move(timeConsiderations));

        Tucuxi::Core::OperationCollection collection;
        collection.populate();
        std::shared_ptr<Operation> sharedOperation = collection.getOperationFromId("sum2");

        std::unique_ptr<Operation> activeMoietyOperation = std::unique_ptr<Operation>(sharedOperation->clone());

        std::vector<AnalyteId> analyteList;
        analyteList.push_back(AnalyteId("analyte0"));
        analyteList.push_back(AnalyteId("analyte1"));
        std::unique_ptr<ActiveMoiety> activeMoiety = std::make_unique<ActiveMoiety>(
                ActiveMoietyId("activeMoietyMulti"), TucuUnit("ug/l"), analyteList, std::move(activeMoietyOperation));

        Tucuxi::Common::TranslatableString activeMoietyName;
        activeMoietyName.setString("Active moiety name 2");
        activeMoiety->setName(activeMoietyName);

        // I removed the targets from the build, to let tests define various targets
        /*
        // Add targets
        std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 750.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1500.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
        TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                        Unit("mg/l"),
                                                        "analyte",
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
*/

        model->addActiveMoiety(std::move(activeMoiety));

        return model;
    }
};


#endif // BUILDMULTIANALYTESSINGLAACTIVEMOIETY_H
