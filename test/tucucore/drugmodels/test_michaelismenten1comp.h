#ifndef TEST_MICHAELISMENTEN1COMP_H
#define TEST_MICHAELISMENTEN1COMP_H


#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "fructose/fructose.h"

#include "tucucommon/datetime.h"


#include "tucucore/drugmodelimport.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingcomponent.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;


static std::string test_mm_1comp_bolus_tdd = R"(<?xml version="1.0" encoding="UTF-8"?>
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
                <absorptionModelId>bolus</absorptionModelId>
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
                    <absorptionModelId>bolus</absorptionModelId>
                        <parameterSet>
                            <parameters>
                                <parameter>
                                    <parameterId>F</parameterId>
                                    <unit>%</unit>
                                    <parameterValue>
                                        <standardValue>1</standardValue>
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

struct TestMichaelisMenten1comp : public fructose::test_base<TestMichaelisMenten1comp>
{
    TestMichaelisMenten1comp() { }

    void buildDrugTreatment(DrugTreatment *&_drugTreatment, FormulationAndRoute _route)
    {
        _drugTreatment = new DrugTreatment();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startSept2018(date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                               Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(200.0),
                                 Unit("mg"),
                                 _route,
                                 Duration(),
                                 Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> sept2018(new Tucuxi::Core::DosageTimeRange(startSept2018, repeatedDose));


        _drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);
    }


    /// \brief Check that objects are correctly constructed by the constructor.
    void testMichaelisMenten1comp(const std::string& /* _testName */)
    {
        DrugModelImport importer;

        DrugModel *drugModel;

        importer.importFromString(drugModel, test_mm_1comp_bolus_tdd);

        fructose_assert(drugModel != nullptr);


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);


        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::ParenteralSolution,
                                        AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);


        buildDrugTreatment(drugTreatment, route);

        {

            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 5, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq( result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData *resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getIds().size(), size_t{1});
            fructose_assert_eq(resp->getIds()[0], "analyte");

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
            std::unique_ptr<ComputingTraitConcentration> traits =
                    std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, start, end, nbPointsPerHour, computingOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq( result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData *resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getIds().size(), size_t{1});
            fructose_assert_eq(resp->getIds()[0], "analyte");

            //std::cout << "A priori parameters : " << std::endl;
            //for (auto parameter : resp->getData()[0].m_parameters) {
            //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
            //}
        }


        if (drugTreatment != nullptr) {
            delete drugTreatment;
        }
        if (drugModel != nullptr) {
            delete drugModel;
        }
        if (component != nullptr) {
            delete component;
        }
    }
};


#endif // TEST_MICHAELISMENTEN1COMP_H
