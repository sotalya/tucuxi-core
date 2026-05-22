/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


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

    std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel(
            //the method buildDrugModel() takes a certain number of arguments, that were relevant for other drug models. Do not hesitate to add some (for instance we could have 4 times a conversionFactor instead of 2, ...).
            double _conversionFactor0 = 1.0,
            double _conversionFactor1 = 1.0,
            double _conversionFactor2 = 1.0,
            double _conversionFactor3 = 1.0,
            ResidualErrorType _errorModelType = ResidualErrorType::NONE,
            std::vector<Value> _sigmas = {0.0},
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeA0 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeM0 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeR0 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeS0 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeA1 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeM1 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeR1 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeS1 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeA2 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeM2 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeR2 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeS2 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeA3 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeM3 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeR3 = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeS3 = Tucuxi::Core::ParameterVariabilityType::None,

            Value _variabilityValueA0 = 0.0,
            Value _variabilityValueM0 = 1.0,
            Value _variabilityValueR0 = 0.0,
            Value _variabilityValueS0 = 0.0,
            Value _variabilityValueA1 = 0.0,
            Value _variabilityValueM1 = 1.0,
            Value _variabilityValueR1 = 0.0,
            Value _variabilityValueS1 = 0.0,
            Value _variabilityValueA2 = 0.0,
            Value _variabilityValueM2 = 1.0,
            Value _variabilityValueR2 = 0.0,
            Value _variabilityValueS2 = 0.0,
            Value _variabilityValueA3 = 0.0,
            Value _variabilityValueM3 = 1.0,
            Value _variabilityValueR3 = 0.0,
            Value _variabilityValueS3 = 0.0)

    {
        auto model = std::make_unique<Tucuxi::Core::DrugModel>();

        model->setDrugId("drugTestMultiAnalytesMultiActiveMoieties");
        model->setDrugModelId("MultiAnalytesMultiActiveMoieties");

        std::unique_ptr<DrugModelMetadata> metaData = std::make_unique<DrugModelMetadata>();
        metaData->addAtc("fake Atc");
        Tucuxi::Common::TranslatableString drugName;
        drugName.setString("Multi analytes multi active moieties test");
        metaData->setDrugName(drugName);
        metaData->setAuthorName("The authors");
        model->setMetadata(std::move(metaData));


        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covS0", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covA0", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covR0", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covM0", "1.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covS1", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covA1", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covR1", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covM1", "1.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covS2", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covA2", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covR2", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covM2", "1.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covS3", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covA3", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covR3", "0.0", nullptr, CovariateType::Standard));

        model->addCovariate(
                std::make_unique<Tucuxi::Core::CovariateDefinition>("covM3", "1.0", nullptr, CovariateType::Standard));

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

            auto opS0 = std::make_unique<JSOperation>(
                    "return covS0;", OperationInputList{OperationInput("covS0", InputType::DOUBLE)});
            auto pS0 = std::make_unique<ParameterDefinition>(
                    "TestS",
                    0.0,
                    std::move(opS0),
                    std::make_unique<ParameterVariability>(_variabilityTypeS0, _variabilityValueS0));
            dispositionParameters->addParameter(std::move(pS0));
            auto opA0 = std::make_unique<JSOperation>(
                    "return covA0;", OperationInputList{OperationInput("covA0", InputType::DOUBLE)});
            auto pA0 = std::make_unique<ParameterDefinition>(
                    "TestA",
                    0.0,
                    std::move(opA0),
                    std::make_unique<ParameterVariability>(_variabilityTypeA0, _variabilityValueA0));
            dispositionParameters->addParameter(std::move(pA0));
            auto opR0 = std::make_unique<JSOperation>(
                    "return covR0;", OperationInputList{OperationInput("covR0", InputType::DOUBLE)});
            auto pR0 = std::make_unique<ParameterDefinition>(
                    "TestR",
                    0.0,
                    std::move(opR0),
                    std::make_unique<ParameterVariability>(_variabilityTypeR0, _variabilityValueR0));
            dispositionParameters->addParameter(std::move(pR0));
            auto opM0 = std::make_unique<JSOperation>(
                    "return covM0;", OperationInputList{OperationInput("covM0", InputType::DOUBLE)});
            auto pM0 = std::make_unique<ParameterDefinition>(
                    "TestM",
                    1.0,
                    std::move(opM0),
                    std::make_unique<ParameterVariability>(_variabilityTypeM0, _variabilityValueM0));
            dispositionParameters->addParameter(std::move(pM0));

            analyteSet0->setDispositionParameters(std::move(dispositionParameters));



            model->addAnalyteSet(std::move(analyteSet0));
        }

        // Build analyteSet1
        {
            auto analyteSet1 = std::make_unique<AnalyteSet>();

            analyteSet1->setId("analyteSet1");
            analyteSet1->setPkModelId("test.constantelimination");

            std::unique_ptr<Analyte> analyte1 = std::make_unique<Analyte>(
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

            auto opS1 = std::make_unique<JSOperation>(
                    "return covS1;", OperationInputList{OperationInput("covS1", InputType::DOUBLE)});
            auto pS1 = std::make_unique<ParameterDefinition>(
                    "TestS",
                    0.0,
                    std::move(opS1),
                    std::make_unique<ParameterVariability>(_variabilityTypeS1, _variabilityValueS1));
            dispositionParameters->addParameter(std::move(pS1));
            auto opA1 = std::make_unique<JSOperation>(
                    "return covA1;", OperationInputList{OperationInput("covA1", InputType::DOUBLE)});
            auto pA1 = std::make_unique<ParameterDefinition>(
                    "TestA",
                    0.0,
                    std::move(opA1),
                    std::make_unique<ParameterVariability>(_variabilityTypeA1, _variabilityValueA1));
            dispositionParameters->addParameter(std::move(pA1));
            auto opR1 = std::make_unique<JSOperation>(
                    "return covR1;", OperationInputList{OperationInput("covR1", InputType::DOUBLE)});
            auto pR1 = std::make_unique<ParameterDefinition>(
                    "TestR",
                    0.0,
                    std::move(opR1),
                    std::make_unique<ParameterVariability>(_variabilityTypeR1, _variabilityValueR1));
            dispositionParameters->addParameter(std::move(pR1));
            auto opM1 = std::make_unique<JSOperation>(
                    "return covM1;", OperationInputList{OperationInput("covM1", InputType::DOUBLE)});
            auto pM1 = std::make_unique<ParameterDefinition>(
                    "TestM",
                    1.0,
                    std::move(opM1),
                    std::make_unique<ParameterVariability>(_variabilityTypeM1, _variabilityValueM1));
            dispositionParameters->addParameter(std::move(pM1));

            analyteSet1->setDispositionParameters(std::move(dispositionParameters));



            model->addAnalyteSet(std::move(analyteSet1));
        }


        //Build AnalyteSet2

        auto analyteSet2 = std::make_unique<AnalyteSet>();

        analyteSet2->setId("analyteSet2");
        analyteSet2->setPkModelId("test.multiconstantelimination");

        std::unique_ptr<Analyte> analyte2 = std::make_unique<Analyte>(
                "analyte2", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));
        std::unique_ptr<Analyte> analyte3 = std::make_unique<Analyte>(
                "analyte3", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));

        // TODO : Here we use the same error model for all analytes. That should be changed
        //        to allow different error models, thanks to the function parameters
        auto errorModel2 = std::make_unique<ErrorModel>();
        errorModel2->setErrorModel(_errorModelType);
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

        auto dispositionParameters = std::make_unique<ParameterSetDefinition>();

        auto opS2 = std::make_unique<JSOperation>(
                "return covS2;", OperationInputList{OperationInput("covS2", InputType::DOUBLE)});
        auto pS2 = std::make_unique<ParameterDefinition>(
                "TestS0",
                0.0,
                std::move(opS2),
                std::make_unique<ParameterVariability>(_variabilityTypeS2, _variabilityValueS2));
        dispositionParameters->addParameter(std::move(pS2));
        auto opA2 = std::make_unique<JSOperation>(
                "return covA2;", OperationInputList{OperationInput("covA2", InputType::DOUBLE)});
        auto pA2 = std::make_unique<ParameterDefinition>(
                "TestA0",
                0.0,
                std::move(opA2),
                std::make_unique<ParameterVariability>(_variabilityTypeA2, _variabilityValueA2));
        dispositionParameters->addParameter(std::move(pA2));
        auto opR2 = std::make_unique<JSOperation>(
                "return covR2;", OperationInputList{OperationInput("covR2", InputType::DOUBLE)});
        auto pR2 = std::make_unique<ParameterDefinition>(
                "TestR0",
                0.0,
                std::move(opR2),
                std::make_unique<ParameterVariability>(_variabilityTypeR2, _variabilityValueR2));
        dispositionParameters->addParameter(std::move(pR2));
        auto opM2 = std::make_unique<JSOperation>(
                "return covM2;", OperationInputList{OperationInput("covM2", InputType::DOUBLE)});
        auto pM2 = std::make_unique<ParameterDefinition>(
                "TestM0",
                1.0,
                std::move(opM2),
                std::make_unique<ParameterVariability>(_variabilityTypeM2, _variabilityValueM2));
        dispositionParameters->addParameter(std::move(pM2));
        auto opS3 = std::make_unique<JSOperation>(
                "return covS3;", OperationInputList{OperationInput("covS3", InputType::DOUBLE)});
        auto pS3 = std::make_unique<ParameterDefinition>(
                "TestS1",
                0.0,
                std::move(opS3),
                std::make_unique<ParameterVariability>(_variabilityTypeS3, _variabilityValueS3));
        dispositionParameters->addParameter(std::move(pS3));
        auto opA3 = std::make_unique<JSOperation>(
                "return covA3;", OperationInputList{OperationInput("covA3", InputType::DOUBLE)});
        auto pA3 = std::make_unique<ParameterDefinition>(
                "TestA1",
                0.0,
                std::move(opA3),
                std::make_unique<ParameterVariability>(_variabilityTypeA3, _variabilityValueA3));
        dispositionParameters->addParameter(std::move(pA3));
        auto opR3 = std::make_unique<JSOperation>(
                "return covR3;", OperationInputList{OperationInput("covR3", InputType::DOUBLE)});
        auto pR3 = std::make_unique<ParameterDefinition>(
                "TestR1",
                0.0,
                std::move(opR3),
                std::make_unique<ParameterVariability>(_variabilityTypeR3, _variabilityValueR3));
        dispositionParameters->addParameter(std::move(pR3));
        auto opM3 = std::make_unique<JSOperation>(
                "return covM3;", OperationInputList{OperationInput("covM3", InputType::DOUBLE)});
        auto pM3 = std::make_unique<ParameterDefinition>(
                "TestM1",
                1.0,
                std::move(opM3),
                std::make_unique<ParameterVariability>(_variabilityTypeM3, _variabilityValueM3));
        dispositionParameters->addParameter(std::move(pM3));

        analyteSet2->setDispositionParameters(std::move(dispositionParameters));



        model->addAnalyteSet(std::move(analyteSet2));


        auto drugDomain = std::make_unique<DrugModelDomain>();

        model->setDomain(std::move(drugDomain));

        {
            DMFormulationAndRoute formulationSpecs(
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

            std::unique_ptr<AnalyteConversion> analyteConversion0 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte0"), _conversionFactor0);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion0));
            std::unique_ptr<AnalyteConversion> analyteConversion1 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte1"), _conversionFactor1);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion1));
            std::unique_ptr<AnalyteConversion> analyteConversion2 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte2"), _conversionFactor2);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion2));
            std::unique_ptr<AnalyteConversion> analyteConversion3 =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte3"), _conversionFactor3);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion3));

            auto validDoses = std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
            std::unique_ptr<ValidValuesFixed> specificDoses = std::make_unique<ValidValuesFixed>();
            specificDoses->addValue(DoseValue{100});
            specificDoses->addValue(DoseValue{200});
            specificDoses->addValue(DoseValue{300});
            specificDoses->addValue(DoseValue{400});
            specificDoses->addValue(DoseValue{500});
            specificDoses->addValue(DoseValue{600});
            specificDoses->addValue(DoseValue{700});
            specificDoses->addValue(DoseValue{800});
            specificDoses->addValue(DoseValue{900});
            specificDoses->addValue(DoseValue{1000});

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

        auto activeMoietyOperation0 = sharedOperation->clone();
        auto activeMoietyOperation1 = sharedOperation->clone();

        std::vector<AnalyteId> analyteList0;
        analyteList0.push_back(AnalyteId("analyte0"));
        analyteList0.push_back(AnalyteId("analyte1"));

        std::unique_ptr<ActiveMoiety> activeMoiety0 = std::make_unique<ActiveMoiety>(
                ActiveMoietyId("activeMoietyMulti0"),
                TucuUnit("ug/l"),
                analyteList0,
                std::move(activeMoietyOperation0));

        std::vector<AnalyteId> analyteList1;
        analyteList1.push_back(AnalyteId("analyte2"));
        analyteList1.push_back(AnalyteId("analyte3"));
        std::unique_ptr<ActiveMoiety> activeMoiety1 = std::make_unique<ActiveMoiety>(
                ActiveMoietyId("activeMoietyMulti1"),
                TucuUnit("ug/l"),
                analyteList1,
                std::move(activeMoietyOperation1));

        Tucuxi::Common::TranslatableString activeMoietyName0;
        activeMoietyName0.setString("Test MAMA Active Moiety 1");
        activeMoiety0->setName(activeMoietyName0);

        Tucuxi::Common::TranslatableString activeMoietyName1;
        activeMoietyName1.setString("Test MAMA Active moiety with a group of 2 analytes");
        activeMoiety1->setName(activeMoietyName1);

        // I removed the targets from the build, to let tests define various targets
        /*
        // Add targets
        // Add targets
        auto target = std::make_unique<TargetDefinition>(TargetType::Residual,
                                                        Unit("mg/l"),
                                                        "analyte",
                                                        std::make_unique<SubTargetDefinition>("cMin", 750.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("cMax", 1500.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("cBest", 1000.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("mic", 0.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("tMin", 1000.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("tMax", 1000.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("tBest", 1000.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("toxicity", 10000.0, nullptr),
                                                        std::make_unique<SubTargetDefinition>("inefficacy", 000.0, nullptr));

        activeMoiety->addTarget(std::move(target));
*/

        model->addActiveMoiety(std::move(activeMoiety0));
        model->addActiveMoiety(std::move(activeMoiety1));

        return model;
    }
};


#endif // BUILDMULTIANALYTESMULTIACTIVEMOIETIES_H
