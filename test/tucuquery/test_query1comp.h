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


#ifndef TEST_QUERY1COMP_H
#define TEST_QUERY1COMP_H


#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

#include "tucucommon/utils.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/xmlnode.h"

#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"

#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucuquery/querycomputer.h"
#include "tucuquery/queryimport.h"
#include "tucuquery/querylogger.h"

#include "fructose/fructose.h"



static const std::string tdd = R"(<?xml version='1.0' encoding='UTF-8'?>
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
                <unit>mg/l</unit>
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
                        <unit>mg/l</unit>
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
            </formulationAndRoutes>
            <timeConsiderations>
                <halfLife>
                    <unit>h</unit>
                    <duration>
                        <standardValue>1</standardValue>
                    </duration>
                    <multiplier>120</multiplier>
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

static const std::string tqf = R"(<?xml version='1.0' ?>
<query version='1.0' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='xml_query.xsd'>
	<queryId>2_ch.tucuxi.virtualdrug.mod1</queryId>
	<clientId>2</clientId>
	<date>2023-09-13T10:53:20</date>
	<!-- Date the xml has been sent -->
	<language>en</language>
	<drugTreatment>
		<!-- All the information regarding the patient -->
		<patient>
			<covariates>
				<covariate>
					<covariateId>birthdate</covariateId>
					<date>2023-01-01T00:00:00</date>
					<value>2023-01-01T00:00:00</value>
					<unit/>
					<dataType>date</dataType>
					<nature>discrete</nature>
				</covariate>
				<covariate>
					<covariateId>sampling_group</covariateId>
					<date>2023-01-01T00:00:00</date>
					<value>1</value>
					<unit/>
					<dataType>int</dataType>
					<nature>discrete</nature>
				</covariate>
			</covariates>
		</patient>
		<!-- List of the drugs informations we have concerning the patient -->
		<drugs>
			<!-- All the information regarding the drug -->
			<drug>
				<drugId>virtualdrug</drugId>
				<activePrinciple>virtualdrug</activePrinciple>
				<brandName/>
				<atc/>
				<!-- All the information regarding the treatment -->
				<treatment>
					<dosageHistory>
						<dosageTimeRange>
							<start>2023-01-01T08:00:00</start>
							<end>2023-01-02T08:00:00</end>
							<dosage>
								<dosageLoop>
									<lastingDosage>
										<interval>24:0:0</interval>
										<dose>
											<value>120</value>
											<unit>mg</unit>
											<infusionTimeInMinutes>60.0</infusionTimeInMinutes>
										</dose>
										<formulationAndRoute>
											<formulation>oralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>oral</administrationRoute>
											<absorptionModel>extra</absorptionModel>
										</formulationAndRoute>
									</lastingDosage>
								</dosageLoop>
							</dosage>
						</dosageTimeRange>
					</dosageHistory>
				</treatment>
				<!-- Samples history -->
				<samples>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T09:00:00</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>2415.5</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T12:00:00</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>2288.7</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T16:00:00</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1157.0</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
                        <sampleDate>2023-01-02T08:00:00</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>68.782</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
				</samples>
			</drug>
		</drugs>
	</drugTreatment>
	<!-- List of the requests we want the server to take care of -->
	<requests>
		<request>
			<requestId>ch.tucuxi.virtualdrug.mod1.stats</requestId>
			<drugId>virtualdrug</drugId>
			<drugModelId>ch.tucuxi.virtualdrug.mod1</drugModelId>
			<predictionTraits>
				<computingOption>
                    <parametersType>aposteriori</parametersType>
					<compartmentOption>allActiveMoieties</compartmentOption>
					<retrieveStatistics>true</retrieveStatistics>
					<retrieveParameters>true</retrieveParameters>
					<retrieveCovariates>true</retrieveCovariates>
				</computingOption>
				<nbPointsPerHour>20</nbPointsPerHour>
				<dateInterval>
					<start>2023-01-01T08:00:00</start>
					<end>2023-01-02T08:00:00</end>
				</dateInterval>
			</predictionTraits>
		</request>
		<request>
			<requestId>ch.tucuxi.virtualdrug.mod1.pred</requestId>
			<drugId>virtualdrug</drugId>
			<drugModelId>ch.tucuxi.virtualdrug.mod1</drugModelId>
			<predictionAtTimesTraits>
				<computingOption>
                    <parametersType>aposteriori</parametersType>
					<compartmentOption>allActiveMoieties</compartmentOption>
					<retrieveStatistics>true</retrieveStatistics>
					<retrieveParameters>true</retrieveParameters>
					<retrieveCovariates>true</retrieveCovariates>
				</computingOption>
				<dates>
					<date>2023-01-01T09:00:00</date>
                    <date>2023-01-01T12:00:00</date>
					<date>2023-01-01T16:00:00</date>
                    <date>2023-01-02T08:00:00</date>
				</dates>
			</predictionAtTimesTraits>
		</request>
	</requests>
</query>)";



struct TestQuery1Comp : public fructose::test_base<TestQuery1Comp>
{
    void testSingle(const std::string& /*_testName*/)
    {
        Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();



        auto drugModelRepository =
                dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

        pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);

        Tucuxi::Core::DrugModelImport importer;

        std::unique_ptr<Tucuxi::Core::DrugModel> drugModel;

        auto status = importer.importFromString(drugModel, tdd);
        fructose_assert_eq(status, Tucuxi::Core::DrugModelImport::Status::Ok);

        fructose_assert(drugModel != nullptr);

        drugModelRepository->addDrugModel(drugModel.get());

        std::string queryLogPath = "./log.txt";
        auto* queryLogger =
                dynamic_cast<Tucuxi::Query::QueryLogger*>(Tucuxi::Query::QueryLogger::createComponent(queryLogPath));

        pCmpMgr->registerComponent("QueryLogger", queryLogger);

        Tucuxi::Query::QueryComputer computer;
        Tucuxi::Query::ComputingQueryResponse response;
        computer.compute(tqf, response);

        fructose_assert(response.getRequestResponses().size() == 2);

        auto& r1 = response.getRequestResponses()[0];
        auto& r2 = response.getRequestResponses()[1];

        auto resp1 = r1.m_computingResponse.get();
        auto resp2 = r2.m_computingResponse.get();

        const auto* d1 = dynamic_cast<const Tucuxi::Core::SinglePredictionData*>(resp1->getData());
        const auto* d2 = dynamic_cast<const Tucuxi::Core::SinglePointsData*>(resp2->getData());

        //fructose_assert_eq(d2->m_concentrations[0].size(), 4);
        fructose_assert_double_eq(d1->getData()[0].m_concentrations[0][20], d2->m_concentrations[0][0]);
    }
};



#endif // TEST_QUERY1COMP_H
