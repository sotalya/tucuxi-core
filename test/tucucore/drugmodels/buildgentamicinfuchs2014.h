#ifndef BUILDGENTAMICINFUCHS2014_H
#define BUILDGENTAMICINFUCHS2014_H

//@@license@@

#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

using namespace Tucuxi::Core;

static const std::string gentamicinFuchs2014_tdd = R"(<?xml version="1.0" encoding="UTF-8"?>
<model version='0.6' xsi:noNamespaceSchemaLocation='drugfile.xsd' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'>
    <history>
        <revisions>
            <revision>
                <revisionAction>creation</revisionAction>
                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                <institution>HEIG-VD // REDS</institution>
                <email>yann.thoma@heig-vd.ch</email>
                <date>2019-02-21</date>
                <comments/>
            </revision>
            <revision>
                <revisionAction>modification</revisionAction>
                <revisionAuthorName>Yann Thoma</revisionAuthorName>
                <institution>HEIG-VD</institution>
                <email>yann.thoma@heig-vd.ch</email>
                <date>2021-01-04</date>
                <comments>
                    <comment lang='en'>Added the age covariate, for domain checking, and modified the half life multiplier</comment>
                </comments>
            </revision>
        </revisions>
    </history>
    <head>
        <drug>
            <atcs>
                <atc>J01GB03</atc>
            </atcs>
            <activeSubstances>
                <activeSubstance>gentamicin</activeSubstance>
            </activeSubstances>
            <drugName>
                <name lang='en'>Gentamicine</name>
                <name lang='fr'>Gentamicin</name>
            </drugName>
            <drugDescription>
                <desc lang='en'>TODO : Add a description here</desc>
            </drugDescription>
            <tdmStrategy>
                <text lang='en'>TODO : Add a TDM strategy</text>
            </tdmStrategy>
        </drug>
        <study>
            <studyName>
                <name lang='en'>Population pharmacokinetic study of gentamicin in a large cohort of premature and term neonates.</name>
            </studyName>
            <studyAuthors> Fuchs et al</studyAuthors>
            <description>
                <desc lang='en'>...</desc>
            </description>
            <references>
                <reference type='bibtex'>
                    <![CDATA[@article{Fuchs2014PopulationPS,
                      title={Population pharmacokinetic study of gentamicin in a large cohort of premature and term neonates.},
                      author={Aline Fuchs and Monia Guidi and Eric Giannoni and Dominique Werner and Thierry Buclin and Nicolas Widmer and Chantal Csajka},
                      journal={British journal of clinical pharmacology},
                      year={2014},
                      volume={78 5},
                      pages={
                              1090-101
                            }
                    }]]>
                </reference>
            </references>
        </study>
        <comments/>
    </head>
    <drugModel>
        <drugId>gentamicin</drugId>
        <drugModelId>ch.tucuxi.gentamicin.fuchs2014</drugModelId>
        <domain>
            <description>
                <desc lang='en'>Age range : 20-100 years. Weight range : 29-97 kg. Disease : Healthy subjects (n=6) and adult patients with infections</desc>
            </description>
            <constraints>
                <constraint>
                    <constraintType>hard</constraintType>
                    <errorMessage>
                        <text lang='en'>The age shall be greater or equal to 20</text>
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
                            <code>
                                <![CDATA[return (age > 19);
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
                        <text lang='en'>The age shall be greater or smaller or equal to 100</text>
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
                            <code>
                                <![CDATA[return (age < 101);
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
                        <text lang='en'>The weight should be in the [29,97] margin</text>
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
                            <code>
                                <![CDATA[
                                return ((bodyweight < 98) && (bodyweight > 28));
                                            ]]>
                                </code>
                            </softFormula>
                            <comments/>
                        </checkOperation>
                        <comments/>
                    </constraint>
                </constraints>
            </domain>)"
            R"(
            <covariates>
                        <covariate>
                            <covariateId>age</covariateId>
                            <covariateName>
                                <name lang='en'>Age</name>
                                <name lang='fr'>Age</name>
                            </covariateName>
                            <description>
                                <desc lang='en'>Age of the patient, in years, only used for domain checking</desc>
                                <desc lang='fr'>Âge du patient, en années, utilisé pour valider l'usage du modèle</desc>
                            </description>
                            <unit>y</unit>
                            <covariateType>ageInYears</covariateType>
                            <dataType>double</dataType>
                            <interpolationType>direct</interpolationType>
                            <refreshPeriod>
                              <unit>y</unit>
                              <value>1</value>
                            </refreshPeriod>
                            <covariateValue>
                                <standardValue>10</standardValue>
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
                            <comments/>
                        </covariate>
                <covariate>
                    <covariateId>bodyweight</covariateId>
                    <covariateName>
                        <name lang='en'>Total Body Weight</name>
                        <name lang='fr'>Poids total</name>
                    </covariateName>
                    <description>
                        <desc lang='en'>Total body weight of patient, in kilogramms</desc>
                        <desc lang='fr'>Poids total du patient, en kilogramme</desc>
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
                        <standardValue>2.170</standardValue>
                    </covariateValue>
                    <validation>
                        <errorMessage>
                            <text lang='fr'>
                            </text>
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
                                    <![CDATA[return ((bodyweight < 98) && (bodyweight > 0.3));
                                        ]]>
                                    </code>
                                </softFormula>
                                <comments/>
                            </operation>
                            <comments/>
                        </validation>
                        <comments>
                            <comment lang='en'>About the mean value in patients</comment>
                        </comments>
                    </covariate>
                    <covariate>
                        <covariateId>gestationalage</covariateId>
                        <covariateName>
                            <name lang='en'>Gestational age</name>
                            <name lang='fr'>Age gestationnel</name>
                        </covariateName>
                        <description>
                            <desc lang='en'>Gestational age of the patient, in weeks</desc>
                            <desc lang='fr'>Âge gestationnel du patient, en semaines</desc>
                        </description>
                        <unit>w</unit>
                        <covariateType>standard</covariateType>
                        <dataType>double</dataType>
                        <interpolationType>direct</interpolationType>
                        <refreshPeriod>
                            <unit>y</unit>
                            <value>1</value>
                        </refreshPeriod>
                        <covariateValue>
                            <standardValue>34</standardValue>
                        </covariateValue>
                        <validation>
                            <errorMessage>
                                <text lang='fr'>
                                </text>
                            </errorMessage>
                            <operation>
                                <softFormula>
                                    <inputs>
                                        <input>
                                            <id>gestationalage</id>
                                            <type>double</type>
                                        </input>
                                    </inputs>
                                    <code>
                                        <![CDATA[return (gestationalage  > 0.0);
                                        ]]>
                                    </code>
                                </softFormula>
                                <comments/>
                            </operation>
                            <comments/>
                        </validation>
                        <comments/>
                    </covariate>
                    <covariate>
                        <covariateId>pna</covariateId>
                        <covariateName>
                            <name lang='en'>Postnatal age</name>
                            <name lang='fr'>Âge post-natal</name>
                        </covariateName>
                        <description>
                            <desc lang='en'>Postnatal age of the patient, in days</desc>
                            <desc lang='fr'>Âge post-natal du patient, en jours</desc>
                        </description>
                        <unit>day</unit>
                        <covariateType>ageInDays</covariateType>
                        <dataType>double</dataType>
                        <interpolationType>linear</interpolationType>
                        <refreshPeriod>
                            <unit>d</unit>
                            <value>1</value>
                        </refreshPeriod>
                        <covariateValue>
                            <standardValue>1</standardValue>
                        </covariateValue>
                        <validation>
                            <errorMessage>
                                <text lang='fr'>
                                </text>
                            </errorMessage>
                            <operation>
                                <softFormula>
                                    <inputs>
                                        <input>
                                            <id>pna</id>
                                            <type>double</type>
                                        </input>
                                    </inputs>
                                    <code>
                                        <![CDATA[return pna > 0;]]>
                                    </code>
                                </softFormula>
                                <comments/>
                            </operation>
                            <comments/>
                        </validation>
                        <comments>
                            <comment lang='en'>
                            </comment>
                        </comments>
                    </covariate>
                </covariates>)"
                R"(
                <activeMoieties>
                    <activeMoiety>
                        <activeMoietyId>gentamicin</activeMoietyId>
                        <activeMoietyName>
                            <name lang='en'>Gentamicin</name>
                        </activeMoietyName>
                        <unit>mg/l</unit>
                        <analyteIdList>
                            <analyteId>gentamicin</analyteId>
                        </analyteIdList>
                        <analytesToMoietyFormula>
                            <hardFormula>direct</hardFormula>
                            <comments/>
                        </analytesToMoietyFormula>
                        <targets>
                            <target>
                                <targetType>peak</targetType>
                                <targetValues>
                                    <unit>mg/l</unit>
                                    <min>
                                        <standardValue>6</standardValue>
                                    </min>
                                    <max>
                                        <standardValue>10</standardValue>
                                    </max>
                                    <best>
                                        <standardValue>8</standardValue>
                                    </best>
                                    <toxicityAlarm>
                                        <standardValue>1000.0</standardValue>
                                    </toxicityAlarm>
                                    <inefficacyAlarm>
                                        <standardValue>0.0</standardValue>
                                    </inefficacyAlarm>
                                    <mic>
                                        <unit>mg/l</unit>
                                        <micValue>
                                            <standardValue>1.0</standardValue>
                                        </micValue>
                                    </mic>
                                </targetValues>
                        <times>
                            <unit>h</unit>
                            <min>
                                <standardValue>0.5</standardValue>
                            </min>
                            <max>
                                <standardValue>3</standardValue>
                            </max>
                            <best>
                                <standardValue>2.5</standardValue>
                            </best>
                        </times>
                                <comments>
                                    <comment lang='en'>TODO : Check the alarm values</comment>
                                </comments>
                            </target>
                            <target>
                                <targetType>residual</targetType>
                                <targetValues>
                                    <unit>mg/l</unit>
                                    <min>
                                        <standardValue>0.5</standardValue>
                                    </min>
                                    <max>
                                        <standardValue>1.5</standardValue>
                                    </max>
                                    <best>
                                        <standardValue>1</standardValue>
                                    </best>
                                    <toxicityAlarm>
                                        <standardValue>1000.0</standardValue>
                                    </toxicityAlarm>
                                    <inefficacyAlarm>
                                        <standardValue>0.0</standardValue>
                                    </inefficacyAlarm>
                                    <mic>
                                        <unit>mg/l</unit>
                                        <micValue>
                                            <standardValue>1.0</standardValue>
                                        </micValue>
                                    </mic>
                                </targetValues>
                                <comments>
                                    <comment lang='en'>TODO : Check the alarm values</comment>
                                </comments>
                            </target>
                        </targets>
                    </activeMoiety>
                </activeMoieties>
                <analyteGroups>
                    <analyteGroup>
                        <groupId>gentamicin</groupId>
                        <pkModelId>linear.2comp.macro</pkModelId>
                        <analytes>
                            <analyte>
                                <analyteId>gentamicin</analyteId>
                                <unit>mg/l</unit>
                                <molarMass>
                                    <value>477.596</value>
                                    <unit>g/mol</unit>
                                </molarMass>
                                <description>
                                    <desc lang='en'>
                                    </desc>
                                </description>
                                <errorModel>
                                    <errorModelType>mixed</errorModelType>
                                    <sigmas>
                                        <sigma>
                                            <standardValue>0.0999</standardValue>
                                        </sigma>
                                        <sigma>
                                            <standardValue>0.178</standardValue>
                                        </sigma>
                                    </sigmas>
                                    <comments>
                                        <comment lang='fr'>add: en mg/L; prop, en % ie 0.178 = 17.8%</comment>
                                        <comment lang='en'>add: in mg/L; prop, in % ie 0.178 = 17.8%</comment>
                                    </comments>
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
                                        <standardValue>0.0886</standardValue>
                                        <aprioriComputation>
                                            <softFormula>
                                                <inputs>
                                                    <input>
                                                        <id>bodyweight</id>
                                                        <type>double</type>
                                                    </input>
                                                    <input>
                                                        <id>gestationalage</id>
                                                        <type>double</type>
                                                    </input>
                                                    <input>
                                                        <id>pna</id>
                                                        <type>double</type>
                                                    </input>
                                                </inputs>
                                                <code>
                                                    <![CDATA[
                                                    mean_BW=2.170;
                                                    theta_bwcl = 0.75;
                                                    theta_gacl = 1.87561;
                                                    mean_ga = 34;
                                                    theta_pnacl = 0.0553356;
                                                    mean_pna = 1;
                                                    theta_CL = 0.0886;

                                                    cl1 = theta_CL * Math.pow((bodyweight/ mean_BW),theta_bwcl);
                                                    cl2 = cl1*(1+theta_gacl*(gestationalage-mean_ga)/mean_ga);
                                                    cl3 = cl2*(1+theta_pnacl*(pna-mean_pna)/mean_pna);
                                                    if (cl3 < 0) {
                                                      cl3 = 0.001;
                                                    }
                                                    return cl3;]]>
                                                </code>
                                            </softFormula>
                                            <comments/>
                                        </aprioriComputation>
                                    </parameterValue>
                                    <bsv>
                                        <bsvType>exponential</bsvType>
                                        <stdDevs>
                                            <stdDev>0.893</stdDev>
                                        </stdDevs>
                                    </bsv>
                                    <comments>
                                        <comment lang='en'>Typical clearance calculated for a patients with CCR = 80 ml/min.</comment>
                                    </comments>
                                </parameter>
                                <parameter>
                                    <parameterId>V1</parameterId>
                                    <unit>l</unit>
                                    <parameterValue>
                                        <standardValue>0.905</standardValue>
                                        <aprioriComputation>
                                            <softFormula>
                                                <inputs>
                                                    <input>
                                                        <id>bodyweight</id>
                                                        <type>double</type>
                                                    </input>
                                                    <input>
                                                        <id>gestationalage</id>
                                                        <type>double</type>
                                                    </input>
                                                </inputs>
                                                <code>
                                                    <![CDATA[
                                                    mean_BW=2.170;
                                                    theta_bwv1=1;
                                                    theta_gav1 = -0.920913;
                                                    mean_ga = 34;
                                                    theta_V1 = 0.905;

                                                    vv1 = theta_V1 *Math.pow((bodyweight/mean_BW),theta_bwv1);
                                                    v2 = vv1*(1+theta_gav1*(gestationalage-mean_ga)/mean_ga);
                                                    if (v2 < 0) {
                                                      v2 = 0.001;
                                                    }
                                                    return v2;]]>
                                                </code>
                                            </softFormula>
                                            <comments/>
                                        </aprioriComputation>
                                    </parameterValue>
                                    <bsv>
                                        <bsvType>exponential</bsvType>
                                        <stdDevs>
                                            <stdDev>0.184</stdDev>
                                        </stdDevs>
                                    </bsv>
                                    <comments>
                                        <comment lang='en'></comment>
                                    </comments>
                                </parameter>
                                <parameter>
                                    <parameterId>Q</parameterId>
                                    <unit>l/h</unit>
                                    <parameterValue>
                                        <standardValue>0.161</standardValue>
                                        <aprioriComputation>
                                            <softFormula>
                                                <inputs>
                                                    <input>
                                                        <id>bodyweight</id>
                                                        <type>double</type>
                                                    </input>
                                                </inputs>
                                                <code>
                                                    <![CDATA[mean_BW=2.170;
                                                    theta_bwq=0.75;
                                                    theta_Q = 0.161;

                                                    q1 = theta_Q*Math.pow((bodyweight/mean_BW),theta_bwq);
                                                    if (q1 < 0) {
                                                      q1 = 0.001;
                                                    }
                                                    return q1;]]>
                                                </code>
                                            </softFormula>
                                            <comments/>
                                        </aprioriComputation>
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
                                        <standardValue>0.558</standardValue>
                                        <aprioriComputation>
                                            <softFormula>
                                                <inputs>
                                                    <input>
                                                        <id>bodyweight</id>
                                                        <type>double</type>
                                                    </input>
                                                </inputs>
                                                <code>
                                                    <![CDATA[mean_BW=2.170;
                                                    theta_bwv3=1;
                                                    theta_V2 = 0.558;

                                                    v3 = theta_V2*Math.pow((bodyweight/mean_BW),theta_bwv3);
                                                    if (v3 < 0) {
                                                      v3 = 0.001;
                                                    }
                                                    return v3;]]>
                                                </code>
                                            </softFormula>
                                            <comments/>
                                        </aprioriComputation>
                                    </parameterValue>
                                    <bsv>
                                        <bsvType>none</bsvType>
                                    </bsv>
                                    <comments/>
                                </parameter>
                            </parameters>
                            <correlations>
                                <correlation>
                                    <param1>CL</param1>
                                    <param2>V1</param2>
                                    <value>0.275</value>
                                    <comments/>
                                </correlation>
                            </correlations>
                        </dispositionParameters>
                    </analyteGroup>
                </analyteGroups>)"
                R"(
                <formulationAndRoutes default='id0'>
                    <formulationAndRoute>
                        <formulationAndRouteId>id0</formulationAndRouteId>
                        <formulation>parenteralSolution</formulation>
                        <administrationName>champ libre</administrationName>
                        <administrationRoute>intravenousDrip</administrationRoute>
                        <absorptionModel>infusion</absorptionModel>
                        <dosages>
                            <analyteConversions>
                                <analyteConversion>
                                    <analyteId>gentamicin</analyteId>
                                    <factor>1</factor>
                                </analyteConversion>
                            </analyteConversions>
                            <availableDoses>
                                <unit>mg</unit>
                                <default>
                                    <standardValue>5</standardValue>
                                </default>
                                <rangeValues>
                                    <from>
                                        <standardValue>1</standardValue>
                                    </from>
                                    <to>
                                        <standardValue>25</standardValue>
                                    </to>
                                    <step>
                                        <standardValue>0.5</standardValue>
                                    </step>
                                </rangeValues>
                            </availableDoses>
                            <availableIntervals>
                                <unit>h</unit>
                                <default>
                                    <standardValue>24</standardValue>
                                </default>
                                <fixedValues>
                                    <value>8</value>
                                    <value>12</value>
                                    <value>18</value>
                                    <value>24</value>
                                    <value>30</value>
                                    <value>36</value>
                                    <value>42</value>
                                    <value>48</value>
                                    <value>60</value>
                                    <value>72</value>
                                </fixedValues>
                            </availableIntervals>
                            <availableInfusions>
                                <unit>min</unit>
                                <default>
                                    <standardValue>30</standardValue>
                                </default>
                                <fixedValues>
                                    <value>5</value>
                                    <value>10</value>
                                    <value>15</value>
                                    <value>30</value>
                                    <value>45</value>
                                </fixedValues>
                            </availableInfusions>
                            <comments/>
                        </dosages>
                        <absorptionParameters/>
                    </formulationAndRoute>
                </formulationAndRoutes>
                <timeConsiderations>
                    <halfLife>
                        <unit>h</unit>
                        <duration>
                            <standardValue>2</standardValue>
                        </duration>
                        <multiplier>200</multiplier>
                        <comments>
                            <comment lang='en'>TODO : Check halflife</comment>
                        </comments>
                    </halfLife>
                    <outdatedMeasure>
                        <unit>d</unit>
                        <duration>
                            <standardValue>100</standardValue>
                        </duration>
                        <comments>
                            <comment lang='en'>TODO : This value is not set now</comment>
                        </comments>
                    </outdatedMeasure>
                </timeConsiderations>
                <comments/>
            </drugModel>
        </model>
        )";


class BuildGentamicinFuchs2014
{
public:
    BuildGentamicinFuchs2014() {}

    std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel()
    {
        DrugModelImport importer;

        std::unique_ptr<DrugModel> drugModel;

        auto status = importer.importFromString(drugModel, gentamicinFuchs2014_tdd);
        EXPECT_EQ(status, DrugModelImport::Status::Ok);

        EXPECT_NE(drugModel, nullptr);

        return drugModel;
    }
};

#endif // BUILDGENTAMICINFUCHS2014_H
