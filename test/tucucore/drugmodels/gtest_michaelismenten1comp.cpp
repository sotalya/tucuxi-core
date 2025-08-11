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


#include <chrono>
#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

#include "../gtest_core.h"


using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;


static const std::string test_mm_1comp_bolus_tdd = R"(<?xml version="1.0" encoding="UTF-8"?>
<model version='0.6' xsi:noNamespaceSchemaLocation='drug2.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
    <history>
        <revisions>
            <revision>
                <revisionAction>creation</revisionAction>
                <revisionAuthorName>
                </revisionAuthorName>
                <institution>
                </institution>
                <email>
                </email>
                <date>
                </date>
                <comments/>
            </revision>
        </revisions>
    </history>
    <head>
        <drug>
            <atcs>
                <atc>
                </atc>
            </atcs>
            <activeSubstances>
                <activeSubstance>
                </activeSubstance>
            </activeSubstances>
            <drugName>
                <name lang='en'>
                </name>
            </drugName>
            <drugDescription>
                <desc lang='en'>
                </desc>
            </drugDescription>
            <tdmStrategy>
                <text lang='en'>
                </text>
            </tdmStrategy>
        </drug>
        <study>
            <studyName>
                <name lang='en'>
                </name>
            </studyName>
            <studyAuthors>
            </studyAuthors>
            <description>
                <desc lang='en'>
                </desc>
            </description>
            <references/>
        </study>
        <comments/>
    </head>
    <drugModel>
        <drugId>fakeMM</drugId>
        <drugModelId>test.mm.1comp.bolus</drugModelId>
        <domain>
            <description>
                <desc lang='en'>
                </desc>
            </description>
            <constraints/>
        </domain>
        <covariates/>
        <activeMoieties>
            <activeMoiety>
                <activeMoietyId>activeMoiety</activeMoietyId>
                <activeMoietyName>
                    <name lang='en'>test MM 1comp</name>
                </activeMoietyName>
                <unit>ug/l</unit>
                <analyteIdList>
                    <analyteId>analyte</analyteId>
                </analyteIdList>
                <analytesToMoietyFormula>
                    <hardFormula>direct</hardFormula>
                    <comments/>
                </analytesToMoietyFormula>
                <targets/>
            </activeMoiety>
        </activeMoieties>
        <analyteGroups>
            <analyteGroup>
                <groupId>group</groupId>
                <pkModelId>michaelismenten.1comp</pkModelId>
                <analytes>
                    <analyte>
                        <analyteId>analyte</analyteId>
                        <unit>ug/l</unit>
                        <molarMass>
                            <value>1</value>
                            <unit>g/mol</unit>
                        </molarMass>
                        <description>
                            <desc lang='en'>
                            </desc>
                        </description>
                        <errorModel>
                            <errorModelType>exponential</errorModelType>
                            <sigmas>
                                <sigma>
                                    <standardValue>.1</standardValue>
                                </sigma>
                            </sigmas>
                            <comments/>
                        </errorModel>
                        <comments/>
                    </analyte>
                </analytes>
                <dispositionParameters>
                    <parameters>
                        <parameter>
                            <parameterId>Km</parameterId>
                            <unit>l/h</unit>
                            <parameterValue>
                                <standardValue>100000</standardValue>
                            </parameterValue>
                            <bsv>
                                <bsvType>none</bsvType>
                            </bsv>
                            <validation>
                                <errorMessage>
                                    <text lang='fr'>
                                    </text>
                                </errorMessage>
                                <operation>
                                    <softFormula>
                                        <inputs>
                                            <input>
                                                <id>Km</id>
                                                <type>double</type>
                                            </input>
                                        </inputs>
                                        <code>
                                            <![CDATA[
                                            return Km > 0.0;
                                            ]]>
                                        </code>
                                    </softFormula>
                                    <comments/>
                                </operation>
                                <comments/>
                            </validation>
                            <comments/>
                        </parameter>
                        <parameter>
                            <parameterId>Vmax</parameterId>
                            <unit>l/h</unit>
                            <parameterValue>
                                <standardValue>100</standardValue>
                            </parameterValue>
                            <bsv>
                                <bsvType>none</bsvType>
                            </bsv>
                            <validation>
                                <errorMessage>
                                    <text lang='fr'>
                                    </text>
                                </errorMessage>
                                <operation>
                                    <softFormula>
                                        <inputs>
                                            <input>
                                                <id>Vmax</id>
                                                <type>double</type>
                                            </input>
                                        </inputs>
                                        <code>
                                            <![CDATA[
                                            return Vmax > 0.0;
                                            ]]>
                                        </code>
                                    </softFormula>
                                    <comments/>
                                </operation>
                                <comments/>
                            </validation>
                            <comments/>
                        </parameter>
                        <parameter>
                            <parameterId>V</parameterId>
                            <unit>l</unit>
                            <parameterValue>
                                <standardValue>1</standardValue>
                            </parameterValue>
                            <bsv>
                                <bsvType>none</bsvType>
                            </bsv>
                            <validation>
                                <errorMessage>
                                    <text lang='fr'>
                                    </text>
                                </errorMessage>
                                <operation>
                                    <softFormula>
                                        <inputs>
                                            <input>
                                                <id>V</id>
                                                <type>double</type>
                                            </input>
                                        </inputs>
                                        <code>
                                            <![CDATA[
                                            return V > 0.0;
                                            ]]>
                                        </code>
                                    </softFormula>
                                    <comments/>
                                </operation>
                                <comments/>
                            </validation>
                            <comments/>
                        </parameter>
                    </parameters>
                    <correlations/>
                </dispositionParameters>
            </analyteGroup>
        </analyteGroups>
        <formulationAndRoutes default='id0'>
            <formulationAndRoute>
                <formulationAndRouteId>id0</formulationAndRouteId>
                <formulation>parenteral solution</formulation>
                <administrationName>admname</administrationName>
                <administrationRoute>intravenousBolus</administrationRoute>
                <absorptionModel>bolus</absorptionModel>
                <dosages>
                    <analyteConversions>
                        <analyteConversion>
                            <analyteId>analyte</analyteId>
                            <factor>1</factor>
                        </analyteConversion>
                    </analyteConversions>
                    <availableDoses>
                        <unit>mg</unit>
                        <default>
                            <standardValue>400</standardValue>
                        </default>
                        <rangeValues>
                            <from>
                                <standardValue>100</standardValue>
                            </from>
                            <to>
                                <standardValue>400</standardValue>
                            </to>
                            <step>
                                <standardValue>100</standardValue>
                            </step>
                        </rangeValues>
                        <fixedValues>
                            <value>600</value>
                            <value>800</value>
                        </fixedValues>
                    </availableDoses>
                    <availableIntervals>
                        <unit>h</unit>
                        <default>
                            <standardValue>24</standardValue>
                        </default>
                        <fixedValues>
                            <value>12</value>
                            <value>24</value>
                        </fixedValues>
                    </availableIntervals>
                    <comments/>
                </dosages>
              <absorptionParameters/>
            </formulationAndRoute>
)"
                                                   R"(
            <formulationAndRoute>
                <formulationAndRouteId>id1</formulationAndRouteId>
                <formulation>parenteral solution</formulation>
                <administrationName>admname</administrationName>
                <administrationRoute>oral</administrationRoute>
                <absorptionModel>extra</absorptionModel>
                <dosages>
                    <analyteConversions>
                        <analyteConversion>
                            <analyteId>analyte</analyteId>
                            <factor>1</factor>
                        </analyteConversion>
                    </analyteConversions>
                    <availableDoses>
                        <unit>mg</unit>
                        <default>
                            <standardValue>400</standardValue>
                        </default>
                        <rangeValues>
                            <from>
                                <standardValue>100</standardValue>
                            </from>
                            <to>
                                <standardValue>400</standardValue>
                            </to>
                            <step>
                                <standardValue>100</standardValue>
                            </step>
                        </rangeValues>
                        <fixedValues>
                            <value>600</value>
                            <value>800</value>
                        </fixedValues>
                    </availableDoses>
                    <availableIntervals>
                        <unit>h</unit>
                        <default>
                            <standardValue>24</standardValue>
                        </default>
                        <fixedValues>
                            <value>12</value>
                            <value>24</value>
                        </fixedValues>
                    </availableIntervals>
                    <comments/>
                </dosages>


                <absorptionParameters>
                    <parameterSetAnalyteGroup>
                    <analyteGroupId>group</analyteGroupId>
                    <absorptionModel>extra</absorptionModel>
                        <parameterSet>
                            <parameters>
                                <parameter>
                                    <parameterId>F</parameterId>
                                    <unit>%</unit>
                                    <parameterValue>
                                        <standardValue>0.8</standardValue>
                                    </parameterValue>
                                    <bsv>
                                        <bsvType>none</bsvType>
                                    </bsv>
                                    <validation>
                                        <errorMessage><text lang="fr"></text></errorMessage>
                                        <operation>
                                            <softFormula>
                                                <inputs>
                                                    <input>
                                                        <id>F</id>
                                                        <type>double</type>
                                                    </input>
                                                </inputs>
                                                <code><![CDATA[
                                                    return F <= 1.0 and F > 0.0;
                                                    ]]>
                                                </code>
                                            </softFormula>
                                            <comments/>
                                        </operation>
                                        <comments/>
                                    </validation>
                                    <comments>
                                    </comments>
                                </parameter>
                                <parameter>
                                    <parameterId>Ka</parameterId>
                                    <unit>-</unit>
                                    <parameterValue>
                                        <standardValue>0.7</standardValue>
                                    </parameterValue>
                                    <bsv>
                                        <bsvType>none</bsvType>
                                    </bsv>
                                    <validation>
                                        <errorMessage><text lang="fr"></text></errorMessage>
                                        <operation>
                                            <softFormula>
                                                <inputs>
                                                    <input>
                                                        <id>F</id>
                                                        <type>double</type>
                                                    </input>
                                                </inputs>
                                                <code><![CDATA[
                                                    return F <= 1.0 and F > 0.0;
                                                    ]]>
                                                </code>
                                            </softFormula>
                                            <comments/>
                                        </operation>
                                        <comments/>
                                    </validation>
                                    <comments>
                                    </comments>
                                </parameter>
                            </parameters>
                            <correlations/>
                        </parameterSet>
                    </parameterSetAnalyteGroup>
                </absorptionParameters>
            </formulationAndRoute>
        </formulationAndRoutes>
        <timeConsiderations>
            <halfLife>
                <unit>h</unit>
                <duration>
                    <standardValue>10</standardValue>
                </duration>
                <multiplier>10</multiplier>
                <comments>
                    <comment lang='en'>
                    </comment>
                </comments>
            </halfLife>
            <outdatedMeasure>
                <unit>d</unit>
                <duration>
                    <standardValue>10</standardValue>
                </duration>
                <comments>
                    <comment lang='en'>
                    </comment>
                </comments>
            </outdatedMeasure>
        </timeConsiderations>
        <comments/>
    </drugModel>
</model>)";

TEST(Core_TestMichaelisMenten1comp, MichaelisMenten1comp)
{
    DrugModelImport importer;

    std::unique_ptr<DrugModel> drugModel;

    auto importStatus = importer.importFromString(drugModel, test_mm_1comp_bolus_tdd);
    ASSERT_EQ(importStatus, DrugModelImport::Status::Ok);

    ASSERT_TRUE(drugModel != nullptr);


    auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    const FormulationAndRoute route(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018);

    {

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
        auto* resp = dynamic_cast<const SinglePredictionData*>(responseData);

        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

        //std::cout << "Population parameters : " << std::endl;
        //for (auto parameter : resp->getData()[0].m_parameters) {
        //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
        //}
    }

    {

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Apriori, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
        const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

        //std::cout << "A priori parameters : " << std::endl;
        //for (auto parameter : resp->getData()[0].m_parameters) {
        //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
        //}
    }

    if (component != nullptr) {
        delete component;
    }
}


TEST(Core_TestMichaelisMenten1comp, MichaelisMenten1compMixedRoutes)
{
    DrugModelImport importer;

    std::unique_ptr<DrugModel> drugModel;

    auto importStatus = importer.importFromString(drugModel, test_mm_1comp_bolus_tdd);
    ASSERT_EQ(importStatus, DrugModelImport::Status::Ok);

    ASSERT_TRUE(drugModel != nullptr);


    auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    const FormulationAndRoute route(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus);

    DateTime startSept2018(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto drugTreatment = buildDrugTreatment(route, startSept2018, DoseValue(200), TucuUnit("mg"), 6, 16);

    const FormulationAndRoute route2(Formulation::OralSolution, AdministrationRoute::Oral);

    DateTime startSept2018_2(
            date::year_month_day(date::year(2018), date::month(9), date::day(5)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    drugTreatment->addDosageTimeRange(buildDosageTimeRange(route2, startSept2018_2));
    {

        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
        Tucuxi::Common::DateTime end(2018_y / sep / 9, 8h + 0min);
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(
                PredictionParameterType::Population,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::RetrieveParameters);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
        auto* resp = dynamic_cast<const SinglePredictionData*>(responseData);

        ASSERT_EQ(resp->getCompartmentInfos().size(), static_cast<size_t>(1));
        ASSERT_EQ(resp->getCompartmentInfos()[0].getId(), "analyte");
        ASSERT_EQ(resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

        //for (size_t i = 0; i < resp->getData().size(); i++) {
        //    std::cout << "Cycle " << i << std::endl;
        //    for (const auto& p : resp->getData()[i].m_parameters) {
        //        std::cout << p.m_parameterId << " : " << p.m_value << std::endl;
        //    }
        //}

        // F
        ASSERT_EQ(resp->getData()[0].m_parameters[0].m_value, 0.8);
        // Ka
        ASSERT_EQ(resp->getData()[0].m_parameters[1].m_value, 0.7);

        // F
        ASSERT_EQ(resp->getData()[16].m_parameters[0].m_value, 0.8);
        // Ka
        ASSERT_EQ(resp->getData()[0].m_parameters[1].m_value, 0.7);

        //std::cout << "Population parameters : " << std::endl;
        //for (auto parameter : resp->getData()[0].m_parameters) {
        //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
        //}
    }

    if (component != nullptr) {
        delete component;
    }
}
