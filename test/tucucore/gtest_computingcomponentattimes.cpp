//@@license@@

#include <memory>

#include <gtest/gtest.h>

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/drugtreatment.h"


using namespace Tucuxi::Core;

using namespace std::chrono_literals;
using namespace date;

static const std::string test_mm_1comp_extra_tdd = R"(<?xml version="1.0" encoding="UTF-8"?>
<model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
    <history>
        <revisions>
            <revision>
                <revisionAction>creation</revisionAction>
                <revisionAuthorName>Anne Ravix</revisionAuthorName>
                <institution>chuv</institution>
                <email>anne.ravix@hotmail.fr</email>
                <date>2023-08-17</date>
                <comments/>
            </revision>
        </revisions>
    </history>
    <head>
        <drug>
            <atcs>
                <atc>0</atc>
            </atcs>
            <activeSubstances>
                <activeSubstance>virtualdrug</activeSubstance>
            </activeSubstances>
            <drugName>
                <name lang='en'>virtualdrug</name>
            </drugName>
            <drugDescription>
                <desc lang='en'>virtualdrug used for numerical validation between Tucuxi and Nonmem</desc>
            </drugDescription>
            <tdmStrategy>
                <text lang='en'>-</text>
            </tdmStrategy>
        </drug>
        <study>
            <studyName>
                <name lang='en'>Numerical validation of Tucuxi</name>
            </studyName>
            <studyAuthors>Anne Ravix
                                Annie Cathignol
                                Yann Thoma
                        Monia Guidi</studyAuthors>
            <description>
                <desc lang='en'>comparison of a posteriori PK parameters between tucuxi and nonmem for different virtual models
                                Model 1: 1CMT - Oral - Linear absorption - IIV on CL, KA, V - Proportional error</desc>
            </description>
            <references/>
        </study>
        <comments/>
    </head>
    <drugModel>
        <drugId>virtualdrug</drugId>
        <drugModelId>ch.tucuxi.virtualdrug.mod1</drugModelId>
        <domain>
            <description>
                <desc lang='en'>Model without covariates</desc>
            </description>
            <constraints />
        </domain>
        <covariates>
            <covariate>
                <covariateId>sampling_group</covariateId>
                <covariateName>
                    <name lang='en'>sampling_group</name>
                </covariateName>
                <description>
                    <desc lang='en'>Study group of the patient</desc>
                </description>
                <unit>-</unit>
                <covariateType>standard</covariateType>
                <dataType>int</dataType>
                <interpolationType>linear</interpolationType>
                <refreshPeriod>
                    <unit>y</unit>
                    <value>1</value>
                </refreshPeriod>
                <covariateValue>
                    <standardValue>1</standardValue>
                </covariateValue>
                <validation>
                    <errorMessage/>
                    <operation>
                        <softFormula>
                            <inputs>
                                <input>
                                    <id>sampling_group</id>
                                    <type>int</type>
                                </input>
                            </inputs>
                            <code>
                                <![CDATA[
                                return sampling_group>0;
                                ]]>
                            </code>
                        </softFormula>
                        <comments/>
                    </operation>
                    <comments/>
                </validation>
                <comments/>
            </covariate>
        </covariates>
        <activeMoieties>
            <activeMoiety>
                <activeMoietyId>virtualdrug</activeMoietyId>
                <activeMoietyName>
                    <name lang='en'>virtualdrug</name>
                </activeMoietyName>
                <unit>ug/l</unit>
                <analyteIdList>
                    <analyteId>virtualdrug</analyteId>
                </analyteIdList>
                <analytesToMoietyFormula>
                    <hardFormula>direct</hardFormula>
                    <comments/>
                </analytesToMoietyFormula>
                <targets>
                </targets>
            </activeMoiety>
        </activeMoieties>
        <analyteGroups>
            <analyteGroup>
                <groupId>virtualdrug</groupId>
                <pkModelId>linear.1comp.macro</pkModelId>
                <analytes>
                    <analyte>
                        <analyteId>virtualdrug</analyteId>
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
                            <errorModelType>proportional</errorModelType>
                            <sigmas>
                                <sigma>
                                    <standardValue>0.22</standardValue>
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
                            <parameterId>CL</parameterId>
                            <unit>l/h</unit>
                            <parameterValue>
                                <standardValue>4</standardValue>
                            </parameterValue>
                            <bsv>
                                <bsvType>exponential</bsvType>
                                <stdDevs>
                                    <stdDev>0.45</stdDev>
                                </stdDevs>
                            </bsv>
                            <validation>
                                <errorMessage>
                                    <text lang='en'>CL must be positive</text>
                                </errorMessage>
                                <operation>
                                    <softFormula>
                                        <inputs>
                                            <input>
                                                <id>CL</id>
                                                <type>double</type>
                                            </input>
                                        </inputs>
                                        <code>
                                            <![CDATA[
                                            return CL > 0.0;
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
                                <standardValue>70</standardValue>
                            </parameterValue>
                            <bsv>
                                <bsvType>exponential</bsvType>
                                <stdDevs>
                                    <stdDev>0.45</stdDev>
                                </stdDevs>
                            </bsv>
                            <validation>
                                <errorMessage>
                                    <text lang='en'>V must be positive</text>
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
                    <correlations />
                </dispositionParameters>
            </analyteGroup>
        </analyteGroups>
        <formulationAndRoutes>
            <formulationAndRoute>
                <formulationAndRouteId>id0</formulationAndRouteId>
                <formulation>oralSolution</formulation>
                <administrationName>foo bar</administrationName>
                <administrationRoute>oral</administrationRoute>
                <absorptionModel>extra</absorptionModel>
                <dosages>
                    <analyteConversions>
                        <analyteConversion>
                            <analyteId>virtualdrug</analyteId>
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
                        <analyteGroupId>virtualdrug</analyteGroupId>
                        <absorptionModel>extra</absorptionModel>
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
                                        <stdDevs />
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
                                                        <id>F</id>
                                                        <type>double</type>
                                                    </input>
                                                </inputs>
                                                <code>
                                                    <![CDATA[
                                                    return F <= 1.0 && F > 0.0;
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
                                        <parameterId>Ka</parameterId>
                                        <unit>h-1</unit>
                                        <parameterValue>
                                            <standardValue>1</standardValue>
                                        </parameterValue>
                                        <bsv>
                                            <bsvType>exponential</bsvType>
                                            <stdDevs>
                                                <stdDev>0.45</stdDev>
                                            </stdDevs>
                                        </bsv>
                                        <validation>
                                            <errorMessage>
                                                <text lang='en'>Ka must be positive</text>
                                            </errorMessage>
                                            <operation>
                                                <softFormula>
                                                    <inputs>
                                                        <input>
                                                            <id>Ka</id>
                                                            <type>double</type>
                                                        </input>
                                                    </inputs>
                                                    <code>
                                                        <![CDATA[
                                                        return Ka > 0.0;
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
                                <correlations />
                            </parameterSet>
                        </parameterSetAnalyteGroup>
                    </absorptionParameters>
                </formulationAndRoute>
            </formulationAndRoutes>)"
                                                   R"(
            <timeConsiderations>
                <halfLife>
                    <unit>h</unit>
                    <duration>
                        <standardValue>1</standardValue>
                    </duration>
                    <multiplier>200</multiplier>
                    <comments>
                        <comment lang='en'>
                        </comment>
                    </comments>
                </halfLife>
                <outdatedMeasure>
                    <unit>d</unit>
                    <duration>
                        <standardValue>15</standardValue>
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

static std::unique_ptr<DrugModel> buildDrugModel()
{
    DrugModelImport importer;

    std::unique_ptr<DrugModel> drugModel;

    auto importStatus = importer.importFromString(drugModel, test_mm_1comp_extra_tdd);
    EXPECT_EQ(importStatus, DrugModelImport::Status::Ok);

    EXPECT_TRUE(drugModel != nullptr);
    return drugModel;
}

static std::unique_ptr<DrugTreatment> buildSimpleDrugTreatment(
        FormulationAndRoute _route, DateTime& _startTime, Duration _interval, Duration _treatmentDuration)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    LastingDose periodicDose(DoseValue(200.0), TucuUnit("mg"), _route, Duration(), _interval);
    DosageRepeat repeatedDose(periodicDose, static_cast<unsigned int>(_treatmentDuration / _interval));
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(_startTime, repeatedDose);

    drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);

    return drugTreatment;
}

TEST(Core_TestComputingComponentAtTimes, MeasureFar)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    auto drugModel = buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    DateTime startTreatment(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
    Duration interval(std::chrono::hours(24));
    Duration treatmentDuration(std::chrono::hours(24 * 60));
    DateTime endTreatment = startTreatment + treatmentDuration;


    FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "foo bar");

    // Test of a posteriori concentration prediction with one unvalid sample too early in time
    auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);

    drugTreatment->addSample(std::make_unique<Sample>(
            endTreatment - Duration(std::chrono::hours(3)), AnalyteId("virtualdrug"), 100.0, TucuUnit("mg/l")));


    {
        // A sample at the very end of the 30-day treatment, and times asked at the beginning of it.
        // We compare the full prediction data with single points computations, as they should be equal.
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(startTreatment);
        Tucuxi::Common::DateTime end(startTreatment + 24h);
        double nbPointsPerHour = 12.0;
        ComputingOption computingOption(
                PredictionParameterType::Aposteriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::DoNotRetrieveStatistics,
                RetrieveParametersOption::DoNotRetrieveParameters,
                RetrieveCovariatesOption::DoNotRetrieveCovariates,
                ForceUgPerLiterOption::Force);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        std::vector<Tucuxi::Common::DateTime> times = {startTreatment + 1h, startTreatment + 2h};

        std::unique_ptr<ComputingTraitSinglePoints> traits2 =
                std::make_unique<ComputingTraitSinglePoints>(requestResponseId, times, computingOption);

        ComputingRequest request2(requestResponseId, *drugModel, *drugTreatment, std::move(traits2));

        std::unique_ptr<ComputingResponse> response2 = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result2;
        result2 = component->compute(request2, response2);

        ASSERT_EQ(result2, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
        const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);


        const ComputedData* responseData2 = response2->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePointsData*>(responseData2) != nullptr);
        const SinglePointsData* resp2 = dynamic_cast<const SinglePointsData*>(responseData2);

        auto data = resp->getData();
        auto data2 = resp2->m_concentrations;
        ASSERT_DOUBLE_EQ(data[0].getConcentrations()[0][12], data2[0][0]);
        ASSERT_DOUBLE_EQ(data[0].getConcentrations()[0][24], data2[0][1]);
    }


    // Delete all dynamically allocated objects
    delete component;
}

TEST(Core_TestComputingComponentAtTimes, MeasureFarTimes)
{
    IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);

    auto drugModel = buildDrugModel();
    ASSERT_TRUE(drugModel != nullptr);

    DateTime startTreatment(
            date::year_month_day(date::year(2018), date::month(9), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));
    Duration interval(std::chrono::hours(24));
    Duration treatmentDuration(std::chrono::hours(24 * 30));
    DateTime endTreatment = startTreatment + treatmentDuration;


    FormulationAndRoute route(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "foo bar");

    // Test of a posteriori concentration prediction with one unvalid sample too early in time
    auto drugTreatment = buildSimpleDrugTreatment(route, startTreatment, interval, treatmentDuration);

    drugTreatment->addSample(std::make_unique<Sample>(
            endTreatment - Duration(std::chrono::hours(3)), AnalyteId("virtualdrug"), 100.0, TucuUnit("mg/l")));


    {
        // A sample at the very end of the 30-day treatment, and times asked at the beginning and end of it.
        // We compare the full prediction data with single points computations, as they should be equal.
        RequestResponseId requestResponseId = "1";
        Tucuxi::Common::DateTime start(startTreatment);
        Tucuxi::Common::DateTime end(endTreatment);
        double nbPointsPerHour = 12.0;
        ComputingOption computingOption(
                PredictionParameterType::Aposteriori,
                CompartmentsOption::MainCompartment,
                RetrieveStatisticsOption::DoNotRetrieveStatistics,
                RetrieveParametersOption::DoNotRetrieveParameters,
                RetrieveCovariatesOption::DoNotRetrieveCovariates,
                ForceUgPerLiterOption::Force);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, start, end, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        ASSERT_EQ(result, ComputingStatus::Ok);

        std::vector<Tucuxi::Common::DateTime> times = {
                startTreatment + 1h, endTreatment - 24h + 1h, endTreatment - 24h + 2h};

        std::unique_ptr<ComputingTraitSinglePoints> traits2 =
                std::make_unique<ComputingTraitSinglePoints>(requestResponseId, times, computingOption);

        ComputingRequest request2(requestResponseId, *drugModel, *drugTreatment, std::move(traits2));

        std::unique_ptr<ComputingResponse> response2 = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result2;
        result2 = component->compute(request2, response2);

        ASSERT_EQ(result2, ComputingStatus::Ok);


        const ComputedData* responseData = response->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
        const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);


        const ComputedData* responseData2 = response2->getData();
        ASSERT_TRUE(dynamic_cast<const SinglePointsData*>(responseData2) != nullptr);
        const SinglePointsData* resp2 = dynamic_cast<const SinglePointsData*>(responseData2);

        auto data = resp->getData();
        auto data2 = resp2->m_concentrations;
        ASSERT_DOUBLE_EQ(data[29].getConcentrations()[0][12], data2[0][1]);
        ASSERT_DOUBLE_EQ(data[29].getConcentrations()[0][24], data2[0][2]);
    }

    /*
        {
            // A sample at the very end of the 30-day treatment, and times asked also at the end of it.
            // We compare the full prediction data with single points computations, as they should be equal.
            RequestResponseId requestResponseId = "1";
            Tucuxi::Common::DateTime start(startTreatment);
            Tucuxi::Common::DateTime end(endTreatment);
            double nbPointsPerHour = 12.0;
            ComputingOption computingOption(
                    PredictionParameterType::Aposteriori,
                    CompartmentsOption::MainCompartment,
                    RetrieveStatisticsOption::DoNotRetrieveStatistics,
                    RetrieveParametersOption::DoNotRetrieveParameters,
                    RetrieveCovariatesOption::DoNotRetrieveCovariates,
                    ForceUgPerLiterOption::Force);
            std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            ASSERT_EQ(result, ComputingStatus::Ok);

            std::vector<Tucuxi::Common::DateTime> times = {endTreatment - 24h + 1h, endTreatment - 24h + 2h};

            std::unique_ptr<ComputingTraitSinglePoints> traits2 = std::make_unique<ComputingTraitSinglePoints>(
                    requestResponseId, times, computingOption);

            ComputingRequest request2(requestResponseId, *drugModel, *drugTreatment, std::move(traits2));

            std::unique_ptr<ComputingResponse> response2 = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result2;
            result2 = component->compute(request2, response2);

            ASSERT_EQ(result2, ComputingStatus::Ok);


            const ComputedData* responseData = response->getData();
            ASSERT_TRUE(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);


            const ComputedData* responseData2 = response2->getData();
            ASSERT_TRUE(dynamic_cast<const SinglePointsData*>(responseData2) != nullptr);
            const SinglePointsData* resp2 = dynamic_cast<const SinglePointsData*>(responseData2);

            auto data = resp->getData();
            auto data2 = resp2->m_concentrations;
            ASSERT_DOUBLE_EQ(data[29].getConcentrations()[0][12], data2[0][0]);
            ASSERT_DOUBLE_EQ(data[29].getConcentrations()[0][24], data2[0][1]);

        }
*/

    // Delete all dynamically allocated objects
    delete component;
}
