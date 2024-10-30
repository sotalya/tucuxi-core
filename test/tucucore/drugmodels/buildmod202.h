#ifndef BUILDMOD202_H
#define BUILDMOD202_H




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


#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

using namespace Tucuxi::Core;

static const std::string mod202_tdd = R"(<?xml version="1.0" encoding="UTF-8"?>
<model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
    <history>
        <revisions>
            <revision>
                <revisionAction>creation</revisionAction>
                <revisionAuthorName>Anne Ravix</revisionAuthorName>
                <institution>chuv</institution>
                <email>anne.ravix@chuv.ch</email>
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
                                                                                Model 1: 1CMT - IV - MM elimination - IIV on Vmax VC KM - Proportional error</desc>
            </description>
            <references/>
        </study>
        <comments/>
    </head>
    <drugModel>
        <drugId>virtualdrug</drugId>
        <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
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
                <pkModelId>michaelismenten.1comp</pkModelId>
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
                            <parameterId>Vmax</parameterId>
                            <unit>µg/h</unit>
                            <parameterValue>
                                <standardValue>10</standardValue>
                            </parameterValue>
                            <bsv>
                                <bsvType>exponential</bsvType>
                                <stdDevs>
                                    <stdDev>0.45</stdDev>
                                </stdDevs>
                            </bsv>
                            <validation>
                                <errorMessage>
                                    <text lang='en'>Vmax must be positive</text>
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
                        <parameter>
                            <parameterId>Km</parameterId>
                            <unit>mg/l</unit>
                            <parameterValue>
                                <standardValue>3500</standardValue>
                            </parameterValue>
                            <bsv>
                                <bsvType>exponential</bsvType>
                                <stdDevs>
                                    <stdDev>0.45</stdDev>
                                </stdDevs>
                            </bsv>
                            <validation>
                                <errorMessage/>
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
                                            return Km > 0.0 ;
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
                <formulation>parenteralSolution</formulation>
                <administrationName>foo bar</administrationName>
                <administrationRoute>intravenousDrip</administrationRoute>
                <absorptionModel>infusion</absorptionModel>
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
                    <availableInfusions>
                        <unit>h</unit>
                        <default>
                            <standardValue>1</standardValue>
                        </default>
                        <fixedValues />
                    </availableInfusions>
                    <comments/>
                </dosages>
                <absorptionParameters>
                    <parameterSetAnalyteGroup>
                        <analyteGroupId>virtualdrug</analyteGroupId>
                        <absorptionModel>infusion</absorptionModel>
                        <parameterSet>
                            <parameters />
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
                <multiplier>80000</multiplier>
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
</model>
        )";


class BuildMod202
{
public:
    BuildMod202() {}

    std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel()
    {
        DrugModelImport importer;

        std::unique_ptr<DrugModel> drugModel;

        auto status = importer.importFromString(drugModel, mod202_tdd);

        return drugModel;
    }
};


// 52-minute infusion time
const std::string tqf52 = R"(<?xml version="1.0" ?>
<query version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="xml_query.xsd">
	<queryId>4_ch.tucuxi.virtualdrug.mod202</queryId>
	<clientId>4</clientId>
	<date>2024-04-09T10:12:43</date>
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
											<infusionTimeInMinutes>52.0</infusionTimeInMinutes>
										</dose>
										<formulationAndRoute>
											<formulation>parenteralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>intravenousDrip</administrationRoute>
											<absorptionModel>infusion</absorptionModel>
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
						<sampleId>c0</sampleId>
						<sampleDate>2023-01-01T08:10:30</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>272.0</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T09:10:12</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1728.8</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T12:52:48</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1718.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T16:02:24</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1709.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-02T04:13:12</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1675.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
				</samples>
			</drug>
		</drugs>
	</drugTreatment>
	)"
                          R"(
	<!-- List of the requests we want the server to take care of -->
	<requests>
		<request>
		    <requestId>aposteriori_2</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>100</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
		          <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
		
		<request>
		    <requestId>aposteriori_20</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>10</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
		          <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
		
		<request>
		    <requestId>aposteriori_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionAtSampleTimesTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		    </predictionAtSampleTimesTraits>
		</request>

		<request>
		    <requestId>apriori_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>apriori</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>20</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
			  <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>

		<request>
		    <requestId>population_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>20</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
			  <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
	</requests>
</query>
)";


// 65-minute infusion time
const std::string tqf65 = R"(<?xml version="1.0" ?>
<query version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="xml_query.xsd">
	<queryId>4_ch.tucuxi.virtualdrug.mod202</queryId>
	<clientId>4</clientId>
	<date>2024-04-09T10:12:43</date>
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
											<infusionTimeInMinutes>65.0</infusionTimeInMinutes>
										</dose>
										<formulationAndRoute>
											<formulation>parenteralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>intravenousDrip</administrationRoute>
											<absorptionModel>infusion</absorptionModel>
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
						<sampleId>c0</sampleId>
						<sampleDate>2023-01-01T08:10:30</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>272.0</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T09:10:12</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1728.8</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T12:52:48</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1718.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T16:02:24</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1709.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-02T04:13:12</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1675.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
				</samples>
			</drug>
		</drugs>
	</drugTreatment>
	)"
                          R"(
	<!-- List of the requests we want the server to take care of -->
	<requests>
		<request>
		    <requestId>aposteriori_2</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>100</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
		          <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
		
		<request>
		    <requestId>aposteriori_20</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>10</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
		          <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
		
		<request>
		    <requestId>aposteriori_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionAtSampleTimesTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		    </predictionAtSampleTimesTraits>
		</request>

		<request>
		    <requestId>apriori_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>apriori</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>20</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
			  <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>

		<request>
		    <requestId>population_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>20</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
			  <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
	</requests>
</query>
)";


// 60-minute infusion time
const std::string tqf60 = R"(<?xml version="1.0" ?>
<query version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="xml_query.xsd">
	<queryId>4_ch.tucuxi.virtualdrug.mod202</queryId>
	<clientId>4</clientId>
	<date>2024-04-09T10:12:43</date>
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
											<formulation>parenteralSolution</formulation>
											<administrationName>foo bar</administrationName>
											<administrationRoute>intravenousDrip</administrationRoute>
											<absorptionModel>infusion</absorptionModel>
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
						<sampleId>c0</sampleId>
						<sampleDate>2023-01-01T08:10:30</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>272.0</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T09:10:12</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1728.8</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T12:52:48</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1718.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-01T16:02:24</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1709.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
					<sample>
						<sampleId>c1</sampleId>
						<sampleDate>2023-01-02T04:13:12</sampleDate>
						<concentrations>
							<concentration>
								<analyteId>virtualdrug</analyteId>
								<value>1675.3</value>
								<unit>ug/l</unit>
							</concentration>
						</concentrations>
					</sample>
				</samples>
			</drug>
		</drugs>
	</drugTreatment>
	)"
                          R"(
	<!-- List of the requests we want the server to take care of -->
	<requests>
		<request>
		    <requestId>aposteriori_2</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>100</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
		          <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
		
		<request>
		    <requestId>aposteriori_20</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>10</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
		          <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
		
		<request>
		    <requestId>aposteriori_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionAtSampleTimesTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		    </predictionAtSampleTimesTraits>
		</request>

		<request>
		    <requestId>apriori_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>apriori</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>20</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
			  <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>

		<request>
		    <requestId>population_1</requestId>
		    <drugId>virtualdrug</drugId>
		    <drugModelId>ch.tucuxi.virtualdrug.mod202</drugModelId>
		    <predictionTraits>
		      <computingOption>
		        <parametersType>population</parametersType>
		        <compartmentOption>allActiveMoieties</compartmentOption>
		        <retrieveStatistics>true</retrieveStatistics>
		        <retrieveParameters>true</retrieveParameters>
		        <retrieveCovariates>true</retrieveCovariates>
		      </computingOption>
		      <nbPointsPerHour>20</nbPointsPerHour>
		      <dateInterval>
		          <start>2023-01-01T08:00:00</start>
			  <end>2023-01-03T08:00:00</end>
		      </dateInterval>
		    </predictionTraits>
		</request>
	</requests>
</query>
)";



#endif // BUILDMOD202_H
