#ifndef TEST_DRUGMODELIMPORT_H
#define TEST_DRUGMODELIMPORT_H

#include "fructose/fructose.h"

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;


static std::string drug1 = R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
                           <?xml-stylesheet href="drugsmodel.xsl" type="text/xsl" ?>
                           <model xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="0.6" xsi:noNamespaceSchemaLocation="drug2.xsd">
                               <!-- Drug history -->
                               <history>
                                   <revisions>
                                       <revision>
                                           <revisionAction>creation</revisionAction>
                                           <revisionAuthorName>Robert Hooper</revisionAuthorName>
                                           <institution>HEIG-VD // REDS</institution>
                                           <email>robert.hooper@heig-vd.ch</email>
                                           <date>2014-07-17</date>
                                           <comments/>
                                       </revision>
                                       <revision>
                                           <revisionAction>modification</revisionAction>
                                           <revisionAuthorName>Robert Hooper</revisionAuthorName>
                                           <institution>HEIG-VD // REDS</institution>
                                           <email>robert.hooper@heig-vd.ch</email>
                                           <date>2014-07-30</date>
                                           <comments/>
                                       </revision>
                                       <revision>
                                           <revisionAction>modification</revisionAction>
                                           <revisionAuthorName>Sylvain Goutelle</revisionAuthorName>
                                           <institution>CHUV</institution>
                                           <email>sylvain.goutelle@chuv.ch</email>
                                           <date>2017-07-27</date>
                                           <comments>
                                               <comment lang="en">
                                                   Age range : 18-77 years
                                                   Weight range : 50-130 kg
                                                   Disease : Critically ill patients
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
                                           <desc lang="fr">Vanc est un un antibio super sympa</desc>
                                       </drugDescription>
                                       <tdmStrategy>
                                           <text lang="fr">Pourquoi et comment le TDm devrait être appliqué</text>
                                       </tdmStrategy>
                                   </drug>
                                   <study>
                                       <studyName>
                                           <name lang="en">Population pharmacokinetic parameters of vancomycin in critically ill patients</name>
                                           <name lang="fr">Paramètres pharmacocinétiques de la vancomycine de patients en soins intensifs</name>
                                       </studyName>
                                       <studyAuthors>Llopis-Salvia, P. and Jiménez-Torres, N. V.</studyAuthors>
                                       <description>
                                           <desc lang="en">Study description on the parameters concerning the administration of vancomycin</desc>
                                           <desc lang="fr">Description de l'étude sur les paramètres ayant attrait à l'administration de la vancomycine</desc>
                                       </description>
                                       <references>
                                           <reference type="bibtex">@article{llopis-salvia_population_2006,
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
                                               }
                                           </reference>
                                       </references>
                                   </study>
                                   <comments/>
                               </head>

                               <!-- Drug data -->
                               <drugModel>
                                   <drugId>vancomycin</drugId> <!-- id d'une des substances actives -->
                                   <drugModelId>ch.heig-vd.tucuxi.vancomycin</drugModelId>

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
                                                       <formula><![CDATA[return (age > 0);
                                                       ]]>
                                                     </formula>
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
                                                   <formula><![CDATA[return (return (bodyweight < 100);
                                                       ]]>
                                                     </formula>
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
                                           <id>bodyweight</id>
                                           <name>
                                               <name lang="en">Total Body Weight</name>
                                               <name lang="fr">Poids total</name>
                                           </name>
                                           <description>
                                               <desc lang="en">Total body weight of patient, in kilogramms</desc>
                                               <desc lang="fr">Poids total du patient, en kilogramme</desc>
                                           </description>
                                           <unit>kg</unit>
                                           <type>standard</type>
                                           <dataType>double</dataType>
                                           <interpolationType>linear</interpolationType>
                                           <value>
                                               <standardValue>75</standardValue>
                                           </value>
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
                                                   <formula><![CDATA[
                                                   return bodyweight < 300 and bodyweight > 20;
                                                   ]]>
                                                     </formula>
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
                                           <id>bodyweight2</id>
                                           <name>
                                               <name lang="en">Total Body Weight</name>
                                               <name lang="fr">Poids total</name>
                                           </name>
                                           <description>
                                               <desc lang="en">Total body weight of patient, in kilogramms</desc>
                                               <desc lang="fr">Poids total du patient, en kilogramme</desc>
                                           </description>
                                           <unit>kg</unit>
                                           <type>standard</type>
                                           <dataType>double</dataType>
                                           <interpolationType>linear</interpolationType>
                                           <value>
                                               <standardValue>75</standardValue>
                                           </value>
                                           <validation>
                                               <errorMessage><text lang="fr"></text></errorMessage>
                                                 <operation>
                                                   <softFormula>
                                                   <inputs>
                                                     <input>
                                                       <id>bodyweight</id>
                                                       <type>double</type>
                                                     </input>
                                                     </inputs>
                                                   <formula><![CDATA[
                                                       return bodyweight < 300 and bodyweight > 20;
                                                       ]]>
                                                     </formula>
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
                                               <name lang="fr">        </name>
                                           </activeMoietyName>
                                           <unit>mg/l</unit>
                                           <analyteIdList>
                                               <analyteId>vancomycin</analyteId>
                                           </analyteIdList>
                                           <formula>
                                           <hardFormula>direct</hardFormula>
                                               <comments/>
                                           </formula>
                                           <!-- Drug targets -->
                                           <targets>
                                               <target>
                                                   <targetType>residual</targetType>
                                                   <targetValues>
                                                       <min>
                                                           <standardValue>10</standardValue>
                                                       </min>
                                                       <max>
                                                           <standardValue>20</standardValue>
                                                           <aprioriComputation>
                                                               <softFormula>
                                                               <inputs>
                                                                 <input>
                                                                   <id>gist</id>
                                                                   <type>bool</type>
                                                                 </input>
                                                                 </inputs>
                                                               <formula><![CDATA[
                                                                   if gist then
                                                                   return 20;
                                                                   else
                                                                   return 30;
                                                                   ]]>
                                                                 </formula>
                                                               </softFormula>
                                                               <comments/>
                                                           </aprioriComputation>
                                                       </max>
                                                       <best>
                                                           <standardValue>15</standardValue>
                                                       </best>
                                                       <toxicityAlarm><standardValue>15</standardValue></toxicityAlarm>
                                                       <inefficacyAlarm><standardValue>3</standardValue></inefficacyAlarm>
                                                   </targetValues>
                                                   <comments>
                                                       <comment lang="en">As seen with Aline for the residual concentration</comment>
                                                       <comment lang="fr">Vu avec Aline pour la concentration résiduelle</comment>
                                                   </comments>
                                               </target>
                                           </targets>


                                       </activeMoiety> <!--fraction active -->
                                   </activeMoieties>

                                   <analyteGroups>
                                       <analyteGroup>
                                           <groupId>vanco1</groupId>
                                           <pkModelId>std.linear.2comp</pkModelId>
                                           <analytes>
                                               <analyte>
                                                   <analyteId>vanco</analyteId>
                                                   <unit>mg/l</unit>
                                                   <molarMass>
                                                       <value>0</value>
                                                       <unit>g/mol</unit>
                                                   </molarMass>
                                                   <description>
                                                       <desc lang="fr"></desc>
                                                   </description> <!-- peut être vide -->

                                                   <errorModel> <!-- optional -->
                                                       <errorModelType>softcoded</errorModelType>
                                                       <formula>
                                                           <softFormula>
                                                           <inputs>
                                                             <input>
                                                               <id>eps</id>
                                                               <type>double</type>
                                                             </input>
                                                               <input>
                                                                 <id>sigma0</id>
                                                                 <type>double</type>
                                                               </input>
                                                                 <input>
                                                                   <id>sigma1</id>
                                                                   <type>double</type>
                                                                </input>
                                                             </inputs>
                                                           <formula><![CDATA[
                                                               return eps * sigma0 + eps * exp(sigma1);
                                                               ]]>
                                                             </formula>
                                                           </softFormula>
                                                           <comments/>
                                                       </formula>
                                                       <sigmas>
                                                           <sigma>
                                                               <standardValue>0.239</standardValue>
                                                           </sigma>
                                                           <sigma>
                                                               <standardValue>0.185</standardValue>
                                                           </sigma>
                                                       </sigmas>
                                                       <comments>
                                                           <comment lang="fr">Une fonction d'erreur model définie par la fiche médicament</comment>
                                                       </comments>
                                                   </errorModel>
                                                   <comments/>
                                               </analyte>
                                           </analytes>

                                           <!-- Drug parameters -->
                                           <dispositionParameters>
                                               <parameters>
                                                   <parameter>
                                                       <id>CL</id>
                                                       <unit>l/h</unit>
                                                       <value>
                                                           <standardValue>3.505</standardValue>
                                                           <aprioriComputation>

                                                                 <softFormula>
                                                                 <inputs>
                                                                   <input>
                                                                     <id>covariate_CLcr</id>
                                                                     <type>double</type>
                                                                   </input>
                                                                     <input>
                                                                       <id>covariate_bodyweight</id>
                                                                       <type>double</type>
                                                                     </input>
                                                                   </inputs>
                                                                 <formula><![CDATA[
                                                                     CCR = covariate_CLcr
                                                                     BW = covariate_bodyweight
                                                                     theta_1 = 0.034;
                                                                     theta_2 = 0.015;

                                                                     TVCL = theta_1*CCR+theta_2*BW;
                                                                     return TVCL;
                                                                     ]]>
                                                                   </formula>
                                                                 </softFormula>
                                                               <comments/>
                                                           </aprioriComputation>
                                                       </value>
                                                       <bsv>
                                                           <bsvType>normal</bsvType> <!-- même chose que le modèle d'erreur -->
                                                           <stdDevs>
                                                               <stdDev>0.292</stdDev>
                                                           </stdDevs>
                                                       </bsv>
                                                       <validation>
                                                           <errorMessage><text lang="fr"></text></errorMessage>
                                                             <operation>
                                                               <softFormula>
                                                               <inputs>
                                                                 <input>
                                                                   <id>bodyweight</id>
                                                                   <type>double</type>
                                                                 </input>
                                                                 </inputs>
                                                               <formula><![CDATA[
                                                                   return bodyweight < 300 and bodyweight > 20;
                                                                   ]]>
                                                                 </formula>
                                                               </softFormula>
                                               <comments/>
                                                             </operation>
                                                           <comments/>
                                                       </validation>
                                                       <comments>
                                                           <comment lang="en">Typical clearance calculated for a patients with weight = 75 kg et CCR = 70 ml/min</comment>
                                                       </comments>
                                                   </parameter>
                                                   <parameter>
                                                       <id>V1</id>
                                                       <unit>l</unit>
                                                       <value>
                                                           <standardValue>3.1</standardValue>
                                                           <aprioriComputation>

                                                                                                   <softFormula>
                                                                                                   <inputs>
                                                                                                     <input>
                                                                                                       <id>covariate_CLcr</id>
                                                                                                       <type>double</type>
                                                                                                     </input>
                                                                                                       <input>
                                                                                                         <id>covariate_bodyweight</id>
                                                                                                         <type>double</type>
                                                                                                       </input>
                                                                                                     </inputs>
                                                                                                   <formula><![CDATA[
                                                                                                       CCR = covariate_CLcr
                                                                                                       BW = covariate_bodyweight
                                                                                                       theta_1 = 0.034;
                                                                                                       theta_2 = 0.015;

                                                                                                       TVCL = theta_1*CCR+theta_2*BW;
                                                                                                       return TVCL;
                                                                                                       ]]>
                                                                                                     </formula>
                                                                                                   </softFormula>
                                                               <comments/>
                                                           </aprioriComputation>
                                                       </value>
                                                       <bsv>
                                                           <bsvType>normal</bsvType>
                                                           <stdDevs>
                                                               <stdDev>2.0</stdDev>
                                                           </stdDevs>
                                                       </bsv>
                                                       <validation>
                                                           <errorMessage><text lang="fr"></text></errorMessage>
                                                             <operation>
                                                               <softFormula>
                                                               <inputs>
                                                                 <input>
                                                                   <id>bodyweight</id>
                                                                   <type>double</type>
                                                                 </input>
                                                                 </inputs>
                                                               <formula><![CDATA[
                                                                   return bodyweight < 300 and bodyweight > 20;
                                                                   ]]>
                                                                 </formula>
                                                               </softFormula>
                                               <comments/>
                                                             </operation>
                                                           <comments/>
                                                       </validation>
                                                       <comments>
                                                           <comment lang="en">Typical volume calculated for a patients with weight = 75 kg</comment>
                                                       </comments>
                                                   </parameter>
                                                   <parameter>
                                                       <id>Q</id>
                                                       <unit>l/h</unit>
                                                       <value>
                                                           <standardValue>7.48</standardValue>
                                                       </value>
                                                       <bsv>
                                                           <bsvType>none</bsvType>
                                                           <stdDevs>
                                                               <stdDev>0</stdDev>
                                                           </stdDevs>
                                                       </bsv>
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
                                                               <formula><![CDATA[
                                                                   return bodyweight < 300 and bodyweight > 20;
                                                                   ]]>
                                                                 </formula>
                                                               </softFormula>
                                               <comments/>
                                                             </operation>
                                                           <comments/>
                                                       </validation>
                                                       <comments/>
                                                   </parameter>
                                                   <parameter>
                                                       <id>V2</id>
                                                       <unit>l</unit>
                                                       <value>
                                                           <standardValue>99</standardValue>
                                                       </value>
                                                       <bsv>
                                                           <bsvType>none</bsvType>
                                                           <stdDevs>
                                                               <stdDev>0</stdDev>
                                                           </stdDevs>
                                                       </bsv>
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
                                                               <formula><![CDATA[
                                                                   return bodyweight < 300 and bodyweight > 20;
                                                                   ]]>
                                                                 </formula>
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
                                           <administrationRoute>intravenousBolus</administrationRoute> <!-- dictionnaire -->


                                           <!-- Drug dosages -->
                                           <dosages>
                                               <analyteConversions>
                                                   <analyteConversion>
                                                       <analyteId>vanco</analyteId>
                                                       <factor>1</factor>
                                                   </analyteConversion>
                                                   <analyteConversion>
                                                       <analyteId>analyte2</analyteId>
                                                       <factor>0.4</factor>
                                                   </analyteConversion>
                                               </analyteConversions>

                                               <availableDoses>
                                                   <unit>mg</unit>
                                                   <default>
                                                       <standardValue>1000</standardValue>
                                                   </default>
                                                   <rangeValues>
                                                       <from>
                                                           <standardValue>5</standardValue>
                                                       </from>
                                                       <to>
                                                           <standardValue>1500</standardValue>
                                                       </to>
                                                       <step>
                                                           <standardValue>5</standardValue>
                                                       </step>
                                                   </rangeValues>
                                                   <fixedValues>
                                                       <value>250</value>
                                                       <value>500</value>
                                                       <value>750</value>
                                                       <value>1000</value>
                                                       <value>1250</value>
                                                       <value>1500</value>
                                                       <value>1750</value>
                                                       <value>2000</value>
                                                       <value>2500</value>
                                                       <value>3000</value>
                                                   </fixedValues>
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


                                           <absorptionParameters>
                                           <parameterSetAnalyteGroup>
                                                   <analyteGroupId>vanco1</analyteGroupId>
                                                   <absorptionModelId>bolus</absorptionModelId>
                                                   <parameterSet>

                                                   <parameters>
                                                       <parameter>
                                                           <id>Ka</id>
                                                           <unit>l/h</unit>
                                                           <value>
                                                               <standardValue>7.48</standardValue>
                                                           </value>
                                                           <bsv>
                                                               <bsvType>none</bsvType>
                                                               <stdDevs>
                                                                   <stdDev>0</stdDev>
                                                               </stdDevs>
                                                           </bsv>
                                                           <validation> <!-- pourrait être une contrainte -->
                                                               <errorMessage>
                                                                   <text lang="fr"></text>
                                                               </errorMessage>
                                                               <operation>
                                                                 <softFormula>
                                                                 <inputs>
                                                                   <input>
                                                                     <id>bodyweight</id>
                                                                     <type>double</type>
                                                                   </input>
                                                                   </inputs>
                                                                 <formula><![CDATA[
                                                                     return bodyweight < 300 and bodyweight > 20;
                                                                     ]]>
                                                                   </formula>
                                                                 </softFormula>
                                                           <comments/>
                                                               </operation>
                                                               <comments/>
                                                           </validation>
                                                           <comments/>
                                                       </parameter>
                                                   </parameters>
                                                   <!-- Correlations between absorption parameters, and between absorption and disposition parameters -->
                                                   <correlations />
                                               </parameterSet>
                                             </parameterSetAnalyteGroup>

                                           </absorptionParameters>
                                       </formulationAndRoute>
                                   </formulationAndRoutes>



                                   <timeConsiderations>
                                       <!-- Drug half-life -->
                                       <halfLife>
                                           <unit>h</unit>
                                           <value>
                                               <standardValue>12</standardValue>
                                           </value>
                                           <multiplier>10</multiplier>
                                           <comments/>
                                       </halfLife>


                                       <outdatedMeasure>
                                           <unit>d</unit>
                                           <value>
                                               <standardValue>100</standardValue>
                                           </value>
                                           <comments/>
                                       </outdatedMeasure>
                                   </timeConsiderations>

                                   <!-- Drug general comments -->
                                   <comments>
                                   </comments>
                               </drugModel>
                           </model>

)";


struct TestDrugModelImport : public fructose::test_base<TestDrugModelImport>
{
    TestDrugModelImport() { }

    DrugModel *buildImatinib()
    {
        BuildImatinib builder;
        return builder.buildDrugModel();
    }

    void testImatinib(const std::string& /* _testName */)
    {
        DrugModel* imatinib;
        DrugModelImport *importer = new DrugModelImport();

        DrugModelImport::Result result = importer->importFromString(imatinib, drug1);
//        DrugModelImport::Result result = importer->importFromFile(imatinib, "/home/ythoma/docs/ezechiel/git/dev/src/newdrugs/last/ch.heig-vd.ezechiel.example.xml");

        fructose_assert( result == DrugModelImport::Result::Ok);

        fructose_assert(imatinib != nullptr);

        delete importer;
    }

};

#endif // TEST_DRUGMODELIMPORT_H
