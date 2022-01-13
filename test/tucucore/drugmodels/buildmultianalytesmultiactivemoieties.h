#ifndef BUILDMULTIANALYTESMULTIACTIVEMOIETIES_H
#define BUILDMULTIANALYTESMULTIACTIVEMOIETIES_H


#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/hardcodedoperation.h"

#include "drugmodelbuilder.h"

using namespace Tucuxi::Core;

class BuildMultiAnalytesMultiActiveMoieties
{
public:
    BuildMultiAnalytesMultiActiveMoieties() {}

    Tucuxi::Core::DrugModel* buildDrugModel(
            //the method buildDrugModel() takes a certain number of arguments, that were relevant for other drug models. Do not hesitate to add some (for instance we could have 4 times a conversionFactor instead of 2, ...).
            double conversionFactor0 = 1.0,
            double conversionFactor1 = 1.0,
            double conversionFactor2 = 1.0,
            double conversionFactor3 = 1.0,
            ResidualErrorType _errorModelType = ResidualErrorType::NONE,
            std::vector<Value> _sigmas = {0.0},
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeA = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeM = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeR = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeS = Tucuxi::Core::ParameterVariabilityType::None,
            Value _variabilityValueA0 = 0.0,
            Value _variabilityValueM0 = 1.0,
            Value _variabilityValueR0 = 0.0,
            Value _variabilityValueS0 = 0.0,
            Value _variabilityValueA1 = 0.0,
            Value _variabilityValueM1 = 1.0,
            Value _variabilityValueR1 = 0.0,
            Value _variabilityValueS1 = 0.0)
    {
        Tucuxi::Core::DrugModel* model;
        model = new Tucuxi::Core::DrugModel();

        model->setDrugId("drugTestMultiAnalytesMultiActiveMoieties");
        model->setDrugModelId("MultiAnalytesMultiActiveMoieties");

        std::unique_ptr<DrugModelMetadata> metaData = std::make_unique<DrugModelMetadata>();
        metaData->addAtc("fake Atc");
        Tucuxi::Common::TranslatableString drugName;
        drugName.setString("Multi analytes multi active moieties test");
        metaData->setDrugName(drugName);
        metaData->setAuthorName("The authors");
        model->setMetadata(std::move(metaData));



        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covS0", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covA0", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covR0", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covM0", "1.0", nullptr, CovariateType::Standard));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covS1", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covA1", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covR1", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covM1", "1.0", nullptr, CovariateType::Standard));

        // Build analyteSet0
        {
            std::unique_ptr<AnalyteSet> analyteSet0(new AnalyteSet());

            analyteSet0->setId("analyteSet0");
            analyteSet0->setPkModelId("test.multiconstantelimination");

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

            Operation* opS0 = new JSOperation(
                    "return covS0;",
                    {OperationInput("covS", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PS0(new Tucuxi::Core::ParameterDefinition(
                    "TestS0",
                    0.0,
                    opS0,
                    std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS0)));
            dispositionParameters->addParameter(std::move(PS0));
            Operation* opA0 = new JSOperation(
                    "return covA0;",
                    {OperationInput("covA", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PA0(new Tucuxi::Core::ParameterDefinition(
                    "TestA0",
                    0.0,
                    opA0,
                    std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA0)));
            dispositionParameters->addParameter(std::move(PA0));
            Operation* opR0 = new JSOperation(
                    "return covR0;",
                    {OperationInput("covR", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PR0(new Tucuxi::Core::ParameterDefinition(
                    "TestR0",
                    0.0,
                    opR0,
                    std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR0)));
            dispositionParameters->addParameter(std::move(PR0));
            Operation* opM0 = new JSOperation(
                    "return covM0;",
                    {OperationInput("covM", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PM0(new Tucuxi::Core::ParameterDefinition(
                    "TestM0",
                    1.0,
                    opM0,
                    std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM0)));
            dispositionParameters->addParameter(std::move(PM0));

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

            Operation* opS0 = new JSOperation(
                    " \
                                             return covS0;",
                    {OperationInput("covS", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PS0(new Tucuxi::Core::ParameterDefinition(
                    "TestS0",
                    0.0,
                    opS0,
                    std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS0)));
            dispositionParameters->addParameter(std::move(PS0));
            Operation* opA0 = new JSOperation(
                    " \
                                             return covA0;",
                    {OperationInput("covA", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PA0(new Tucuxi::Core::ParameterDefinition(
                    "TestA0",
                    0.0,
                    opA0,
                    std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA0)));
            dispositionParameters->addParameter(std::move(PA0));
            Operation* opR0 = new JSOperation(
                    " \
                                             return covR0;",
                    {OperationInput("covR", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PR0(new Tucuxi::Core::ParameterDefinition(
                    "TestR0",
                    0.0,
                    opR0,
                    std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR0)));
            dispositionParameters->addParameter(std::move(PR0));
            Operation* opM0 = new JSOperation(
                    " \
                                             return covM0;",
                    {OperationInput("covM", InputType::DOUBLE)});
            std::unique_ptr<ParameterDefinition> PM0(new Tucuxi::Core::ParameterDefinition(
                    "TestM0",
                    1.0,
                    opM0,
                    std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM0)));
            dispositionParameters->addParameter(std::move(PM0));

            analyteSet1->setDispositionParameters(std::move(dispositionParameters));



            model->addAnalyteSet(std::move(analyteSet1));
        }


        //Build AnalyteSet2

        std::unique_ptr<AnalyteSet> analyteSet2(new AnalyteSet());

        analyteSet2->setId("analyteSet2");
        analyteSet2->setPkModelId("test.multiconstantelimination");

        std::unique_ptr<Analyte> analyte2 = std::make_unique<Analyte>(
                "analyte2", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));
        std::unique_ptr<Analyte> analyte3 = std::make_unique<Analyte>(
                "analyte3", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));

        // TODO : Here we use the same error model for all analytes. That should be changed
        //        to allow different error models, thanks to the function parameters
        auto errorModel2 = std::make_unique<ErrorModel>();
        for (size_t i = 0; i < _sigmas.size(); i++) {
            std::string sigmaName = "sigma" + std::to_string(i);
            errorModel2->addOriginalSigma(std::make_unique<PopulationValue>(sigmaName, _sigmas[i]));
        }
        analyte2->setResidualErrorModel(std::move(errorModel2));

        auto errorModel3 = std::make_unique<ErrorModel>();
        errorModel3->setErrorModel(_errorModelType);
        for (size_t i = 0; i < _sigmas.size(); i++) {
            std::string sigmaName = "sigma" + std::to_string(i);
            errorModel3->addOriginalSigma(std::make_unique<PopulationValue>(sigmaName, _sigmas[i]));
        }
        analyte3->setResidualErrorModel(std::move(errorModel3));

        analyteSet2->addAnalyte(std::move(analyte2));
        analyteSet2->addAnalyte(std::move(analyte3));

        std::unique_ptr<ParameterSetDefinition> dispositionParameters(new ParameterSetDefinition());

        Operation* opS0 = new JSOperation(
                " \
                                         return covS0;",
                {OperationInput("covS0", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PS0(new Tucuxi::Core::ParameterDefinition(
                "TestS0", 0.0, opS0, std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS0)));
        dispositionParameters->addParameter(std::move(PS0));
        Operation* opA0 = new JSOperation(
                " \
                                         return covA0;",
                {OperationInput("covA0", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PA0(new Tucuxi::Core::ParameterDefinition(
                "TestA0", 0.0, opA0, std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA0)));
        dispositionParameters->addParameter(std::move(PA0));
        Operation* opR0 = new JSOperation(
                " \
                                         return covR0;",
                {OperationInput("covR0", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PR0(new Tucuxi::Core::ParameterDefinition(
                "TestR0", 0.0, opR0, std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR0)));
        dispositionParameters->addParameter(std::move(PR0));
        Operation* opM0 = new JSOperation(
                " \
                                         return covM0;",
                {OperationInput("covM0", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PM0(new Tucuxi::Core::ParameterDefinition(
                "TestM0", 1.0, opM0, std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM0)));
        dispositionParameters->addParameter(std::move(PM0));
        Operation* opS1 = new JSOperation(
                " \
                                         return covS1;",

                {OperationInput("covS1", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PS1(new Tucuxi::Core::ParameterDefinition(
                "TestS0", 0.0, opS1, std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS1)));
        dispositionParameters->addParameter(std::move(PS1));
        Operation* opA1 = new JSOperation(
                " \
                                         return covA1;",
                {OperationInput("covA1", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PA1(new Tucuxi::Core::ParameterDefinition(
                "TestA0", 0.0, opA1, std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA1)));
        dispositionParameters->addParameter(std::move(PA1));
        Operation* opR1 = new JSOperation(
                " \
                                         return covR1;",
                {OperationInput("covR1", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PR1(new Tucuxi::Core::ParameterDefinition(
                "TestR0", 0.0, opR1, std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR1)));
        dispositionParameters->addParameter(std::move(PR1));
        Operation* opM1 = new JSOperation(
                " \
                                         return covM1;",
                {OperationInput("covM1", InputType::DOUBLE)});
        std::unique_ptr<ParameterDefinition> PM1(new Tucuxi::Core::ParameterDefinition(
                "TestM0", 1.0, opM1, std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM1)));
        dispositionParameters->addParameter(std::move(PM1));

        analyteSet2->setDispositionParameters(std::move(dispositionParameters));



        model->addAnalyteSet(std::move(analyteSet2));





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
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte0"), conversionFactor0);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion0));
            std::unique_ptr<AnalyteConversion> analyteConversion1 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte1"), conversionFactor1);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion1));
            std::unique_ptr<AnalyteConversion> analyteConversion2 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte2"), conversionFactor2);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion2));
            std::unique_ptr<AnalyteConversion> analyteConversion3 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte3"), conversionFactor3);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion3));

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

        std::unique_ptr<Operation> activeMoietyOperation0 = std::unique_ptr<Operation>(sharedOperation->clone());
        std::unique_ptr<Operation> activeMoietyOperation1 = std::unique_ptr<Operation>(sharedOperation->clone());

        std::vector<AnalyteId> analyteList0;
        analyteList0.push_back(AnalyteId("analyte0"));
        analyteList0.push_back(AnalyteId("analyte1"));

        std::unique_ptr<ActiveMoiety> activeMoiety0 = std::make_unique<ActiveMoiety>(
                ActiveMoietyId("activeMoietyMulti"), TucuUnit("ug/l"), analyteList0, std::move(activeMoietyOperation0));

        std::vector<AnalyteId> analyteList1;
        analyteList1.push_back(AnalyteId("analyte2"));
        analyteList1.push_back(AnalyteId("analyte3"));
        std::unique_ptr<ActiveMoiety> activeMoiety1 = std::make_unique<ActiveMoiety>(
                ActiveMoietyId("activeMoietyMulti"), TucuUnit("ug/l"), analyteList1, std::move(activeMoietyOperation1));

        Tucuxi::Common::TranslatableString activeMoietyName0;
        activeMoietyName0.setString("Active moiety name 2");
        activeMoiety0->setName(activeMoietyName0);

        Tucuxi::Common::TranslatableString activeMoietyName1;
        activeMoietyName1.setString("Active moiety with a group of 2 analytes");
        activeMoiety1->setName(activeMoietyName1);

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

        model->addActiveMoiety(std::move(activeMoiety0));
        model->addActiveMoiety(std::move(activeMoiety1));

        return model;
    }
};


#endif // BUILDMULTIANALYTESMULTIACTIVEMOIETIES_H
