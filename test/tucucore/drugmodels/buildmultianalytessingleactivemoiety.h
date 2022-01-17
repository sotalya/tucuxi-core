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

    std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel(
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
        auto model = std::make_unique<Tucuxi::Core::DrugModel>();

        model->setDrugId("drugTestMultiAnalytesSingleActiveMoiety");
        model->setDrugModelId("MultiAnalytesSingleActiveMoiety");

        auto metaData = std::make_unique<DrugModelMetadata>();
        metaData->addAtc("fake Atc");
        Tucuxi::Common::TranslatableString drugName;
        drugName.setString("Multi analytes single active moiety test");
        metaData->setDrugName(drugName);
        metaData->setAuthorName("The authors");
        model->setMetadata(std::move(metaData));



        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covS", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covA", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covR", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covM", "1.0", nullptr, CovariateType::Standard));

        // Build analyteSet0
        {
            auto analyteSet0 = std::make_unique<AnalyteSet>();

            analyteSet0->setId("analyteSet0");
            analyteSet0->setPkModelId("test.constantelimination");

            auto analyte0 = std::make_unique<Analyte>(
                    "analyte0", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));

            auto errorModel = std::make_unique<ErrorModel>();

            errorModel->setErrorModel(_errorModelType);
            for (size_t i = 0; i < _sigmas.size(); i++) {
                std::string sigmaName = "sigma" + std::to_string(i);
                errorModel->addOriginalSigma(std::make_unique<PopulationValue>(sigmaName, _sigmas[i]));
            }

            analyte0->setResidualErrorModel(std::move(errorModel));
            analyteSet0->addAnalyte(std::move(analyte0));

            auto dispositionParameters = std::make_unique<ParameterSetDefinition>();

            auto opS = std::make_unique<JSOperation>("return covS;", OperationInputList{OperationInput("covS", InputType::DOUBLE)});
            auto PS = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestS", 0.0, std::move(opS), std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS));
            dispositionParameters->addParameter(std::move(PS));
            auto opA = std::make_unique<JSOperation>("return covA;", OperationInputList{OperationInput("covA", InputType::DOUBLE)});
            auto PA = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestA", 0.0, std::move(opA), std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA));
            dispositionParameters->addParameter(std::move(PA));
            auto opR = std::make_unique<JSOperation>("return covR;", OperationInputList{OperationInput("covR", InputType::DOUBLE)});
            auto PR = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestR", 0.0, std::move(opR), std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR));
            dispositionParameters->addParameter(std::move(PR));
            auto opM = std::make_unique<JSOperation>("return covM;", OperationInputList{OperationInput("covM", InputType::DOUBLE)});
            auto PM = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestM", 1.0, std::move(opM), std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM));
            dispositionParameters->addParameter(std::move(PM));

            analyteSet0->setDispositionParameters(std::move(dispositionParameters));

            model->addAnalyteSet(std::move(analyteSet0));
        }

        // Build analyteSet1
        {
            auto analyteSet1 = std::make_unique<AnalyteSet>();

            analyteSet1->setId("analyteSet1");
            analyteSet1->setPkModelId("test.constantelimination");

            auto analyte1 = std::make_unique<Analyte>(
                    "analyte1", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));

            auto errorModel = std::make_unique<ErrorModel>();

            errorModel->setErrorModel(_errorModelType);
            for (size_t i = 0; i < _sigmas.size(); i++) {
                std::string sigmaName = "sigma" + std::to_string(i);
                errorModel->addOriginalSigma(std::make_unique<PopulationValue>(sigmaName, _sigmas[i]));
            }

            analyte1->setResidualErrorModel(std::move(errorModel));
            analyteSet1->addAnalyte(std::move(analyte1));

            auto dispositionParameters = std::make_unique<ParameterSetDefinition>();

            auto opS = std::make_unique<JSOperation>("return covS;", OperationInputList{OperationInput("covS", InputType::DOUBLE)});
            auto PS = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestS", 0.0, std::move(opS), std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS));
            dispositionParameters->addParameter(std::move(PS));
            auto opA = std::make_unique<JSOperation>("return covA;", OperationInputList{OperationInput("covA", InputType::DOUBLE)});
            auto PA = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestA", 0.0, std::move(opA), std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA));
            dispositionParameters->addParameter(std::move(PA));
            auto opR = std::make_unique<JSOperation>("return covR;", OperationInputList{OperationInput("covR", InputType::DOUBLE)});
            auto PR = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestR", 0.0, std::move(opR), std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR));
            dispositionParameters->addParameter(std::move(PR));
            auto opM = std::make_unique<JSOperation>("return covM;", OperationInputList{OperationInput("covM", InputType::DOUBLE)});
            auto PM = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                    "TestM", 1.0, std::move(opM), std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM));
            dispositionParameters->addParameter(std::move(PM));

            analyteSet1->setDispositionParameters(std::move(dispositionParameters));

            model->addAnalyteSet(std::move(analyteSet1));
        }

        auto drugDomain = std::make_unique<DrugModelDomain>();

        model->setDomain(std::move(drugDomain));

        {
            FormulationAndRoute formulationSpecs(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "No details");
            auto formulationAndRoute = std::make_unique<FullFormulationAndRoute>(formulationSpecs, "extraId");
            {
                const AnalyteSet* a = model->getAnalyteSets()[0].get();
                auto association0 = std::make_unique<AnalyteSetToAbsorptionAssociation>(*a);
                association0->setAbsorptionModel(AbsorptionModel::Extravascular);

                auto absorptionParameters = std::make_unique<ParameterSetDefinition>();

                association0->setAbsorptionParameters(std::move(absorptionParameters));
                formulationAndRoute->addAssociation(std::move(association0));
            }

            {
                const AnalyteSet* a = model->getAnalyteSets()[0].get();
                auto association1 = std::make_unique<AnalyteSetToAbsorptionAssociation>(*a);
                association1->setAbsorptionModel(AbsorptionModel::Extravascular);

                auto absorptionParameters = std::make_unique<ParameterSetDefinition>();

                association1->setAbsorptionParameters(std::move(absorptionParameters));
                formulationAndRoute->addAssociation(std::move(association1));
            }

            auto analyteConversion0 = std::make_unique<AnalyteConversion>(AnalyteId("analyte0"), _conversionFactor0);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion0));
            auto analyteConversion1 = std::make_unique<AnalyteConversion>(AnalyteId("analyte1"), _conversionFactor1);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion1));

            auto validDoses = std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            auto specificDoses = std::make_unique<ValidValuesFixed>();
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

            formulationAndRoute->setValidDoses(std::move(validDoses));

            auto fixedIntervals = std::make_unique<ValidValuesFixed>();
            fixedIntervals->addValue(6);
            fixedIntervals->addValue(12);
            fixedIntervals->addValue(24);

            auto validIntervals =
                    std::make_unique<ValidDurations>(TucuUnit("h"), std::make_unique<PopulationValue>("", 24));
            validIntervals->addValues(std::move(fixedIntervals));

            formulationAndRoute->setValidIntervals(std::move(validIntervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        auto timeConsiderations = std::make_unique<TimeConsiderations>();

        // Arbitrary 20h half life. To be checked. Multiplier of 20
        // With a multiplier of 10 it fails
        auto halfLife = std::make_unique<HalfLife>("halflife", 20, Tucuxi::Common::TucuUnit("h"), 20);
        timeConsiderations->setHalfLife(std::move(halfLife));
        auto outdatedMeasure = std::make_unique<OutdatedMeasure>("id", 0.0, TucuUnit());
        timeConsiderations->setOutdatedMeasure(std::move(outdatedMeasure));
        model->setTimeConsiderations(std::move(timeConsiderations));

        Tucuxi::Core::OperationCollection collection;
        collection.populate();
        std::shared_ptr<Operation> sharedOperation = collection.getOperationFromId("sum2");

        std::unique_ptr<Operation> activeMoietyOperation = std::unique_ptr<Operation>(sharedOperation->clone());

        std::vector<AnalyteId> analyteList;
        analyteList.push_back(AnalyteId("analyte0"));
        analyteList.push_back(AnalyteId("analyte1"));
        auto activeMoiety = std::make_unique<ActiveMoiety>(
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
