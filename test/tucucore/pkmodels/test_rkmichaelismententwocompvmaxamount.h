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


#ifndef TEST_RKMICHAELISMENTTWOCOMPVMAXAMOUNT_H
#define TEST_RKMICHAELISMENTTWOCOMPVMAXAMOUNT_H


#include <chrono>
#include <iostream>
#include <memory>

#include <date/date.h>

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/drugtreatment.h"

#include "fructose/fructose.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;


static const std::string test_mm_2comp_vmaxamount_tdd = R"(
                                                          <?xml version="1.0" encoding="UTF-8"?>
                                                          <model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
                                                              <history>
                                                                  <revisions>
                                                                      <revision>
                                                                          <revisionAction>creation</revisionAction>
                                                                          <revisionAuthorName>Goutelle</revisionAuthorName>
                                                                          <institution>HCL</institution>
                                                                          <email>sylvain.goutelle@chu-lyon.fr</email>
                                                                          <date>2021-07-15</date>
                                                                          <comments/>
                                                                      </revision>
                                                                      <revision>
                                                                          <revisionAction>modification</revisionAction>
                                                                          <revisionAuthorName>Yann Thoma</revisionAuthorName>
                                                                          <institution>HEIG-VD</institution>
                                                                          <email>yann.thoma@heig-vd.ch</email>
                                                                          <date>2021-07-28</date>
                                                                          <comments>
                                                                              <comment lang='en'>Modified Ids to be only lowercases. Modified F to be in [0,1] instead of [0,100].</comment>
                                                                          </comments>
                                                                      </revision>
                                                                      <revision>
                                                                          <revisionAction>modification</revisionAction>
                                                                          <revisionAuthorName>Sylvain Goutelle</revisionAuthorName>
                                                                          <institution>HCL</institution>
                                                                          <email>sylvain.goutelle@chu-lyon.fr</email>
                                                                          <date>2023-02-24</date>
                                                                          <comments>
                                                                              <comment lang='en'>Michaelis Menten Vmax expressed in mg/h as in the original publication</comment>
                                                                          </comments>
                                                                      </revision>
                                                                  </revisions>
                                                              </history>
                                                              <head>
                                                                  <drug>
                                                                      <atcs>
                                                                          <atc>J02AC03</atc>
                                                                      </atcs>
                                                                      <activeSubstances>
                                                                          <activeSubstance>voriconazole</activeSubstance>
                                                                      </activeSubstances>
                                                                      <drugName>
                                                                          <name lang='en'>Voriconazole</name>
                                                                      </drugName>
                                                                      <drugDescription>
                                                                          <desc lang='en'>Antifungal agent</desc>
                                                                      </drugDescription>
                                                                      <tdmStrategy>
                                                                          <text lang='en'>Trough monitoring</text>
                                                                      </tdmStrategy>
                                                                  </drug>
                                                                  <study>
                                                                      <studyName>
                                                                          <name lang='en'>Understanding variability with voriconazole using a population
                                                                                                                                                                                                                                                                          pharmacokinetic approach: implications for optimal dosing</name>
                                                                      </studyName>
                                                                      <studyAuthors>Dolton et al.</studyAuthors>
                                                                      <description>
                                                                          <desc lang='en'>Non-linear mixed effects modelling (NONMEM) was undertaken of six voriconazole studies in healthy
                                                                                                                                                                                                                                                                          volunteers and patients.</desc>
                                                                      </description>
                                                                      <references/>
                                                                  </study>
                                                                  <comments/>
                                                              </head>
                                                              <drugModel>
                                                                  <drugId>voriconazole</drugId>
                                                                  <drugModelId>ch.tucuxi.voriconazole.dolton2014_2023</drugModelId>
                                                                  <domain>
                                                                      <description>
                                                                          <desc lang='en'>Healthy volunteers and patients</desc>
                                                                      </description>
                                                                      <constraints />
                                                                  </domain>
                                                                  <covariates>
                                                                      <covariate>
                                                                          <covariateId>CYP2C19LoF</covariateId>
                                                                          <covariateName>
                                                                              <name lang='en'>CYP2C19 loss of function allele</name>
                                                                          </covariateName>
                                                                          <description>
                                                                              <desc lang='en'>CYP2C19 loss of function allele</desc>
                                                                          </description>
                                                                          <unit>-</unit>
                                                                          <covariateType>standard</covariateType>
                                                                          <dataType>int</dataType>
                                                                          <interpolationType>direct</interpolationType>
                                                                          <covariateValue>
                                                                              <standardValue>0</standardValue>
                                                                          </covariateValue>
                                                                          <validation>
                                                                              <errorMessage>
                                                                                  <text lang='en'>Should be 0 (no mutation) or 1 (one or more mutation)</text>
                                                                              </errorMessage>
                                                                              <operation>
                                                                                  <softFormula>
                                                                                      <inputs>
                                                                                          <input>
                                                                                              <id>CYP2C19LoF</id>
                                                                                              <type>double</type>
                                                                                          </input>
                                                                                      </inputs>
                                                                                      <code>
                                                                                          <![CDATA[
                                                                                          return true;
                                                                                          ]]>
                                                                                      </code>
                                                                                  </softFormula>
                                                                                  <comments/>
                                                                              </operation>
                                                                              <comments/>
                                                                          </validation>
                                                                          <comments>
                                                                              <comment lang='en'>None</comment>
                                                                          </comments>
                                                                      </covariate>
                                                                      <covariate>
                                                                          <covariateId>INHIB</covariateId>
                                                                          <covariateName>
                                                                              <name lang='en'>Strong CYP2C19 inhibitor</name>
                                                                          </covariateName>
                                                                          <description>
                                                                              <desc lang='en'>Strong CYP2C19 inhibitor</desc>
                                                                          </description>
                                                                          <unit>-</unit>
                                                                          <covariateType>standard</covariateType>
                                                                          <dataType>int</dataType>
                                                                          <interpolationType>direct</interpolationType>
                                                                          <covariateValue>
                                                                              <standardValue>0</standardValue>
                                                                          </covariateValue>
                                                                          <validation>
                                                                              <errorMessage>
                                                                                  <text lang='en'>Should be 0 (no CYP2C19 inhibitor co-administered) or 1.</text>
                                                                              </errorMessage>
                                                                              <operation>
                                                                                  <softFormula>
                                                                                      <inputs>
                                                                                          <input>
                                                                                              <id>INHIB</id>
                                                                                              <type>double</type>
                                                                                          </input>
                                                                                      </inputs>
                                                                                      <code>
                                                                                          <![CDATA[
                                                                                          return true;
                                                                                          ]]>
                                                                                      </code>
                                                                                  </softFormula>
                                                                                  <comments/>
                                                                              </operation>
                                                                              <comments/>
                                                                          </validation>
                                                                          <comments>
                                                                              <comment lang='en'>None</comment>
                                                                          </comments>
                                                                      </covariate>)"
                                                        R"(
                                                                      <covariate>
                                                                          <covariateId>InducerSt</covariateId>
                                                                          <covariateName>
                                                                              <name lang='en'>Strong CYP2C19 inducer</name>
                                                                          </covariateName>
                                                                          <description>
                                                                              <desc lang='en'>Strong CYP2C19 inducer</desc>
                                                                          </description>
                                                                          <unit>-</unit>
                                                                          <covariateType>standard</covariateType>
                                                                          <dataType>int</dataType>
                                                                          <interpolationType>linear</interpolationType>
                                                                          <refreshPeriod>
                                                                              <unit>d</unit>
                                                                              <value>30</value>
                                                                          </refreshPeriod>
                                                                          <covariateValue>
                                                                              <standardValue>0</standardValue>
                                                                          </covariateValue>
                                                                          <validation>
                                                                              <errorMessage>
                                                                                  <text lang='en'>Should be 0 (no strong inducer co-administered) or 1.</text>
                                                                              </errorMessage>
                                                                              <operation>
                                                                                  <softFormula>
                                                                                      <inputs>
                                                                                          <input>
                                                                                              <id>InducerSt</id>
                                                                                              <type>double</type>
                                                                                          </input>
                                                                                      </inputs>
                                                                                      <code>
                                                                                          <![CDATA[
                                                                                          return true;
                                                                                          ]]>
                                                                                      </code>
                                                                                  </softFormula>
                                                                                  <comments/>
                                                                              </operation>
                                                                              <comments/>
                                                                          </validation>
                                                                          <comments>
                                                                              <comment lang='en'>None</comment>
                                                                          </comments>
                                                                      </covariate>
                                                        <covariate>
                                                            <covariateId>InducerMod</covariateId>
                                                            <covariateName>
                                                                <name lang='en'>Moderate CYP2C19 inducer</name>
                                                            </covariateName>
                                                            <description>
                                                                <desc lang='en'>Moderate CYP2C19 inducer</desc>
                                                            </description>
                                                            <unit>-</unit>
                                                            <covariateType>standard</covariateType>
                                                            <dataType>double</dataType>
                                                            <interpolationType>linear</interpolationType>
                                                            <refreshPeriod>
                                                                <unit>d</unit>
                                                                <value>30</value>
                                                            </refreshPeriod>
                                                            <covariateValue>
                                                                <standardValue>0</standardValue>
                                                            </covariateValue>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Should be 0 (no moderate CYP2C19 inducer co-administered) or 1.</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>InducerMod</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return true;
                                                                            ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </operation>
                                                                <comments/>
                                                            </validation>
                                                            <comments>
                                                                <comment lang='en'>None</comment>
                                                            </comments>
                                                        </covariate>
                                                        <covariate>
                                                            <covariateId>covF</covariateId>
                                                            <covariateName>
                                                                <name lang='en'>covF</name>
                                                            </covariateName>
                                                            <description>
                                                                <desc lang='en'>covF</desc>
                                                            </description>
                                                            <unit>-</unit>
                                                            <covariateType>standard</covariateType>
                                                            <dataType>double</dataType>
                                                            <interpolationType>linear</interpolationType>
                                                            <refreshPeriod>
                                                                <unit>h</unit>
                                                                <value>1</value>
                                                            </refreshPeriod>
                                                            <covariateValue>
                                                                <standardValue>0.94</standardValue>
                                                            </covariateValue>
                                                            <validation>
                                                                <errorMessage>
                                                                    <text lang='en'>Should be positive</text>
                                                                </errorMessage>
                                                                <operation>
                                                                    <softFormula>
                                                                        <inputs>
                                                                            <input>
                                                                                <id>covF</id>
                                                                                <type>double</type>
                                                                            </input>
                                                                        </inputs>
                                                                        <code>
                                                                            <![CDATA[
                                                                            return covF >= 0.0;
                                                                            ]]>
                                                                        </code>
                                                                    </softFormula>
                                                                    <comments/>
                                                                </operation>
                                                                <comments/>
                                                            </validation>
                                                            <comments>
                                                                <comment lang='en'>None</comment>
                                                            </comments>
                                                        </covariate>
                                                                  </covariates>
                                                                  <activeMoieties>
                                                                      <activeMoiety>
                                                                          <activeMoietyId>voriconazole</activeMoietyId>
                                                                          <activeMoietyName>
                                                                              <name lang='en'>Voriconazole</name>
                                                                          </activeMoietyName>
                                                                          <unit>mg/l</unit>
                                                                          <analyteIdList>
                                                                              <analyteId>voriconazole</analyteId>
                                                                          </analyteIdList>
                                                                          <analytesToMoietyFormula>
                                                                              <hardFormula>direct</hardFormula>
                                                                              <comments/>
                                                                          </analytesToMoietyFormula>
                                                                          <targets>
                                                                              <target>
                                                                                  <targetType>residual</targetType>
                                                                                  <targetValues>
                                                                                      <unit>mg/l</unit>
                                                                                      <min>
                                                                                          <standardValue>2</standardValue>
                                                                                      </min>
                                                                                      <max>
                                                                                          <standardValue>6</standardValue>
                                                                                      </max>
                                                                                      <best>
                                                                                          <standardValue>3</standardValue>
                                                                                      </best>
                                                                                      <toxicityAlarm>
                                                                                          <standardValue>7</standardValue>
                                                                                      </toxicityAlarm>
                                                                                      <inefficacyAlarm>
                                                                                          <standardValue>1</standardValue>
                                                                                      </inefficacyAlarm>
                                                                                  </targetValues>
                                                                                  <comments/>
                                                                              </target>
                                                                          </targets>
                                                                      </activeMoiety>
                                                                  </activeMoieties>)"
                                                        R"(
                                                                  <analyteGroups>
                                                                      <analyteGroup>
                                                                          <groupId>voriconazole</groupId>
                                                                          <pkModelId>michaelismenten.2comp.vmaxamount.macro</pkModelId>
                                                                          <analytes>
                                                                              <analyte>
                                                                                  <analyteId>voriconazole</analyteId>
                                                                                  <unit>mg/l</unit>
                                                                                  <molarMass>
                                                                                      <value>349.3</value>
                                                                                      <unit>g/mol</unit>
                                                                                  </molarMass>
                                                                                  <description>
                                                                                      <desc lang='en'>Voriconazole</desc>
                                                                                  </description>
                                                                                  <errorModel>
                                                                                      <errorModelType>proportional</errorModelType>
                                                                                      <sigmas>
                                                                                          <sigma>
                                                                                              <standardValue>0.338</standardValue>
                                                                                          </sigma>
                                                                                      </sigmas>
                                                                                      <comments>
                                                                                          <comment lang=''>CV% = 33.8 so sigma = 0.338² = 0.114. Additive error ignored, very low (0.005 mg/L)</comment>
                                                                                      </comments>
                                                                                  </errorModel>
                                                                                  <comments/>
                                                                              </analyte>
                                                                          </analytes>
                                                                          <dispositionParameters>
                                                                              <parameters>
                                                                                  <parameter>
                                                                                      <parameterId>Km</parameterId>
                                                                                      <unit>mg/l</unit>
                                                                                      <parameterValue>
                                                                                          <standardValue>3.33</standardValue>
                                                                                      </parameterValue>
                                                                                      <bsv>
                                                                                          <bsvType>none</bsvType>
                                                                                          <stdDevs />
                                                                                      </bsv>
                                                                                      <validation>
                                                                                          <errorMessage>
                                                                                              <text lang='fr'>NA</text>
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
                                                                                      <unit>mg/h</unit>
                                                                                      <parameterValue>
                                                                                          <standardValue>43.9</standardValue>
                                                                                          <aprioriComputation>
                                                                                              <softFormula>
                                                                                                  <inputs>
                                                                                                      <input>
                                                                                                          <id>CYP2C19LoF</id>
                                                                                                          <type>double</type>
                                                                                                      </input>
                                                                                                      <input>
                                                                                                          <id>INHIB</id>
                                                                                                          <type>double</type>
                                                                                                      </input>
                                                                                                      <input>
                                                                                                          <id>InducerSt</id>
                                                                                                          <type>double</type>
                                                                                                      </input>
                                                                                                      <input>
                                                                                                          <id>InducerMod</id>
                                                                                                          <type>double</type>
                                                                                                      </input>
                                                                                                  </inputs>
                                                                                                  <code>
                                                                                                      <![CDATA[
                                                                                                      theta_1 = 43.9; theta_2 = -0.412; theta_3 = -0.429; theta_4 = 2.03; theta_5 = 0.5; TVVmax = (theta_1 * (1 + (CYP2C19LoF * theta_2) + (INHIB * theta_3) + (InducerSt * theta_4) + (InducerMod * theta_5))); return TVVmax;
                                                                                                      ]]>
                                                                                                  </code>
                                                                                              </softFormula>
                                                                                              <comments/>
                                                                                          </aprioriComputation>
                                                                                      </parameterValue>
                                                                                      <bsv>
                                                                                          <bsvType>none</bsvType>
                                                                                          <stdDevs />
                                                                                      </bsv>
                                                                                      <validation>
                                                                                          <errorMessage>
                                                                                              <text lang='fr'>NA</text>
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
                                                                                  </parameter>)"
                                                        R"(
                                                                                  <parameter>
                                                                                      <parameterId>V1</parameterId>
                                                                                      <unit>l</unit>
                                                                                      <parameterValue>
                                                                                          <standardValue>27.1</standardValue>
                                                                                      </parameterValue>
                                                                                      <bsv>
                                                                                          <bsvType>lognormal</bsvType>
                                                                                          <stdDevs>
                                                                                              <stdDev>0.834</stdDev>
                                                                                          </stdDevs>
                                                                                      </bsv>
                                                                                      <validation>
                                                                                          <errorMessage>
                                                                                              <text lang='fr'>NA</text>
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
                                                                                                      <![CDATA[
                                                                                                      return V1 > 0.0;
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
                                                                                      <parameterId>V2</parameterId>
                                                                                      <unit>l</unit>
                                                                                      <parameterValue>
                                                                                          <standardValue>127</standardValue>
                                                                                      </parameterValue>
                                                                                      <bsv>
                                                                                          <bsvType>lognormal</bsvType>
                                                                                          <stdDevs>
                                                                                              <stdDev>0.381</stdDev>
                                                                                          </stdDevs>
                                                                                      </bsv>
                                                                                      <validation>
                                                                                          <errorMessage>
                                                                                              <text lang='fr'>NA</text>
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
                                                                                                      <![CDATA[
                                                                                                      return V2 > 0.0;
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
                                                                                      <unit>L/h</unit>
                                                                                      <parameterValue>
                                                                                          <standardValue>35.1</standardValue>
                                                                                      </parameterValue>
                                                                                      <bsv>
                                                                                          <bsvType>lognormal</bsvType>
                                                                                          <stdDevs>
                                                                                              <stdDev>0.374</stdDev>
                                                                                          </stdDevs>
                                                                                      </bsv>
                                                                                      <validation>
                                                                                          <errorMessage>
                                                                                              <text lang='fr'>NA</text>
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
                                                                                                      <![CDATA[
                                                                                                      return Q > 0.0;
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
                                                            <formulationAndRouteId>id2</formulationAndRouteId>
                                                            <formulation>parenteralSolution</formulation>
                                                            <administrationName>something</administrationName>
                                                            <administrationRoute>intravenousBolus</administrationRoute>
                                                            <absorptionModel>bolus</absorptionModel>
                                                            <dosages>
                                                                <analyteConversions>
                                                                    <analyteConversion>
                                                                        <analyteId>voriconazole</analyteId>
                                                                        <factor>1</factor>
                                                                    </analyteConversion>
                                                                </analyteConversions>
                                                                <availableDoses>
                                                                    <unit>mg</unit>
                                                                    <default>
                                                                        <standardValue>200</standardValue>
                                                                    </default>
                                                                    <rangeValues>
                                                                        <from>
                                                                            <standardValue>50</standardValue>
                                                                        </from>
                                                                        <to>
                                                                            <standardValue>1600</standardValue>
                                                                        </to>
                                                                        <step>
                                                                            <standardValue>50</standardValue>
                                                                        </step>
                                                                    </rangeValues>
                                                                    <fixedValues>
                                                                        <value>200</value>
                                                                        <value>400</value>
                                                                    </fixedValues>
                                                                </availableDoses>
                                                                <availableIntervals>
                                                                    <unit>h</unit>
                                                                    <default>
                                                                        <standardValue>12</standardValue>
                                                                    </default>
                                                                    <fixedValues>
                                                                        <value>12</value>
                                                                        <value>24</value>
                                                                    </fixedValues>
                                                                </availableIntervals>)"
                                                        R"(
                                                                <availableInfusions>
                                                                    <unit>min</unit>
                                                                    <default>
                                                                        <standardValue>60</standardValue>
                                                                    </default>
                                                                    <fixedValues>
                                                                        <value>30</value>
                                                                        <value>60</value>
                                                                    </fixedValues>
                                                                </availableInfusions>
                                                                <comments/>
                                                            </dosages>
                                                            <absorptionParameters>
                                                            </absorptionParameters>
                                                        </formulationAndRoute>
                                                                      <formulationAndRoute>
                                                                          <formulationAndRouteId>id0</formulationAndRouteId>
                                                                          <formulation>parenteralSolution</formulation>
                                                                          <administrationName>something</administrationName>
                                                                          <administrationRoute>intravenousDrip</administrationRoute>
                                                                          <absorptionModel>infusion</absorptionModel>
                                                                          <dosages>
                                                                              <analyteConversions>
                                                                                  <analyteConversion>
                                                                                      <analyteId>voriconazole</analyteId>
                                                                                      <factor>1</factor>
                                                                                  </analyteConversion>
                                                                              </analyteConversions>
                                                                              <availableDoses>
                                                                                  <unit>mg</unit>
                                                                                  <default>
                                                                                      <standardValue>200</standardValue>
                                                                                  </default>
                                                                                  <rangeValues>
                                                                                      <from>
                                                                                          <standardValue>50</standardValue>
                                                                                      </from>
                                                                                      <to>
                                                                                          <standardValue>1600</standardValue>
                                                                                      </to>
                                                                                      <step>
                                                                                          <standardValue>50</standardValue>
                                                                                      </step>
                                                                                  </rangeValues>
                                                                                  <fixedValues>
                                                                                      <value>200</value>
                                                                                      <value>400</value>
                                                                                  </fixedValues>
                                                                              </availableDoses>
                                                                              <availableIntervals>
                                                                                  <unit>h</unit>
                                                                                  <default>
                                                                                      <standardValue>12</standardValue>
                                                                                  </default>
                                                                                  <fixedValues>
                                                                                      <value>12</value>
                                                                                      <value>24</value>
                                                                                  </fixedValues>
                                                                              </availableIntervals>
                                                                              <availableInfusions>
                                                                                  <unit>min</unit>
                                                                                  <default>
                                                                                      <standardValue>60</standardValue>
                                                                                  </default>
                                                                                  <fixedValues>
                                                                                      <value>30</value>
                                                                                      <value>60</value>
                                                                                  </fixedValues>
                                                                              </availableInfusions>
                                                                              <comments/>
                                                                          </dosages>
                                                                          <absorptionParameters>
                                                                          </absorptionParameters>
                                                                      </formulationAndRoute>
                                                                      <formulationAndRoute>
                                                                          <formulationAndRouteId>id1</formulationAndRouteId>
                                                                          <formulation>oralSolution</formulation>
                                                                          <administrationName>something</administrationName>
                                                                          <administrationRoute>oral</administrationRoute>
                                                                          <absorptionModel>extra.lag</absorptionModel>
                                                                          <dosages>
                                                                              <analyteConversions>
                                                                                  <analyteConversion>
                                                                                      <analyteId>voriconazole</analyteId>
                                                                                      <factor>1</factor>
                                                                                  </analyteConversion>
                                                                              </analyteConversions>
                                                                              <availableDoses>
                                                                                  <unit>mg</unit>
                                                                                  <default>
                                                                                      <standardValue>200</standardValue>
                                                                                  </default>
                                                                                  <rangeValues>
                                                                                      <from>
                                                                                          <standardValue>50</standardValue>
                                                                                      </from>
                                                                                      <to>
                                                                                          <standardValue>1600</standardValue>
                                                                                      </to>
                                                                                      <step>
                                                                                          <standardValue>50</standardValue>
                                                                                      </step>
                                                                                  </rangeValues>
                                                                                  <fixedValues>
                                                                                      <value>200</value>
                                                                                      <value>400</value>
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
                                                                                  <analyteGroupId>voriconazole</analyteGroupId>
                                                                                  <absorptionModel>extra.lag</absorptionModel>
                                                                                  <parameterSet>
                                                                                      <parameters>
                                                                                          <parameter>
                                                                                              <parameterId>F</parameterId>
                                                                                              <unit>%</unit>
                                                                                              <parameterValue>
                                                                                                  <standardValue>0.94</standardValue>
                                                        <aprioriComputation>
                                                            <softFormula>
                                                                <inputs>
                                                                    <input>
                                                                        <id>covF</id>
                                                                        <type>double</type>
                                                                    </input>
                                                                </inputs>
                                                                <code>
                                                                    <![CDATA[
                                                                    return covF;
                                                                    ]]>
                                                                </code>
                                                            </softFormula>
                                                            <comments/>
                                                        </aprioriComputation>
                                                                                              </parameterValue>
                                                                                              <bsv>
                                                                                                  <bsvType>lognormal</bsvType>
                                                                                                  <stdDevs>
                                                                                                      <stdDev>0.367</stdDev>
                                                                                                  </stdDevs>
                                                                                              </bsv>)"
                                                        R"(
                                                                                              <validation>
                                                                                                  <errorMessage>
                                                                                                      <text lang='fr'>NA</text>
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
                                                                                                              return F <= 1.0 and F > 0.0;
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
                                                                                                      <standardValue>0.53</standardValue>
                                                                                                  </parameterValue>
                                                                                                  <bsv>
                                                                                                      <bsvType>lognormal</bsvType>
                                                                                                      <stdDevs>
                                                                                                          <stdDev>0.416</stdDev>
                                                                                                      </stdDevs>
                                                                                                  </bsv>
                                                                                                  <validation>
                                                                                                      <errorMessage>
                                                                                                          <text lang='fr'>NA</text>
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
                                                                                              <parameter>
                                                                                                  <parameterId>Tlag</parameterId>
                                                                                                  <unit>h</unit>
                                                                                                  <parameterValue>
                                                                                                      <standardValue>0.162</standardValue>
                                                                                                  </parameterValue>
                                                                                                  <bsv>
                                                                                                      <bsvType>none</bsvType>
                                                                                                      <stdDevs />
                                                                                                  </bsv>
                                                                                                  <validation>
                                                                                                      <errorMessage>
                                                                                                          <text lang='en'>The Tlag value must be positive.</text>
                                                                                                      </errorMessage>
                                                                                                      <operation>
                                                                                                          <softFormula>
                                                                                                              <inputs>
                                                                                                                  <input>
                                                                                                                      <id>Tlag</id>
                                                                                                                      <type>double</type>
                                                                                                                  </input>
                                                                                                              </inputs>
                                                                                                              <code>
                                                                                                                  <![CDATA[
                                                                                                                  return Tlag > 0;
                                                                                                                  ]]>
                                                                                                              </code>
                                                                                                          </softFormula>
                                                                                                          <comments/>
                                                                                                      </operation>
                                                                                                      <comments/>
                                                                                                  </validation>
                                                                                                  <comments>
                                                                                                      <comment lang='en'>NA</comment>
                                                                                                  </comments>
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
                                                                                  <standardValue>6</standardValue>
                                                                              </duration>
                                                                              <multiplier>15</multiplier>
                                                                              <comments>
                                                                                  <comment lang='en'>No comment</comment>
                                                                              </comments>
                                                                          </halfLife>
                                                                          <outdatedMeasure>
                                                                              <unit>d</unit>
                                                                              <duration>
                                                                                  <standardValue>10</standardValue>
                                                                              </duration>
                                                                              <comments>
                                                                                  <comment lang='en'>No comment</comment>
                                                                              </comments>
                                                                          </outdatedMeasure>
                                                                      </timeConsiderations>
                                                                      <comments/>
                                                                  </drugModel>
                                                              </model>)";

struct TestMichaelisMenten2compVmaxAmount : public fructose::test_base<TestMichaelisMenten2compVmaxAmount>
{
    TestMichaelisMenten2compVmaxAmount() {}

    std::unique_ptr<DrugTreatment> buildDrugTreatment(const FormulationAndRoute& _route)
    {
        auto drugTreatment = std::make_unique<DrugTreatment>();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
        // are handled correctly)
        DateTime startSept2018(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));


        //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
        // Add a treatment intake every ten days in June
        // 200mg via a intravascular at 08h30, starting the 01.06
        LastingDose periodicDose(DoseValue(500.0), TucuUnit("mg"), _route, Duration(), Duration(std::chrono::hours(6)));
        DosageRepeat repeatedDose(periodicDose, 16);
        auto sept2018 = std::make_unique<Tucuxi::Core::DosageTimeRange>(startSept2018, repeatedDose);

        drugTreatment->getModifiableDosageHistory().addTimeRange(*sept2018);

        return drugTreatment;
    }


    /// \brief Check that objects are correctly constructed by the constructor.
    void testBolus(const std::string& /* _testName */)
    {
        DrugModelImport importer;

        std::unique_ptr<DrugModel> drugModel;

        auto importStatus = importer.importFromString(drugModel, test_mm_2comp_vmaxamount_tdd);
        fructose_assert_eq(importStatus, DrugModelImport::Status::Ok);

        fructose_assert(drugModel != nullptr);


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        const FormulationAndRoute route(
                Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);

        auto drugTreatment = buildDrugTreatment(route);

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

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            if (dynamic_cast<const SinglePredictionData*>(responseData) == nullptr) {
                return;
            }
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

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

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

            //std::cout << "A priori parameters : " << std::endl;
            //for (auto parameter : resp->getData()[0].m_parameters) {
            //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
            //}
        }

        if (component != nullptr) {
            delete component;
        }
    }

    /// \brief Check that objects are correctly constructed by the constructor.
    void testInfusion(const std::string& /* _testName */)
    {
        DrugModelImport importer;

        std::unique_ptr<DrugModel> drugModel;

        auto importStatus = importer.importFromString(drugModel, test_mm_2comp_vmaxamount_tdd);
        fructose_assert_eq(importStatus, DrugModelImport::Status::Ok);

        fructose_assert(drugModel != nullptr);


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        const FormulationAndRoute route(
                Formulation::ParenteralSolution, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);

        auto drugTreatment = buildDrugTreatment(route);

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

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            if (dynamic_cast<const SinglePredictionData*>(responseData) == nullptr) {
                return;
            }
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

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

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

            //std::cout << "A priori parameters : " << std::endl;
            //for (auto parameter : resp->getData()[0].m_parameters) {
            //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
            //}
        }

        if (component != nullptr) {
            delete component;
        }
    }


    /// \brief Check that objects are correctly constructed by the constructor.
    void testExtra(const std::string& /* _testName */)
    {
        DrugModelImport importer;

        std::unique_ptr<DrugModel> drugModel;

        auto importStatus = importer.importFromString(drugModel, test_mm_2comp_vmaxamount_tdd);
        fructose_assert_eq(importStatus, DrugModelImport::Status::Ok);

        fructose_assert(drugModel != nullptr);


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);

        const FormulationAndRoute route(
                Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag);

        auto drugTreatment = buildDrugTreatment(route);

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

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            if (dynamic_cast<const SinglePredictionData*>(responseData) == nullptr) {
                return;
            }
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

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

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

            //std::cout << "A priori parameters : " << std::endl;
            //for (auto parameter : resp->getData()[0].m_parameters) {
            //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
            //}
        }

        if (component != nullptr) {
            delete component;
        }
    }

    std::unique_ptr<DrugTreatment> buildMixedDrugTreatment(const std::vector<FormulationAndRoute>& _routes)
    {
        auto drugTreatment = std::make_unique<DrugTreatment>();

        // List of time ranges that will be pushed into the history
        DosageTimeRangeList timeRangeList;

        DateTime currentDate(
                date::year_month_day(date::year(2018), date::month(9), date::day(1)),
                Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

        for (const auto& route : _routes) {

            //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
            // Add a treatment intake every ten days in June
            // 200mg via a intravascular at 08h30, starting the 01.06
            LastingDose periodicDose(
                    DoseValue(500.0), TucuUnit("mg"), route, Duration(), Duration(std::chrono::hours(6)));
            DosageRepeat repeatedDose(periodicDose, 4);
            auto dosageTimeRange = Tucuxi::Core::DosageTimeRange(currentDate, repeatedDose);

            drugTreatment->getModifiableDosageHistory().addTimeRange(dosageTimeRange);

            currentDate.addDays(1);
        }
        return drugTreatment;
    }



    /// \brief Check that objects are correctly constructed by the constructor.
    void testMix(const std::string& /* _testName */)
    {
        DrugModelImport importer;

        std::unique_ptr<DrugModel> drugModel;

        auto importStatus = importer.importFromString(drugModel, test_mm_2comp_vmaxamount_tdd);
        fructose_assert_eq(importStatus, DrugModelImport::Status::Ok);

        fructose_assert(drugModel != nullptr);


        IComputingService* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

        fructose_assert(component != nullptr);


        {

            const std::vector<FormulationAndRoute> routes = {
                    {Formulation::ParenteralSolution, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion},
                    {Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag},
                    {Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag}};


            auto drugTreatment = buildMixedDrugTreatment(routes);
            auto covF = std::make_unique<Tucuxi::Core::PatientCovariate>(
                    "covF",
                    "2.0",
                    DataType::Double,
                    TucuUnit("-"),
                    Tucuxi::Common::DateTime(2018_y / sep / 2, 8h + 0min));
            drugTreatment->addCovariate(std::move(covF));
            auto covF2 = std::make_unique<Tucuxi::Core::PatientCovariate>(
                    "covF",
                    "3.0",
                    DataType::Double,
                    TucuUnit("-"),
                    Tucuxi::Common::DateTime(2018_y / sep / 3, 8h + 0min));
            drugTreatment->addCovariate(std::move(covF2));
            auto covF3 = std::make_unique<Tucuxi::Core::PatientCovariate>(
                    "covF",
                    "4.0",
                    DataType::Double,
                    TucuUnit("-"),
                    Tucuxi::Common::DateTime(2018_y / sep / 4, 8h + 0min));
            drugTreatment->addCovariate(std::move(covF3));

            RequestResponseId requestResponseId = "1";
            // 3 days corresponding to the 3 different routes
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 4, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(
                    PredictionParameterType::Apriori,
                    CompartmentsOption::MainCompartment,
                    RetrieveStatisticsOption::DoNotRetrieveStatistics,
                    RetrieveParametersOption::RetrieveParameters,
                    RetrieveCovariatesOption::DoNotRetrieveCovariates);
            std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            if (dynamic_cast<const SinglePredictionData*>(responseData) == nullptr) {
                return;
            }
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

            fructose_assert_eq(resp->getData().at(0).m_parameters.size(), size_t{8});
            fructose_assert_eq(resp->getData().at(4).m_parameters.size(), size_t{8});
            fructose_assert_eq(resp->getData().at(8).m_parameters.size(), size_t{8});

            fructose_assert_eq(resp->getData().at(0).m_parameters[5].m_value, 27.1);
            fructose_assert_eq(resp->getData().at(0).m_parameters[0].m_value, 2.0);
            fructose_assert_eq(resp->getData().at(4).m_parameters[0].m_value, 2.0);
            fructose_assert_eq(resp->getData().at(8).m_parameters[0].m_value, 3.0);

            //std::cout << "Population parameters : " << std::endl;
            //for (auto parameter : resp->getData()[0].m_parameters) {
            //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
            //}
        }

        {

            const std::vector<FormulationAndRoute> routes = {
                    {Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag},
                    {Formulation::ParenteralSolution, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion},
                    {Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag}};

            auto drugTreatment = buildMixedDrugTreatment(routes);

            auto covF = std::make_unique<Tucuxi::Core::PatientCovariate>(
                    "covF",
                    "2.0",
                    DataType::Double,
                    TucuUnit("-"),
                    Tucuxi::Common::DateTime(2018_y / sep / 2, 8h + 0min));
            drugTreatment->addCovariate(std::move(covF));
            auto covF2 = std::make_unique<Tucuxi::Core::PatientCovariate>(
                    "covF",
                    "3.0",
                    DataType::Double,
                    TucuUnit("-"),
                    Tucuxi::Common::DateTime(2018_y / sep / 3, 8h + 0min));
            drugTreatment->addCovariate(std::move(covF2));
            auto covF3 = std::make_unique<Tucuxi::Core::PatientCovariate>(
                    "covF",
                    "4.0",
                    DataType::Double,
                    TucuUnit("-"),
                    Tucuxi::Common::DateTime(2018_y / sep / 4, 8h + 0min));
            drugTreatment->addCovariate(std::move(covF3));

            RequestResponseId requestResponseId = "1";
            // 3 days corresponding to the 3 different routes
            Tucuxi::Common::DateTime start(2018_y / sep / 1, 8h + 0min);
            Tucuxi::Common::DateTime end(2018_y / sep / 4, 8h + 0min);
            double nbPointsPerHour = 10.0;
            ComputingOption computingOption(
                    PredictionParameterType::Apriori,
                    CompartmentsOption::MainCompartment,
                    RetrieveStatisticsOption::DoNotRetrieveStatistics,
                    RetrieveParametersOption::RetrieveParameters,
                    RetrieveCovariatesOption::DoNotRetrieveCovariates);
            std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                    requestResponseId, start, end, nbPointsPerHour, computingOption);

            ComputingRequest request(requestResponseId, *drugModel, *drugTreatment, std::move(traits));

            std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

            ComputingStatus result;
            result = component->compute(request, response);

            fructose_assert_eq(result, ComputingStatus::Ok);

            const ComputedData* responseData = response->getData();
            fructose_assert(dynamic_cast<const SinglePredictionData*>(responseData) != nullptr);
            if (dynamic_cast<const SinglePredictionData*>(responseData) == nullptr) {
                return;
            }
            const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

            fructose_assert_eq(resp->getCompartmentInfos().size(), size_t{1});
            fructose_assert_eq(resp->getCompartmentInfos()[0].getId(), "voriconazole");
            fructose_assert_eq(
                    resp->getCompartmentInfos()[0].getType(), CompartmentInfo::CompartmentType::ActiveMoietyAndAnalyte);

            fructose_assert_eq(resp->getData().at(0).m_parameters.size(), size_t{8});
            fructose_assert_eq(resp->getData().at(4).m_parameters.size(), size_t{8});
            fructose_assert_eq(resp->getData().at(8).m_parameters.size(), size_t{8});

            fructose_assert_eq(resp->getData().at(0).m_parameters[5].m_value, 27.1);
            fructose_assert_eq(resp->getData().at(0).m_parameters[0].m_value, 2.0);
            fructose_assert_eq(resp->getData().at(4).m_parameters[0].m_value, 2.0);
            fructose_assert_eq(resp->getData().at(8).m_parameters[0].m_value, 3.0);

            //std::cout << "Population parameters : " << std::endl;
            //for (auto parameter : resp->getData()[0].m_parameters) {
            //    std::cout << "Param " << parameter.m_parameterId << " : " << parameter.m_value << std::endl;
            //}
        }


        if (component != nullptr) {
            delete component;
        }
    }
};


#endif // TEST_RKMICHAELISMENTTWOCOMPVMAXAMOUNT_H
