//@@license@@

#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

using namespace Tucuxi::Core;

static const std::string tobramycin_tdd = R"(<?xml version="1.0" encoding="UTF-8"?>
                                    <model version='0.6' xsi:noNamespaceSchemaLocation='drug2.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                        <history>
                                            <revisions>
                                                <revision>
                                                    <revisionAction>creation</revisionAction>
                                                    <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                    <institution>HEIG-VD</institution>
                                                    <email>yann.thoma@heig-vd.ch</email>
                                                    <date>2018-11-07</date>
                                                    <comments/>
                                                </revision>
                                            </revisions>
                                        </history>
                                        <head>
                                            <drug>
                                                <atcs>
                                                    <atc>S01AA12</atc>
                                                </atcs>
                                                <activeSubstances>
                                                    <activeSubstance>tobramycin</activeSubstance>
                                                </activeSubstances>
                                                <drugName>
                                                    <name lang='en'>Tobramycin</name>
                                                </drugName>
                                                <drugDescription>
                                                    <desc lang='en'>Please describe the drug</desc>
                                                </drugDescription>
                                                <tdmStrategy>
                                                    <text lang='en'>Please add a TDM strategy</text>
                                                </tdmStrategy>
                                            </drug>
                                            <study>
                                                <studyName>
                                                    <name lang='en'>Population Pharmacokinetics of Tobramycin in Patients With and Without Cystic Fibrosis</name>
                                                </studyName>
                                                <studyAuthors>Stefanie Hennig, Joseph F. Standing, Christine E. Staatz, Alison H. Thomson</studyAuthors>
                                                <description>
                                                    <desc lang='en'>Please add a description</desc>
                                                </description>
                                                <references>
                                                    <reference type='bibtex'>
                                                        <![CDATA[
                                                          @Article{Hennig2013,
                                                          Title                    = {Population Pharmacokinetics of Tobramycin in Patients With and Without Cystic Fibrosis},
                                                          Author                   = {Hennig, Stefanie and Standing, Joseph F. and Staatz, Christine E. and Thomson, Alison H.},
                                                          Journal                  = {Clinical Pharmacokinetics},
                                                          Year                     = {2013},
                                                          Month                    = {Apr},
                                                          Number                   = {4},
                                                          Pages                    = {289--301},
                                                          Volume                   = {52},
                                                          Abstract                 = {While several studies have examined the pharmacokinetics of tobramycin in patients with cystic fibrosis (CF), there is no consensus on whether they differ in patients with and without CF. The objectives of this study were to identify covariates which explain pharmacokinetic variability and to examine whether having the disease CF in itself alters these relationships and drug dose requirements.},
                                                          Day                      = {01},
                                                          Doi                      = {10.1007/s40262-013-0036-y},
                                                          ISSN                     = {1179-1926},
                                                          Url                      = {https://doi.org/10.1007/s40262-013-0036-y}
                                                        }]]>
                                                    </reference>
                                                </references>
                                            </study>
                                            <comments/>
                                        </head>
                                        <drugModel>
                                            <drugId>tobramycin</drugId>
                                            <drugModelId>ch.tucuxi.tobramycin</drugModelId>
                                            <domain>
                                                <description>
                                                    <desc lang='en'>We shall define the domain.</desc>
                                                </description>
                                                <constraints>
                                                    <constraint>
                                                        <constraintType>hard</constraintType>
                                                        <errorMessage>
                                                            <text lang="en">The age shall be greater or equal to 0</text>
                                                        </errorMessage>
                                                        <requiredCovariates>
                                                            <covariateId>age</covariateId>
                                                        </requiredCovariates>
                                                        <checkOperation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>age</id>
                                                                        <type>int</type>
                                                                    </input>
                                                                </inputs>
                                                                <code><![CDATA[return (age > 0);
                                                                    ]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </checkOperation>
                                                        <comments/>
                                                    </constraint>
                                                  </constraints>
                                            </domain>
                                            <covariates>
                                                <covariate>
                                                    <covariateId>age</covariateId>
                                                    <name>
                                                        <name lang='en'>Age</name>
                                                    </name>
                                                    <description>
                                                        <desc lang='en'>Age in years</desc>
                                                    </description>
                                                    <unit>y</unit>
                                                    <covariateType>standard</covariateType>
                                                    <dataType>double</dataType>
                                                    <interpolationType>linear</interpolationType>
                                                    <refreshPeriod>
                                                      <unit>y</unit>
                                                      <value>1</value>
                                                    </refreshPeriod>
                                                    <covariateValue>
                                                        <standardValue>18</standardValue>
                                                    </covariateValue>
                                                    <validation>
                                                        <errorMessage>
                                                            <text lang='fr'>The age shall be positive.</text>
                                                        </errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>age</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code>
                                                                    <![CDATA[return age > 0.0;]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </operation>
                                                        <comments/>
                                                    </validation>
                                                    <comments>
                                                        <comment lang='en'>The validation still needs to be defined precisely.</comment>
                                                    </comments>
                                                </covariate>
                                                <covariate>
                                                    <covariateId>sex</covariateId>
                                                    <name>
                                                        <name lang='en'>Sex</name>
                                                    </name>
                                                    <description>
                                                        <desc lang='en'>Sex of the patient</desc>
                                                    </description>
                                                    <unit>y</unit>
                                                    <covariateType>standard</covariateType>
                                                    <dataType>double</dataType>
                                                    <interpolationType>direct</interpolationType>
                                                    <refreshPeriod>
                                                      <unit>d</unit>
                                                      <value>0</value>
                                                    </refreshPeriod>
                                                    <covariateValue>
                                                        <standardValue>0.5</standardValue>
                                                    </covariateValue>
                                                    <validation>
                                                        <errorMessage>
                                                            <text lang='fr'>The age shall be in [0,1].</text>
                                                        </errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>sex</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code>
                                                                    <![CDATA[if (sex < 0.0) return false;if (sex > 1.0) return false;return true;]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </operation>
                                                        <comments/>
                                                    </validation>
                                                    <comments>
                                                        <comment lang='en'>The validation still needs to be defined precisely.</comment>
                                                    </comments>
                                                </covariate>
                                                <covariate>
                                                    <covariateId>scr</covariateId>
                                                    <name>
                                                        <name lang='en'>serum creatinine concentration</name>
                                                    </name>
                                                    <description>
                                                        <desc lang='en'>Serum creatinine concentration.</desc>
                                                    </description>
                                                    <unit>umol/l</unit>
                                                    <covariateType>standard</covariateType>
                                                    <dataType>double</dataType>
                                                    <interpolationType>linear</interpolationType>
                                                    <refreshPeriod>
                                                      <unit>d</unit>
                                                      <value>1</value>
                                                    </refreshPeriod>
                                                    <covariateValue>
                                                        <standardValue>37.2</standardValue>
                                                    </covariateValue>
                                                    <validation>)"
                                          R"(<errorMessage>
                                                            <text lang='fr'>The SCR shall be positive.</text>
                                                        </errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>scr</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code>
                                                                    <![CDATA[return scr > 0.0;]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </operation>
                                                        <comments/>
                                                    </validation>
                                                    <comments>
                                                        <comment lang='en'>The validation still needs to be defined precisely.</comment>
                                                    </comments>
                                                </covariate>
                                                <covariate>
                                                    <covariateId>bodyweight</covariateId>
                                                    <name>
                                                        <name lang='en'>Body weight</name>
                                                    </name>
                                                    <description>
                                                        <desc lang='en'>Body weight, in Kg</desc>
                                                    </description>
                                                    <unit>kg</unit>
                                                    <covariateType>standard</covariateType>
                                                    <dataType>double</dataType>
                                                    <interpolationType>linear</interpolationType>
                                                    <refreshPeriod>
                                                      <unit>d</unit>
                                                      <value>10</value>
                                                    </refreshPeriod>
                                                    <covariateValue>
                                                        <standardValue>58.0</standardValue>
                                                    </covariateValue>
                                                    <validation>
                                                        <errorMessage>
                                                            <text lang='fr'>The body weight shall be positive.</text>
                                                        </errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>bodyweight</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code>
                                                                    <![CDATA[return bodyweight > 0.0;]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </operation>
                                                        <comments/>
                                                    </validation>
                                                    <comments>
                                                        <comment lang='en'>The validation shall be defined.</comment>
                                                    </comments>
                                                </covariate>
                                                <covariate>
                                                    <covariateId>height</covariateId>
                                                    <name>
                                                        <name lang='en'>Height</name>
                                                    </name>
                                                    <description>
                                                        <desc lang='en'>Height, in cm</desc>
                                                    </description>
                                                    <unit>cm</unit>
                                                    <covariateType>standard</covariateType>
                                                    <dataType>double</dataType>
                                                    <interpolationType>linear</interpolationType>
                                                    <refreshPeriod>
                                                      <unit>d</unit>
                                                      <value>10</value>
                                                    </refreshPeriod>
                                                    <covariateValue>
                                                        <standardValue>167.0</standardValue>
                                                    </covariateValue>
                                                    <validation>
                                                        <errorMessage>
                                                            <text lang='fr'>The height shall be positive.</text>
                                                        </errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>height</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code>
                                                                    <![CDATA[return height > 0.0;]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </operation>
                                                        <comments/>
                                                    </validation>
                                                    <comments>
                                                        <comment lang='en'>The validation shall be defined.</comment>
                                                    </comments>
                                                </covariate>
                                            </covariates>
                                            <activeMoieties>
                                                <activeMoiety>
                                                    <activeMoietyId>tobramycin</activeMoietyId>
                                                    <activeMoietyName>
                                                        <name lang='en'>Tobramycin</name>
                                                    </activeMoietyName>
                                                    <unit>mg/l</unit>
                                                    <analyteIdList>
                                                        <analyteId>tobramycin</analyteId>
                                                    </analyteIdList>
                                                    <analytesToMoietyFormula>
                                                        <hardFormula>direct</hardFormula>
                                                        <comments/>
                                                    </analytesToMoietyFormula>
                                                    <targets>
                                                        <target>
                                                            <targetType>auc24</targetType>
                                                            <targetValues>
                                                                <unit>mg*h/l</unit>
                                                                <min>
                                                                    <standardValue>70.0</standardValue>
                                                                </min>
                                                                <max>
                                                                    <standardValue>110.0</standardValue>
                                                                </max>
                                                                <best>
                                                                    <standardValue>100.0</standardValue>
                                                                </best>
                                                                <toxicityAlarm><standardValue>120.0</standardValue></toxicityAlarm>
                                                                <inefficacyAlarm><standardValue>60.0</standardValue></inefficacyAlarm>
                                                            </targetValues>
                                                            <comments>
                                                                <comment lang="en">Targets suggested by Philip Drennan</comment>
                                                            </comments>
                                                        </target>
                                                    </targets>
                                                </activeMoiety>
                                            </activeMoieties>
                                            <analyteGroups>
                                                <analyteGroup>
                                                    <groupId>tobramycin</groupId>
                                                    <pkModelId>linear.2comp.macro</pkModelId>
                                                    <analytes>
                                                        <analyte>
                                                            <analyteId>tobramycin</analyteId>
                                                            <unit>mg/l</unit>
                                                            <molarMass>
                                                                <value>467.515</value>
                                                                <unit>g/mol</unit>
                                                            </molarMass>
                                                            <description>
                                                                <desc lang='en'>Please write a description of Tobramycin.</desc>
                                                            </description>
                                                            <errorModel>
                                                                <errorModelType>proportional</errorModelType>
                                                                <sigmas>
                                                                    <sigma>
                                                                        <standardValue>0.204</standardValue>
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
                                                                    <standardValue>8.75</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>age</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>scr</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>height</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>)"
                                          R"(<code>
                                                                                <![CDATA[theta_CL = 0.0;
                                                                                theta_AGE = 0.0;
                                                                                ffm = 0.0;
                                                                                bmi = bodyweight / ((height / 100) * (height / 100));
                                                                                if (sex == 1) {
                                                                                    theta_CL = 9.4;
                                                                                    theta_AGE = -0.021;
                                                                                    ffm = 9270 * bodyweight / (6680 + 216 * bmi);
                                                                                }
                                                                                else {
                                                                                    theta_CL = 8.1;
                                                                                    theta_AGE = -0.010;
                                                                                    ffm = 9270 * bodyweight / (8780 + 244 * bmi);
                                                                                }
                                                                                theta_FFM = 0.952;
                                                                                theta_SCR = 0.222;

                                                                                F_AGE = 1 + theta_AGE * (age - 18.0);
                                                                                F_SCR = Math.pow(ffm / scr, theta_SCR);
                                                                                F_FFMCLQ2 = Math.pow(ffm / 70.0, theta_FFM);

                                                                                newCL = theta_CL * F_FFMCLQ2 * F_AGE * F_SCR;
                                                                                return newCL;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>proportional</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.259</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='fr'>The clearance shall be positive.</text>
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
                                                                <parameterId>V1</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>22.6</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                    <input>
                                                                                        <id>height</id>
                                                                                        <type>double</type>
                                                                                    </input>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[theta_V1 = 0.0;

                                                                                ffm = 0.0;
                                                                                bmi = bodyweight / ((height / 100) * (height / 100));
                                                                                if (sex == 1) {
                                                                                    theta_V1 = 25.1;
                                                                                    ffm = 9270 * bodyweight / (6680 + 216 * bmi);
                                                                                }
                                                                                else {
                                                                                    theta_V1 = 20.1;
                                                                                    ffm = 9270 * bodyweight / (8780 + 244 * bmi);
                                                                                }

                                                                                F_FFMV1V2 = ffm / 70.0;

                                                                                newV1 = theta_V1 * F_FFMV1V2;
                                                                                return newV1;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>proportional</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.152</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='fr'>The volume V1 shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V1</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return V1 > 0.0;
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
                                                                <parameterId>Q</parameterId>
                                                                <unit>l/h</unit>
                                                                <parameterValue>
                                                                    <standardValue>1.5</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>height</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[theta_Q = 1.5;
                                                                                ffm = 0.0;
                                                                                bmi = bodyweight / ((height / 100) * (height / 100));
                                                                                if (sex == 1) {
                                                                                    ffm = 9270 * bodyweight / (6680 + 216 * bmi);
                                                                                }
                                                                                else {
                                                                                    ffm = 9270 * bodyweight / (8780 + 244 * bmi);
                                                                                }
                                                                                theta_FFM = 0.952;

                                                                                F_FFMCLQ2 = Math.pow(ffm / 70.0, theta_FFM);

                                                                                newQ = theta_Q * F_FFMCLQ2;
                                                                                return newQ;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>proportional</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.418</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>)"
                                          R"(<errorMessage>
                                                                        <text lang='fr'>Q shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>Q</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return Q > 0.0;]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V2</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>10.0</standardValue>
                                                                    <aprioriComputation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>sex</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>height</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[theta_V2 = 10.0;
                                                                                ffm = 0.0;
                                                                                bmi = bodyweight / ((height / 100) * (height / 100));
                                                                                if (sex == 1) {
                                                                                    ffm = 9270 * bodyweight / (6680 + 216 * bmi);
                                                                                }
                                                                                else {
                                                                                    ffm = 9270 * bodyweight / (8780 + 244 * bmi);
                                                                                }
                                                                                theta_FFM = 0.952;

                                                                                F_FFMV1V2 = ffm / 70.0;

                                                                                newV2 = theta_V2 * F_FFMV1V2;
                                                                                return newV2;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>proportional</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>0.585</stdDev>
                                                                    </stdDevs>
                                                                </bsv>
                                                                <validation>
                                                                    <errorMessage>
                                                                        <text lang='fr'>V2 shall be positive.</text>
                                                                    </errorMessage>
                                                                    <operation>
                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>V2</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code>
                                                                                <![CDATA[return V2 > 0.0;]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </operation>
                                                                    <comments/>
                                                                </validation>
                                                                <comments/>
                                                            </parameter>
                                                        </parameters>
                                                        <correlations>
                                                            <correlation>
                                                                <param1>CL</param1>
                                                                <param2>V1</param2>
                                                                <value>0.841</value>
                                                                <comments/>
                                                            </correlation>
                                                            <correlation>
                                                                <param1>CL</param1>
                                                                <param2>Q</param2>
                                                                <value>0.711</value>
                                                                <comments/>
                                                            </correlation>
                                                            <correlation>
                                                                <param1>V1</param1>
                                                                <param2>Q</param2>
                                                                <value>0.475</value>
                                                                <comments/>
                                                            </correlation>
                                                        </correlations>
                                                    </dispositionParameters>
                                                </analyteGroup>
                                            </analyteGroups>
                                            <formulationAndRoutes default='id0'>
                                                <formulationAndRoute>
                                                    <formulationAndRouteId>id0</formulationAndRouteId>
                                                    <formulation>parenteral solution</formulation>
                                                    <administrationName>To be defined</administrationName>
                                                    <administrationRoute>intravenousDrip</administrationRoute>
                                                    <absorptionModel>infusion</absorptionModel>
                                                    <dosages>
                                                        <analyteConversions>
                                                            <analyteConversion>
                                                                <analyteId>tobramycin</analyteId>
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
                                                                    <standardValue>40</standardValue>
                                                                </from>
                                                                <to>
                                                                    <standardValue>1000</standardValue>
                                                                </to>
                                                                <step>
                                                                    <standardValue>40</standardValue>
                                                                </step>
                                                            </rangeValues>
                                                        </availableDoses>
                                                        <intervals>
                                                            <unit>h</unit>
                                                            <default>
                                                                <standardValue>24</standardValue>
                                                            </default>
                                                            <fixedValues>
                                                                <value>24</value>
                                                            </fixedValues>
                                                        </intervals>
                                                        <infusions>
                                                            <unit>m</unit>
                                                            <default>
                                                                <standardValue>30</standardValue>
                                                            </default>
                                                            <fixedValues>
                                                                <value>30</value>
                                                            </fixedValues>
                                                        </infusions>
                                                        <comments/>
                                                    </dosages>
                                                    <absorptionParameters/>
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
                                                        <comment lang='en'>To be defined</comment>
                                                    </comments>
                                                </halfLife>
                                                <outdatedMeasure>
                                                    <unit>d</unit>
                                                    <duration>
                                                        <standardValue>100</standardValue>
                                                    </duration>
                                                    <comments>
                                                        <comment lang='en'>To be defined</comment>
                                                    </comments>
                                                </outdatedMeasure>
                                            </timeConsiderations>
                                            <comments/>
                                        </drugModel>
                                    </model>

        )";

TEST(Core_TestDrugTobramycin, Tobramycin)
{
    DrugModelImport importer;

    std::unique_ptr<DrugModel> drugModel;

    auto status = importer.importFromString(drugModel, tobramycin_tdd);
    ASSERT_EQ(status, DrugModelImport::Status::Ok);

    ASSERT_TRUE(drugModel != nullptr);
}
