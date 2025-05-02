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


#ifndef BUILDCONSTANTELIMINATION_H
#define BUILDCONSTANTELIMINATION_H


#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/hardcodedoperation.h"

#include "drugmodelbuilder.h"

using namespace Tucuxi::Core;

class BuildConstantElimination
{
public:
    BuildConstantElimination() {}


    // No targets are defined within the build, to let tests define various targets
    std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel(
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

        model->setDrugId("drugTest");
        model->setDrugModelId("constantEliminationBolus1");

        auto metaData = std::make_unique<DrugModelMetadata>();
        metaData->addAtc("fake Atc");
        Tucuxi::Common::TranslatableString drugName;
        drugName.setString("constant elimination test");
        metaData->setDrugName(drugName);
        metaData->setAuthorName("The authors");
        model->setMetadata(std::move(metaData));

        auto analyteSet = std::make_unique<AnalyteSet>();

        analyteSet->setId("analyteSet");
        analyteSet->setPkModelId("test.constantelimination");

        std::unique_ptr<Analyte> analyte = std::make_unique<Analyte>(
                "analyte", TucuUnit("ug/l"), std::make_unique<MolarMass>(10.0, TucuUnit("mol/l")));
        analyte->setAnalyteId("analyte");



        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covS",
                "0.0",
                nullptr,
                CovariateType::Standard,
                DataType::Double,
                Tucuxi::Common::TranslatableString("covS")));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covA",
                "0.0",
                nullptr,
                CovariateType::Standard,
                DataType::Double,
                Tucuxi::Common::TranslatableString("covA")));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covR",
                "0.0",
                nullptr,
                CovariateType::Standard,
                DataType::Double,
                Tucuxi::Common::TranslatableString("covR")));

        model->addCovariate(std::make_unique<Tucuxi::Core::CovariateDefinition>(
                "covM",
                "1.0",
                nullptr,
                CovariateType::Standard,
                DataType::Double,
                Tucuxi::Common::TranslatableString("covM")));

        auto errorModel = std::make_unique<ErrorModel>();

        errorModel->setErrorModel(_errorModelType);
        for (size_t i = 0; i < _sigmas.size(); i++) {
            std::string sigmaName = "sigma" + std::to_string(i);
            errorModel->addOriginalSigma(std::make_unique<PopulationValue>(sigmaName, _sigmas[i]));
        }

        analyte->setResidualErrorModel(std::move(errorModel));
        analyteSet->addAnalyte(std::move(analyte));

        auto dispositionParameters = std::make_unique<ParameterSetDefinition>();

        auto opS = std::make_unique<JSOperation>(
                "return covS;", OperationInputList{OperationInput("covS", InputType::DOUBLE)});
        auto PS = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestS",
                0.0,
                std::move(opS),
                std::make_unique<ParameterVariability>(_variabilityTypeS, _variabilityValueS));
        dispositionParameters->addParameter(std::move(PS));
        auto opA = std::make_unique<JSOperation>(
                "return covA;", OperationInputList{OperationInput("covA", InputType::DOUBLE)});
        auto PA = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestA",
                0.0,
                std::move(opA),
                std::make_unique<ParameterVariability>(_variabilityTypeA, _variabilityValueA));
        dispositionParameters->addParameter(std::move(PA));
        auto opR = std::make_unique<JSOperation>(
                "return covR;", OperationInputList{OperationInput("covR", InputType::DOUBLE)});
        auto PR = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestR",
                0.0,
                std::move(opR),
                std::make_unique<ParameterVariability>(_variabilityTypeR, _variabilityValueR));
        dispositionParameters->addParameter(std::move(PR));
        auto opM = std::make_unique<JSOperation>(
                "return covM;", OperationInputList{OperationInput("covM", InputType::DOUBLE)});
        auto PM = std::make_unique<Tucuxi::Core::ParameterDefinition>(
                "TestM",
                1.0,
                std::move(opM),
                std::make_unique<ParameterVariability>(_variabilityTypeM, _variabilityValueM));
        dispositionParameters->addParameter(std::move(PM));

        analyteSet->setDispositionParameters(std::move(dispositionParameters));



        model->addAnalyteSet(std::move(analyteSet));

        auto drugDomain = std::make_unique<DrugModelDomain>();

        model->setDomain(std::move(drugDomain));

        {
            const AnalyteSet* a = model->getAnalyteSet();
            auto association = std::make_unique<AnalyteSetToAbsorptionAssociation>(*a);
            association->setAbsorptionModel(AbsorptionModel::Extravascular);

            auto absorptionParameters = std::make_unique<ParameterSetDefinition>();

            association->setAbsorptionParameters(std::move(absorptionParameters));
            DMFormulationAndRoute formulationSpecs(
                    Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "No details");
            auto formulationAndRoute = std::make_unique<FullFormulationAndRoute>(formulationSpecs, "extraId");
            formulationAndRoute->addAssociation(std::move(association));

            std::unique_ptr<AnalyteConversion> analyteConversion =
                    std::make_unique<AnalyteConversion>(AnalyteId("analyte"), 1.0);
            formulationAndRoute->addAnalyteConversion(std::move(analyteConversion));

            auto validDoses = std::make_unique<ValidDoses>(TucuUnit("mg"), std::make_unique<PopulationValue>(400));
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
        std::shared_ptr<Operation> sharedOperation = collection.getOperationFromId("direct");

        std::unique_ptr<Operation> activeMoietyOperation = std::unique_ptr<Operation>(sharedOperation->clone());

        std::vector<AnalyteId> analyteList;
        analyteList.push_back(AnalyteId("analyte"));
        std::unique_ptr<ActiveMoiety> activeMoiety = std::make_unique<ActiveMoiety>(
                ActiveMoietyId("activeMoietyConstant"),
                TucuUnit("ug/l"),
                analyteList,
                std::move(activeMoietyOperation));


        Tucuxi::Common::TranslatableString activeMoietyName;
        activeMoietyName.setString("Active moiety name");
        activeMoiety->setName(activeMoietyName);

        model->addActiveMoiety(std::move(activeMoiety));

        return model;
    }
};

#endif // BUILDCONSTANTELIMINATION_H
