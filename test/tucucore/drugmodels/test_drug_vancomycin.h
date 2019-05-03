/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DRUG_VANCOMYCIN_H
#define TEST_DRUG_VANCOMYCIN_H

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


static std::string vancomycin_tdd = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                                    <?xml-stylesheet href="drugsmodel.xsl" type="text/xsl" ?>
                                    <model xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="0.6" xsi:noNamespaceSchemaLocation="drug2.xsd">
                                        <!-- Drug history -->
                                        <history>
                                            <revisions>
                                                <revision>
                                                    <revisionAction>creation</revisionAction>
                                                    <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                    <institution>HEIG-VD // REDS</institution>
                                                    <email>yann.thoma@heig-vd.ch</email>
                                                    <date>2018-10-25</date>
                                                    <comments>
                                                        <comment lang="en">This file is based on the first version of
                                                            vaconcomycin validated by Sylvain Goutelle : ch.heig-vd.ezechiel.vancomycin.xml
                                                        </comment>
                                                    </comments>
                                                </revision>
                                            </revisions>
                                        </history>
                                        <!-- Drug description -->
                                        <head>
                                            <drug>
                                                <atcs>
                                                    <atc>J01XA01</atc>
                                                </atcs>
                                                <activeSubstances>
                                                    <activeSubstance>vancomycin</activeSubstance>
                                                </activeSubstances>
                                                <drugName>
                                                    <name lang="en">Vancomycin</name>
                                                    <name lang="fr">Vancomycine</name>
                                                </drugName>
                                                <drugDescription>
                                                    <desc lang="en">TODO : Add a description here</desc>
                                                </drugDescription>
                                                <tdmStrategy>
                                                    <text lang="en">TODO : Add a TDM strategy</text>
                                                </tdmStrategy>
                                            </drug>
                                            <study>
                                                <studyName>
                                                    <name lang="en">Population pharmacokinetic parameters of vancomycin in critically ill patients</name>
                                                </studyName>
                                                <studyAuthors>Llopis-Salvia, P. and Jiménez-Torres, N. V.</studyAuthors>
                                                <description>
                                                    <desc lang="en">...</desc>
                                                </description>
                                                <references>
                                                    <reference type='bibtex'>@article{llopis-salvia_population_2006,
                                                    title = {Population pharmacokinetic parameters of vancomycin in critically ill patients},
                                                    volume = {31},
                                                    url = {http://onlinelibrary.wiley.com/doi/10.1111/j.1365-2710.2006.00762.x/full},
                                                    number = {5},
                                                    urldate = {2014-10-23},
                                                    journal = {Journal of clinical pharmacy and therapeutics},
                                                    author = {Llopis-Salvia, P. and Jiménez-Torres, N. V.},
                                                    year = {2006},
                                                    pages = {447--454},
                                                    file = {Llopis-Salvia_Vancomycin.pdf:/home/rob/.zotero/zotero/iv5zqg2p.default/zotero/storage/B5WXR3BE/Llopis-Salvia_Vancomycin.pdf:application/pdf}
                                                }</reference>
                                                </references>
                                            </study>
                                            <comments/>
                                        </head>

                                        <!-- Drug data -->
                                        <drugModel>
                                            <drugId>vancomycin</drugId> <!-- id d'une des substances actives -->
                                            <drugModelId>ch.tucuxi.vancomycin</drugModelId>

                                            <domain>
                                                <description>
                                                    <desc lang="en">Adult ICU patients. (Age range: 18-77 years, Weight range: 50-130 kg, Creatinine range: 16-120 ml/min)</desc>
                                                </description>
                                                <constraints>
                                                    <constraint>
                                                        <constraintType>hard</constraintType>
                                                        <errorMessage>
                                                            <text lang="en">The age shall be greater or equal to 18</text>
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
                                                                <code><![CDATA[return (age > 17);
                                                                    ]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </checkOperation>
                                                        <comments/>
                                                    </constraint>
                                                        <constraint>
                                                            <constraintType>hard</constraintType>
                                                            <errorMessage>
                                                                <text lang="en">The age shall be greater or smaller or equal to 77</text>
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
                                                                    <code><![CDATA[return (age < 78);
                                                                        ]]>
                                                                    </code>
                                                                </softFormula>
                                                                <comments/>
                                                            </checkOperation>
                                                            <comments/>
                                                        </constraint>
                                                    <constraint>
                                                        <constraintType>hard</constraintType>
                                                        <errorMessage>
                                                            <text lang="en">The weight should be in the [50,130] margin</text>
                                                        </errorMessage>
                                                        <requiredCovariates>
                                                            <covariateId>bodyweight</covariateId>
                                                        </requiredCovariates>
                                                        <checkOperation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>bodyweight</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code><![CDATA[
                                                                    return ((bodyweight < 131) && (bodyweight > 49));
                                                                    ]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </checkOperation>
                                                        <comments/>
                                                    </constraint>
                                                </constraints>
                                            </domain>



                                            <!-- Drug model covariates -->
                                            <covariates>
                                                <covariate>
                                                    <covariateId>bodyweight</covariateId>
                                                    <name>
                                                        <name lang="en">Total Body Weight</name>
                                                        <name lang="fr">Poids total</name>
                                                    </name>
                                                    <description>
                                                        <desc lang="en">Total body weight of patient, in kilogramms</desc>
                                                        <desc lang="fr">Poids total du patient, en kilogramme</desc>
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
                                                        <standardValue>75</standardValue>
                                                    </covariateValue>
                                                    <validation> <!-- pourrait être une contrainte -->
                                                        <errorMessage><text lang="fr"></text></errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>bodyweight</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code><![CDATA[
                                                                    return ((bodyweight < 131) && (bodyweight > 49));
                                                                    ]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </operation>
                                                        <comments/>
                                                    </validation>
                                                    <comments>
                                                        <comment lang="en">About the mean value in the study population group A</comment>
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
                                                    <covariateId>creatinine</covariateId>
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
                                                    <validation>
                                                        <errorMessage>
                                                            <text lang='fr'>The SCR shall be positive.</text>
                                                        </errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>creatinine</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code>
                                                                    <![CDATA[return creatinine > 0.0;]]>
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
                                                    <covariateId>CLcr</covariateId>
                                                    <name>
                                                        <name lang="en">Creatinine clearance</name>
                                                        <name lang="fr">Clairance de la créatinine</name>
                                                    </name>
                                                    <description>
                                                        <desc lang="en">Creatinine clearance</desc>
                                                        <desc lang="fr">Clairance de la créatinine</desc>
                                                    </description>
                                                    <unit>ml/min</unit>
                                                    <covariateType>standard</covariateType>
                                                    <dataType>double</dataType>
                                                    <interpolationType>linear</interpolationType>
                                                    <refreshPeriod>
                                                      <unit>d</unit>
                                                      <value>1</value>
                                                    </refreshPeriod>
                                                    <covariateValue>
                                                        <standardValue>70</standardValue>
                                                        <aprioriComputation>
                                                          <hardFormula>OperationEGFRCockcroftGaultGeneral</hardFormula>
                                                          <comments/>
                                                        </aprioriComputation>
                                                    </covariateValue>
                                                    <validation> <!-- pourrait être une contrainte -->
                                                        <errorMessage><text lang="fr"></text></errorMessage>
                                                        <operation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>CLcr</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code><![CDATA[
                                                                    return true;
                                                                    ]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </operation>
                                                        <comments/>
                                                    </validation>
                                                    <comments>
                                                    <comment lang="en">About the mean value in the study population group A</comment>
                                                </comments>
                                                </covariate>
                                            </covariates>


                                            <activeMoieties>
                                                <activeMoiety>
                                                    <activeMoietyId>vancomycin</activeMoietyId>
                                                    <activeMoietyName>
                                                        <name lang="en">Vancomycin</name>
                                                    </activeMoietyName>
                                                    <unit>mg/l</unit>
                                                    <analyteIdList>
                                                        <analyteId>vancomycin</analyteId>
                                                    </analyteIdList>
                                                    <analytesToMoietyFormula>
                                                        <hardFormula>direct</hardFormula>
                                                        <comments/>
                                                    </analytesToMoietyFormula>
                                                    <!-- Drug targets -->
                                                    <targets>
                                                        <target>
                                                            <targetType>auc24DividedByMic</targetType>
                                                            <targetValues>
                                                                <unit>h</unit>
                                                                <min>
                                                                    <standardValue>300.0</standardValue>
                                                                </min>
                                                                <max>
                                                                    <standardValue>700.0</standardValue>
                                                                </max>
                                                                <best>
                                                                    <standardValue>400.0</standardValue>
                                                                </best>
                                                                <toxicityAlarm><standardValue>1000.0</standardValue></toxicityAlarm>
                                                                <inefficacyAlarm><standardValue>300.0</standardValue></inefficacyAlarm>
                                                                <mic>
                                                                    <unit>mg/l</unit>
                                                                    <micValue>
                                                                        <standardValue>1.0</standardValue>
                                                                    </micValue>
                                                                </mic>
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
                                                    <groupId>vancomycin</groupId>
                                                    <pkModelId>linear.2comp.macro</pkModelId>
                                                    <analytes>
                                                        <analyte>
                                                            <analyteId>vancomycin</analyteId>
                                                            <unit>mg/l</unit>
                                                            <molarMass>
                                                              <value>1449.3</value>
                                                                <unit>g/mol</unit>
                                                            </molarMass>
                                                            <description>
                                                                <desc lang="en"></desc>
                                                            </description> <!-- peut être vide -->

                                                            <errorModel> <!-- optional -->
                                                                <errorModelType>mixed</errorModelType>
                                                                <sigmas>
                                                                    <sigma>
                                                                        <standardValue>0.185</standardValue>
                                                                    </sigma>
                                                                    <sigma>
                                                                        <standardValue>0.239</standardValue>
                                                                    </sigma>
                                                                </sigmas>
                                                                <comments/>
                                                            </errorModel>
                                                            <comments/>
                                                        </analyte>
                                                    </analytes>

                                                    <!-- Drug parameters -->
                                                    <dispositionParameters>
                                                        <parameters>
                                                            <parameter>
                                                                <parameterId>CL</parameterId>
                                                                <unit>l/h</unit>
                                                                <parameterValue>
                                                                    <standardValue>3.505</standardValue>
                                                                    <aprioriComputation>

                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                                <input>
                                                                                    <id>CLcr</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code><![CDATA[
                                    CCR = CLcr;
                                    BW = bodyweight;
                                    theta_1 = 0.034;
                                    theta_2 = 0.015;

                                    TVCL = theta_1*CCR+theta_2*BW;
                                    return TVCL;
                                                             ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>proportional</bsvType> <!-- même chose que le modèle d'erreur -->
                                                                    <stdDevs>
                                                                        <stdDev>1</stdDev>
                                                                        <!-- <stdDev>0.292</stdDev> -->
                                                                    </stdDevs>
                                                                </bsv>
                                            <comments>
                                                    <comment lang="en">Typical clearance calculated for a patients with weight = 75 kg et CCR = 70 ml/min</comment>
                                                </comments>
                                                            </parameter>
                                                            <parameter>
                                                                <parameterId>V1</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>31.05</standardValue>
                                                                    <aprioriComputation>

                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code><![CDATA[
                                    theta_3 = 0.414;
                                    BW = bodyweight;

                                    TVV1 = theta_3*BW;
                                    return TVV1;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>proportional</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>1</stdDev>
                                                                        <!-- <stdDev>0.364</stdDev> -->
                                                                    </stdDevs>
                                                                </bsv>
                                            <comments>
                                                    <comment lang="en">Typical volume calculated for a patients with weight = 75 kg</comment>
                                                </comments>
                                                            </parameter>

                                                            <parameter>
                                                                <parameterId>Q</parameterId>
                                                                <unit>l/h</unit>
                                                                <parameterValue>
                                                                    <standardValue>7.48</standardValue>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>none</bsvType>
                                                                </bsv>
                                            <comments/>
                                                            </parameter>

                                                            <parameter>
                                                                <parameterId>V2</parameterId>
                                                                <unit>l</unit>
                                                                <parameterValue>
                                                                    <standardValue>99.0</standardValue>
                                                                    <aprioriComputation>

                                                                        <softFormula>
                                                                            <inputs>
                                                                                <input>
                                                                                    <id>bodyweight</id>
                                                                                    <type>double</type>
                                                                                </input>
                                                                            </inputs>
                                                                            <code><![CDATA[
                                    theta_5 = 1.32;
                                    BW = bodyweight;

                                    TVV2 = theta_5*BW;
                                    return TVV2;
                                                                                ]]>
                                                                            </code>
                                                                        </softFormula>
                                                                        <comments/>
                                                                    </aprioriComputation>
                                                                </parameterValue>
                                                                <bsv>
                                                                    <bsvType>proportional</bsvType>
                                                                    <stdDevs>
                                                                        <stdDev>1</stdDev>
                                                                        <!-- <stdDev>0.398</stdDev> -->
                                                                    </stdDevs>
                                                                </bsv>
                                            <comments>
                                                    <comment lang="en">Typical volume calculated for a patients with weight = 75 kg</comment>
                                                </comments>
                                                            </parameter>
                                                        </parameters>

                                                        <!-- elimination parameters correlations -->
                                                        <correlations />
                                                    </dispositionParameters>
                                                </analyteGroup>
                                            </analyteGroups>

                                            <!-- We can have various formulation and routes, and for each one a set of absorption parameters and available dosages -->

                                            <formulationAndRoutes default="id0">
                                                <formulationAndRoute>
                                                    <formulationAndRouteId>id0</formulationAndRouteId>
                                                    <formulation>parenteral solution</formulation><!-- dictionnaire -->
                                                    <administrationName>champ libre</administrationName>
                                                    <administrationRoute>intravenousDrip</administrationRoute> <!-- dictionnaire -->
                                                    <absorptionModelId>infusion</absorptionModelId>


                                                    <!-- Drug dosages -->
                                                    <dosages>
                                                    <analyteConversions>
                                                        <analyteConversion>
                                                            <analyteId>vancomycin</analyteId>
                                                            <factor>1</factor>
                                                        </analyteConversion>
                                                    </analyteConversions>

                                                    <availableDoses>
                                                        <unit>mg</unit>
                                                        <default>
                                                            <standardValue>1000</standardValue>
                                                        </default>
                                                        <rangeValues>
                                                            <from>
                                                                <standardValue>250</standardValue>
                                                            </from>
                                                            <to>
                                                                <standardValue>6000</standardValue>
                                                            </to>
                                                            <step>
                                                                <standardValue>250</standardValue>
                                                            </step>
                                                        </rangeValues>
                                                    </availableDoses>

                                                    <intervals>
                                                        <unit>h</unit>
                                                        <default>
                                                            <standardValue>12</standardValue>
                                                        </default>
                                                        <fixedValues>
                                                            <value>12</value>
                                                        </fixedValues>
                                                    </intervals>
                                                    <infusions>
                                                        <unit>min</unit>
                                                        <default>
                                                            <standardValue>120</standardValue>
                                                        </default>
                                                        <fixedValues>
                                                            <value>120</value>
                                                            <value>240</value>
                                                        </fixedValues>
                                                    </infusions>
                                                    <comments/>

                                                    </dosages>

                                                    <absorptionParameters/>

                                                </formulationAndRoute>
                                            </formulationAndRoutes>



                                            <timeConsiderations>
                                                <!-- Drug half-life -->
                                                <halfLife>
                                                    <unit>h</unit>
                                                    <duration>
                                                        <standardValue>6</standardValue>
                                                    </duration>
                                                    <multiplier>100</multiplier>
                                                    <comments>
                                                        <comment lang="en">Taken from this website: https://www.rxlist.com/vancomycin-injection-drug.htm</comment>
                                                    </comments>
                                                </halfLife>


                                                <outdatedMeasure>
                                                    <unit>d</unit>
                                                    <duration>
                                                        <standardValue>100</standardValue>
                                                    </duration>
                                                    <comments>
                                                        <comment lang="en">TODO : This value is not set now</comment>
                                                    </comments>
                                                </outdatedMeasure>
                                            </timeConsiderations>

                                            <!-- Drug general comments -->
                                            <comments>
                                            </comments>
                                        </drugModel>
                                    </model>

        )";

struct TestDrugVancomycin : public fructose::test_base<TestDrugVancomycin>
{
    TestDrugVancomycin() { }

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
                                 _route,
                                 Duration(),
                                 Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        std::unique_ptr<Tucuxi::Core::DosageTimeRange> sept2018(new Tucuxi::Core::DosageTimeRange(startSept2018, repeatedDose));


        _drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);
    }


    /// \brief Check that objects are correctly constructed by the constructor.
    void testVancomycin(const std::string& /* _testName */)
    {
        DrugModelImport importer;

        DrugModel *drugModel;

        importer.importFromString(drugModel, vancomycin_tdd);
//        importer.importFromFile(drugModel, "/home/ythoma/docs/ezechiel/git/dev/src/drugs2/ch.tucuxi.vancomycin.tdd");

        fructose_assert(drugModel != nullptr);


        IComputingService *component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert( component != nullptr);


        DrugTreatment *drugTreatment;
        const FormulationAndRoute route(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Infusion);


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

            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
            computingTraits->addTrait(std::move(traits));

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Ok);

            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
            for(std::size_t i = 0; i < responses.size(); i++) {
                fructose_assert(dynamic_cast<SinglePredictionResponse*>(responses[i].get()) != nullptr);
                const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[i].get());

                fructose_assert_eq(resp->getIds().size(), size_t{1});
                fructose_assert_eq(resp->getIds()[0], "vancomycin");

                //std::cout << "Population parameters : " << std::endl;
                //for (auto parameter : resp->getData().at(0).m_parameters) {
                //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
                //}
            }

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

            std::unique_ptr<ComputingTraits> computingTraits = std::make_unique<ComputingTraits>();
            computingTraits->addTrait(std::move(traits));

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(computingTraits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingResult result;
            result = component->compute(request, response);

            fructose_assert( result == ComputingResult::Ok);


            const std::vector<std::unique_ptr<SingleComputingResponse> > &responses = response.get()->getResponses();
            for(std::size_t i = 0; i < responses.size(); i++) {
                fructose_assert(dynamic_cast<SinglePredictionResponse*>(responses[i].get()) != nullptr);
                const SinglePredictionResponse *resp = dynamic_cast<SinglePredictionResponse*>(responses[i].get());

                fructose_assert_eq(resp->getIds().size(), size_t{1});
                fructose_assert_eq(resp->getIds()[0], "vancomycin");

                //std::cout << "A priori parameters : " << std::endl;
                //for (auto parameter : resp->getData().at(0).m_parameters) {
                //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
                //}
            }


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

#endif // TEST_DRUG_VANCOMYCIN_H
