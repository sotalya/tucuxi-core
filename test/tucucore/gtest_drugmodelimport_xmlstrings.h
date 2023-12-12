#ifndef GTEST_DRUGMODELIMPORT_XMLSTRINGS_H
#define GTEST_DRUGMODELIMPORT_XMLSTRINGS_H

#include <string>

static const std::string busulfan1 = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                               <date>2018-09-14</date>
                                               <comments>
                                                   <comment lang="en">This file is based on the first version of
                                                       busulfan validated by Sylvain Goutelle : ch.heig-vd.ezechiel.busulfan_children.xml
                                                   </comment>
                                               </comments>
                                           </revision>
                                       </revisions>
                                   </history>
                                   <!-- Drug description -->
                                   <head>
                                       <drug>
                                           <atcs>
                                               <atc>L01AB01</atc>
                                           </atcs>
                                           <activeSubstances>
                                               <activeSubstance>busulfan</activeSubstance>
                                           </activeSubstances>
                                           <drugName>
                                               <name lang="en">Busulfan</name>
                                               <name lang="fr">Busulfan</name>
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
                                               <name lang="en">Busulfan for children with cumulative AUC</name>
                                           </studyName>
                                           <studyAuthors>...</studyAuthors>
                                           <description>
                                               <desc lang="en">...</desc>
                                           </description>
                                           <references>
                                               <reference type="bibtex">TODO : Add bibtex
                                               </reference>
                                           </references>
                                       </study>
                                       <comments/>
                                   </head>

                                   <!-- Drug data -->
                                   <drugModel>
                                       <drugId>busulfan</drugId> <!-- id d'une des substances actives -->
                                       <drugModelId>ch.tucuxi.busulfan.children_1comp</drugModelId>

                                       <domain>
                                           <description>
                                               <desc lang="en">This is the domain, bla bla bla</desc>
                                           </description>
                                           <constraints>
                                               <constraint>
                                                   <constraintType>hard</constraintType>
                                                   <errorMessage>
                                                       <text lang="en">The age shall be positive</text>
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
                                               <constraint>
                                                   <constraintType>soft</constraintType>
                                                   <errorMessage>
                                                       <text lang="en">The weight should not be too much</text>
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
                                                           <code><![CDATA[return (bodyweight < 100);
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
                                               <covariateValue>
                                                   <standardValue>9</standardValue>
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
                                                               return bodyweight < 300 and bodyweight > 0;
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
                                               <activeMoietyId>busulfan</activeMoietyId>
                                               <activeMoietyName>
                                                   <name lang="en">Busulfan</name>
                                               </activeMoietyName>
                                               <unit>mg/l</unit>
                                               <analyteIdList>
                                                   <analyteId>busulfan</analyteId>
                                               </analyteIdList>
                                               <analytesToMoietyFormula>
                                                   <hardFormula>direct</hardFormula>
                                                   <comments/>
                                               </analytesToMoietyFormula>)"
                                     R"(<!-- Drug targets -->
                                               <targets>
                                                   <target>
                                                       <targetType>peak</targetType>
                                                       <targetValues>
                                                       <unit>mg/l</unit>
                                                           <min>
                                                               <standardValue>1.0</standardValue>
                                                           </min>
                                                           <max>
                                                               <standardValue>1.4</standardValue>
                                                           </max>
                                                           <best>
                                                               <standardValue>1.2</standardValue>
                                                           </best>
                                                           <toxicityAlarm><standardValue>10000.0</standardValue></toxicityAlarm>
                                                           <inefficacyAlarm><standardValue>0.0</standardValue></inefficacyAlarm>
                                                       </targetValues>
                                                       <times>
                                                           <unit>h</unit>
                                                           <min>
                                                               <standardValue>0.9</standardValue>
                                                           </min>
                                                           <max>
                                                               <standardValue>3</standardValue>
                                                           </max>
                                                           <best>
                                                               <standardValue>2.5</standardValue>
                                                           </best>
                                                       </times>
                                                       <comments>
                                                           <comment lang="en">The time values shall be checked. They initially were not aligned with possible infusion times. A Toxicity and inefficacyAlarm should be added</comment>
                                                       </comments>
                                                   </target>
                                                   <target>
                                                       <targetType>cumulativeAuc</targetType>
                                                       <targetValues>
                                                           <unit>mg*h/l</unit>
                                                           <min>
                                                               <standardValue>59.0</standardValue>
                                                           </min>
                                                           <max>
                                                               <standardValue>99.0</standardValue>
                                                           </max>
                                                           <best>
                                                               <standardValue>72.0</standardValue>
                                                           </best>
                                                           <!-- TODO : Check that -->
                                                           <toxicityAlarm><standardValue>10000.0</standardValue></toxicityAlarm>
                                                           <inefficacyAlarm><standardValue>0.0</standardValue></inefficacyAlarm>
                                                       </targetValues>
                                                       <comments>
                                                           <comment lang="en">This is the target cumulative for a regimen with 16 doses over 4 days, i.e. a dose every 6h.</comment>
                                                       </comments>
                                                   </target>
                                               </targets>
                                           </activeMoiety>
                                       </activeMoieties>

                                       <analyteGroups>
                                           <analyteGroup>
                                               <groupId>busulfan</groupId>
                                               <pkModelId>linear.1comp.macro</pkModelId>
                                               <analytes>
                                                   <analyte>
                                                       <analyteId>busulfan</analyteId>
                                                       <unit>mg/l</unit>
                                                       <molarMass>
                                                         <value>246.292</value>
                                                           <unit>g/mol</unit>
                                                       </molarMass>
                                                       <description>
                                                           <desc lang="en"></desc>
                                                       </description> <!-- peut être vide -->

                                                       <errorModel> <!-- optional -->
                                                           <errorModelType>mixed</errorModelType>
                                                           <sigmas>
                                                               <sigma>
                                                                   <standardValue>0.057</standardValue>
                                                               </sigma>
                                                               <sigma>
                                                                   <standardValue>0.11</standardValue>
                                                               </sigma>
                                                           </sigmas>
                                                           <comments>
                                                               <comment lang='fr'>add: en mg/L; prop, en % ie 0.11 = 11.0%</comment>
                                                           </comments>
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
                                                               <standardValue>2.18</standardValue>
                                                               <aprioriComputation>

                                                                   <softFormula>
                                                                       <inputs>
                                                                           <input>
                                                                               <id>bodyweight</id>
                                                                               <type>double</type>
                                                                           </input>
                                                                       </inputs>
                                                                       <code><![CDATA[
                                                               theta_1 = 2.18;

                                                               if (bodyweight < 9.0)
                                                                 theta_2=1.25;
                                                               else
                                                                 theta_2=0.76;

                                                               return theta_1 * Math.pow((bodyweight/ 9),theta_2);

                                                        ]]>
                                                                       </code>
                                                                   </softFormula>
                                                                   <comments/>
                                                               </aprioriComputation>
                                                           </parameterValue>
                                                           <bsv>
                                                               <bsvType>proportional</bsvType> <!-- même chose que le modèle d'erreur -->
                                                               <stdDevs>
                                                                   <stdDev>0.23</stdDev>
                                                               </stdDevs>
                                                           </bsv>
                                                           <validation>
                                                               <errorMessage><text lang="fr"></text></errorMessage>
                                                               <operation>
                                                                   <softFormula>
                                                                       <inputs>
                                                                           <input>
                                                                               <id>CL</id>
                                                                               <type>double</type>
                                                                           </input>
                                                                       </inputs>
                                                                       <code><![CDATA[
                                                                           return CL < 300.0 and CL > 0.0;
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
                                                               <standardValue>6.6</standardValue>
                                                               <aprioriComputation>

                                                                   <softFormula>
                                                                       <inputs>
                                                                           <input>
                                                                               <id>bodyweight</id>
                                                                               <type>double</type>
                                                                           </input>
                                                                       </inputs>
                                                                       <code><![CDATA[
                                                               theta_3=0.86;
                                               return Math.pow(bodyweight,theta_3);
                                                                           ]]>
                                                                       </code>
                                                                   </softFormula>
                                                                   <comments/>
                                                               </aprioriComputation>
                                                           </parameterValue>
                                                           <bsv>
                                                               <bsvType>proportional</bsvType>
                                                               <stdDevs>
                                                                   <stdDev>0.22</stdDev>
                                                               </stdDevs>
                                                           </bsv>
                                                           <validation>)"
                                     R"(<errorMessage><text lang="fr"></text></errorMessage>
                                                               <operation>
                                                                   <softFormula>
                                                                       <inputs>
                                                                           <input>
                                                                               <id>V</id>
                                                                               <type>double</type>
                                                                           </input>
                                                                       </inputs>
                                                                       <code><![CDATA[
                                                                           return V < 300.0 and V > 0.0;
                                                                           ]]>
                                                                       </code>
                                                                   </softFormula>
                                                                   <comments/>
                                                               </operation>
                                                               <comments/>
                                                           </validation>
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
                                               <absorptionModel>infusion</absorptionModel>


                                               <!-- Drug dosages -->
                                               <dosages>
                                                   <analyteConversions>
                                                       <analyteConversion>
                                                           <analyteId>busulfan</analyteId>
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
                                                               <standardValue>1</standardValue>
                                                           </from>
                                                           <to>
                                                               <standardValue>400</standardValue>
                                                           </to>
                                                           <step>
                                                               <standardValue>1</standardValue>
                                                           </step>
                                                       </rangeValues>
                                                   </availableDoses>

                                                   <intervals>
                                                       <unit>h</unit>
                                                       <default>
                                                           <standardValue>6</standardValue>
                                                       </default>
                                                       <fixedValues>
                                                           <value>6</value>
                                                           <value>24</value>
                                                       </fixedValues>
                                                   </intervals>
                                                   <infusions>
                                                       <unit>min</unit>
                                                       <default>
                                                           <standardValue>120</standardValue>
                                                       </default>
                                                       <fixedValues>
                                                           <value>60</value>
                                                           <value>120</value>
                                                           <value>180</value>
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
                                                   <standardValue>12</standardValue>
                                               </duration>
                                               <multiplier>10</multiplier>
                                               <comments>
                                                   <comment lang="en">TODO : Find the half life</comment>
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


static const std::string drug1 =
        R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
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
                                           <desc lang="en">
                                               Adult ICU patients
                           (Age range: 18-77 years, Weight range: 50-130 kg, Creatinine range: 16-120 ml/min)</desc>
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
                                           <covariateId>CLcr</covariateId>
                                           <name>
                                               <name lang="en">Clearance of creatinine</name>
                                               <name lang="fr">Clairance de la créatinine</name>
                                           </name>
                                           <description>
                                               <desc lang="en">Clearance of creatinine</desc>
                                               <desc lang="fr">Clairance de la créatinine</desc>
                                           </description>
                                           <unit>ml/min</unit>
                                           <covariateType>standard</covariateType>
                                           <dataType>double</dataType>
                                           <interpolationType>linear</interpolationType>
                                           <covariateValue>
                                               <standardValue>70</standardValue>
                                           </covariateValue>)"
        R"(<validation> <!-- pourrait être une contrainte -->
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
                                                   <targetType>residual</targetType>
                                                   <targetValues>
                                                       <unit>mg/l</unit>
                                                       <min>
                                                           <standardValue>10.0</standardValue>
                                                       </min>
                                                       <max>
                                                           <standardValue>20.0</standardValue>
                                                       </max>
                                                       <best>
                                                           <standardValue>15.0</standardValue>
                                                       </best>
                                                       <!-- TODO : Check that -->
                                                       <toxicityAlarm><standardValue>10000.0</standardValue></toxicityAlarm>
                                                       <inefficacyAlarm><standardValue>0.0</standardValue></inefficacyAlarm>
                                                   </targetValues>
                                                   <comments>
                                                       <comment lang="en">As seen with Aline for the residual concentration</comment>
                                                       <comment lang="fr">Vu avec Aline pour la concentration résiduelle</comment>
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
                                   </analyteGroups>)"

        R"(<!-- We can have various formulation and routes, and for each one a set of absorption parameters and available dosages -->

                                   <formulationAndRoutes default="id0">
                                       <formulationAndRoute>
                                           <formulationAndRouteId>id0</formulationAndRouteId>
                                           <formulation>parenteral solution</formulation><!-- dictionnaire -->
                                           <administrationName>champ libre</administrationName>
                                           <administrationRoute>intravenousDrip</administrationRoute> <!-- dictionnaire -->
                                           <absorptionModel>infusion</absorptionModel>


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
                                                           <standardValue>2000</standardValue>
                                                       </to>
                                                       <step>
                                                           <standardValue>250</standardValue>
                                                       </step>
                                                   </rangeValues>
                                                   <rangeValues>
                                                       <from>
                                                           <standardValue>2500</standardValue>
                                                       </from>
                                                       <to>
                                                           <standardValue>3000</standardValue>
                                                       </to>
                                                       <step>
                                                           <standardValue>500</standardValue>
                                                       </step>
                                                   </rangeValues>
                                               </availableDoses>

                                               <intervals>
                                                   <unit>h</unit>
                                                   <default>
                                                       <standardValue>12</standardValue>
                                                   </default>
                                                   <fixedValues>
                                                       <value>6</value>
                                                       <value>8</value>
                                                       <value>12</value>
                                                       <value>18</value>
                                                       <value>24</value>
                                                       <value>36</value>
                                                       <value>48</value>
                                                       <value>72</value>
                                                       <value>96</value>
                                                   </fixedValues>
                                               </intervals>
                                               <infusions>
                                                   <unit>min</unit>
                                                   <default>
                                                       <standardValue>60</standardValue>
                                                   </default>
                                                   <fixedValues>
                                                       <value>60</value>
                                                       <value>120</value>
                                                       <value>240</value>
                                                   </fixedValues>
                                               </infusions>
                                               <comments>
                                                   <comment lang="en">As seen with Aline for the different optional values</comment>
                                                   <comment lang="fr">Vu avec Aline pour les différentes valeurs optionnelles</comment>
                                               </comments>
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


const std::string operationXml = R"(
        <something>
            <softFormula>
                                        <inputs>
                                            <input>
                                                <id>CL_population</id>
                                                <type>double</type>
                                            </input>
                                            <input>
                                                <id>bodyweight</id>
                                                <type>double</type>
                                            </input>
                                            <input>
                                                <id>age</id>
                                                <type>double</type>
                                            </input>
                                            <input>
                                                <id>gist</id>
                                                <type>bool</type>
                                            </input>
                                            <input>
                                                <id>sex</id>
                                                <type>double</type>
                                            </input>
                                        </inputs>
                                        <code>
                                            <![CDATA[
                                            theta1 = CL_population;
                                            theta4 = 5.42;
                                            theta5 = 1.49;
                                            theta6 = -5.81;
                                            theta7 = -0.806;

                                            MEANBW = 70;
                                            FBW = (bodyweight - MEANBW) / MEANBW;

                                            MEANAG = 50;
                                            FAGE = (age - MEANAG) / MEANAG;

                                            if (gist)
                                              PATH = 1;
                                            else
                                              PATH = 0;

                                            MALE = sex;

                                            TVCL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

                                            return TVCL;
                                                                     ]]>
                                        </code>
                                    </softFormula>
                                 </something>
                                 )";

#endif // GTEST_DRUGMODELIMPORT_XMLSTRINGS_H
