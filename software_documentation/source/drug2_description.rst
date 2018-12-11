*********************
Drug description file
*********************

.. contents:: Contents
	:depth: 1
	:backlinks: top
	:local:

This chapter presents the way medical drugs have to be described. Tucuxi is flexible in terms of handling various medical drugs (or substances). The drugs are defined within XML files, and can be edited thanks to the special Tucuxi drug editor.



General elements
================

This section lists some useful elements used everywhere in the drug model file.



Units
^^^^^

The units in Tucuxi consist of a base and a multiplier. For example in *ug/l*, *g/l* is the base, and *u* the multiplier, which express a unit of *micrograms per liter*. The convertion factors are used to convert the data produced by the models of Tucuxi. The molar mass is used to give the user the choice to use moles instead of the default units.



.. _stdAprioriValue:

stdAprioriValue
^^^^^^^^^^^^^^^

In various parts of the drug model, elements can have a default value, and an apriori value calculated with the help of the patient covariates. Every time such a pattern is used, the element is of type *stdAprioriValue*.

.. csv-table:: stdAprioriValue content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<standardValue>             ",  double        , 1:1 , "The default value"
	 "<aprioriComputation>                 ", :ref:`operation`   , 0:1 , "The operations to calculate the a priori value"


When such an element is used, the software will use default values for calculation involving the typical patient. In case of a priori and a posteriori calculations, it will try to apply the operations on each of such value, in order to better fit the patient reality.

.. _operation:

Operation
^^^^^^^^^

The operations are used to modify the values of the parameters in accordance with the patient's covariates. They are used to compute the *a priori* parameters and can be of three types:
- SoftFormula
- HardFormula
- MultiFormula

These three options are mutually exclusive, so the possible styles of operations are:

.. code-block:: xml

	<operation>
	  <softFormula>
	    <inputs>
	      <input>
	        <id>bodyweight</id>
	        <type>double</type>
	      </input>
	    </inputs>
	    <code><![CDATA[
	        return bodyweight < 100.0 and bodyweight > 0.0;
	            ]]>
	    </code>
	  </softFormula>
	  <comments/>
	</operation>


.. code-block:: xml

	<operation>
	  <hardFormula>formulaId</hardFormula>
	  <comments/>
	</operation>


.. code-block:: xml

	<operation>
	  <multiFormula>
	    <softFormula>
	      <inputs>
	        <input>
	          <id>bodyweight</id>
	          <type>double</type>
	        </input>
	      </inputs>
	      <code><![CDATA[
	          return bodyweight < 100.0 and bodyweight > 0.0;
	              ]]>
	      </code>
	    </softFormula>
	    <hardFormula>formulaId</hardFormula>
	  </multiFormula>
	  <comments/>
	</operation>

|

+-----------------------+----------------+-----+---------------------------------------------+
| Tag name              | Format         | Occ.| Description                                 |
+=======================+================+=====+=============================================+
| <operation>           |                |     | Description of an operation                 |
+-----------------------+----------------+-----+---------------------------------------------+
|____<Softformula>      |                | 1:∞ | An Javascript operation                     |
+-----------------------+----------------+-----+---------------------------------------------+
|________<inputs>       |                | 1:1 | The list of required inputs                 |
+-----------------------+----------------+-----+---------------------------------------------+
|____________<input>    |                | 1:∞ | An input for the formula                    |
+-----------------------+----------------+-----+---------------------------------------------+
|________________<id>   |                | 1:1 | The Id of the required input for the formula|
+-----------------------+----------------+-----+---------------------------------------------+
|________________<type> |                | 1:1 | The type of data : double, int or bool      |
+-----------------------+----------------+-----+---------------------------------------------+
|________<code>      | :ref:`formulaCode` | 1:∞ | The operation formula                      |
+-----------------------+----------------+-----+---------------------------------------------+
|____<hardFormula>      | string         | 0:1 | A hardcoded operation                       |
+-----------------------+----------------+-----+---------------------------------------------+
|____<multiFormula>     |                | 0:1 | A multi-operation formula                   |
+-----------------------+----------------+-----+---------------------------------------------+
|________<...>          |                | 1:∞ | Any of softFormula and hardFormula          |
+-----------------------+----------------+-----+---------------------------------------------+
|____<comments>         |                | 1:1 | Comments about the operation                |
+-----------------------+----------------+-----+---------------------------------------------+

|

An operation can be used in many elements. For instance it is used in parameters, targets, covariates, in order to calculate a priori values.

A formula can use the value of any global or drug-specific covariate. To do so, you must use the last part of a covariate's ID preceded by the *covariate_*keyword, as shown above with *covariate_sex*. You can also use any of the drug's parameters, using its ID preceded by the *parameter_*keyword, as in *parameter_V*. The formula should simply return a value, nothing else is mandatory.

The formula must always be surrounded by the *<![CDATA[* and *]]* markers. The language used to express the formula is based on Javascript and supports a subset of it. A formula must always return a value.

When an operation can contain a list of formula the computing engine shall try to apply the first formula. If there are missing covariates for such formula, then the second formula is tried, and so on, until a valid formula is found.


.. _formulaCode:

Code
^^^^^^^

A formula is an operation returning a value, based on some inputs.
The content of the element is then a source code in the correct format.

The source code must always be surrounded by the *<![CDATA[* and *]]* markers. The language used to express the formula is based on the ECMAScript scripting language, as defined in standard ECMA.262. You can find more information about the language EMCA website (http://www.ecma-international.org/publications/standards/Ecma-262.htm) or directly in the Qt documentation (http://qt-project.org/doc/qt-5/qtscript-index.html#language-overview). A formula must always return a value.


.. _comments:

Comments
^^^^^^^^

Before explaining all the specific fields, a word on comments is required, as the <comments> tag can be found at different places of the file. A comment has the following structure:

.. code-block:: xml

	<comments>
	  <comment lang='en'>This work is based on the paper nice_paper, that can be found here.</comment>
	  <comment lang='fr'>La description de ce médicament est basée sur ce nice_paper, qui peut être trouvé ici.</comment>
	</comments>


|

+-----------------------+--------+-----+--------------------------------------+
| Tag name              | Format | Occ.| Description                          |
+=======================+========+=====+======================================+
| <comments>            |        | 1:1 | List of translated comments          |
+-----------------------+--------+-----+--------------------------------------+
|___<comment lang='xx'> | string | 0:∞ | Comment for a specific language      |
+-----------------------+--------+-----+--------------------------------------+

|

It contains as many <comment> tags as required. Each <comment> tag has an attribute *lang* defining the language of the comment, enabling multi-language support for the description of the medical drugs.


.. _validation:

A validation element allows to specify a validity function to check another element value. It is used in covariates and parameters.

It is based on an operation, and embeds an error message that can serve to display relevant information to the user.

.. literalinclude:: drug_description/examples/validation_example.xml
	:language: xml



.. csv-table:: head content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 20, 10, 5, 30

   "<validation>", , 1:1 , Description of a validation
   "___<errorMessage>",  , 1:1 , List of error messages
   "______<text lang='xx'>", string , 0:∞ , Message for a specific language
   "___<operation>", :ref:`operation`  , 1:1 , The checking operation
   "___<comments>", :ref:`comments`        , 1:1 , Comments about validation



.. _model:

Model
=====


| The global structure of the XML file is the following:


.. code-block:: xml

	<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
	<?xml-stylesheet href="drugsmodel2.xsl" type="text/xsl" ?>
	<model version='1.0'
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:noNamespaceSchemaLocation="drug2.xsd">
	  <history></history>
	  <head></head>
	  <drugModel></drugModel>
	</model>

|

+--------------------+-----------------+--------+-----------------------------------------------+
| Tag name           | Format          | Occ.   | Description                                   |
+====================+=================+========+===============================================+
| <history>          | :ref:`history`  | 1:1    | History of the file                           |
+--------------------+-----------------+--------+-----------------------------------------------+
| <head>             | :ref:`head`     | 1:1    | General information                           |
+--------------------+-----------------+--------+-----------------------------------------------+
| <drugModel>        | :ref:`drugModel`| 1:1    | Everything needed for computation             |
+--------------------+-----------------+--------+-----------------------------------------------+

|


The XML format is described in the file ``drug2.xsd`` which is used by Tucuxi in order to check the structure correctness before loading the drug description file.



.. _history:

History
=======

Used by :ref:`model`.

The history of the file is described inside the <history> tag. A standard history looks like this:


.. literalinclude:: drug_description/examples/history_example.xml
	:language: xml

|

+-----------------------------+------------------+--------+-----------------------------------------------------------------+
| Tag name                    |Format            | Occ.   | Description                                                     |
+=============================+==================+========+=================================================================+
| <revisions>                 |                  | 1:1    | List of revisions                                               |
+-----------------------------+------------------+--------+-----------------------------------------------------------------+
|___<revision>                |                  | 1:∞    | Revision                                                        |
+-----------------------------+------------------+--------+-----------------------------------------------------------------+
|________<revisionAction>     | string           | 1:1    | Type of revision                                                |
+-----------------------------+------------------+--------+-----------------------------------------------------------------+
|________<revisionAuthorName> | string           | 1:1    | Name of the person who wrote the revision                       |
+-----------------------------+------------------+--------+-----------------------------------------------------------------+
|________<institution>        | string           | 1:1    | Name of institution from which the revision was written         |
+-----------------------------+------------------+--------+-----------------------------------------------------------------+
|________<email>              | string           | 1:1    | Email of the person who wrote the revision                      |
+-----------------------------+------------------+--------+-----------------------------------------------------------------+
|________<comments>           | :ref:`comments`  | 0:1    | Comments about the modifications done in this revision          |
+-----------------------------+------------------+--------+-----------------------------------------------------------------+


The type of revision is one of the following possibilities:

+--------------+--------------------------------------------------------------------------------+
| Value        | Description                                                                    |
+==============+================================================================================+
| creation     | Creation of the file. Only one revision of this type is allowed.               |
+--------------+--------------------------------------------------------------------------------+
| modification | Modification of the file.                                                      |
+--------------+--------------------------------------------------------------------------------+
| review       | Review by a person. Comments in the revision tag allows to comment the review. |
+--------------+--------------------------------------------------------------------------------+
| validation   | Validated by a person. No modification done on the document.                   |
+--------------+--------------------------------------------------------------------------------+



|

The history section keeps track of the creator of the drug XML file as well as the people that modified it. The <revisions> contains as many <revision> tag as necessary. Each <revision> tag contains the same structure, as shown in the figure above, and the correct revisionAction should be used according to the action done on the file.


.. _head:

Head
====

Used by :ref:`model`.

The head of the file contains general information about the drug, such as its ID, the model it uses, its name, description and so on. It does not embed information required by the computing engines. The head structure is the following:


.. literalinclude:: drug_description/examples/head_example.xml
	:language: xml



|

.. csv-table:: head content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

   "<drug>", , 1:1 , Description of the drug itself
   "___<atcs>",  , 1:1 , List of ATCs
   "______<atc>", string , 1:∞ , "The drug code, or Anatomical Therapeutic Chemical (ATC)"
   "___<activeSubstances>",        , 1:1 , List of active substances
   "______<activeSubstance>", string , 1:∞ , TB : active substance
   "___<drugName>",        , 1:1 , The translated drug's names
   "______<name>", string , 1:∞ , Name of the drug for the specified language
   "___<drugDescriptions>",        , 1:1 , The translated drug's descriptions
   "______<desc>", string , 1:∞ , Description of the drug for the specified language
   "___<tdmStrategy>",        , 1:1 , The translated drug's TDM strategy
   "______<text>",  string      , 1:∞ , TB : à remplir
   "<study>",  , 1:1 , Description of the study used to fill this file
   "___<studyName>",        , 1:1 , The translated drug's study names
   "______<name>", string , 1:∞ , Study name of the drug for the specified language
   "___<studyAuthors>",  string      , 1:1 , The name of the study authors
   "___<description>",        , 1:1 , The translated descriptions of the study
   "______<desc>", string , 1:∞ , Description of the study for the specified language
   "___<references>",        , 1:1 , References to related publications
   "______<reference>", string , 0:∞ , Reference to a specific article or publication
   "<comments>", :ref:`comments`        , 1:1 , Comments about the drug's header

|

The active substances are identifiers, and shall come from a dictionary.

The corresponding ATC can be found online using the Anatomical Therapeutic Chemical Classification System. The drug ID must be unique amongst all the drugs. By convention, it starts with *ch.heig-vd.ezechiel.*, followed by the drug name and eventually the domain and/or study names, for all the drugs that are shipped with the default package of Tucuxi.

The drug name is the general name of the drug, eg. *Gentamicin*.
The study name finally specifies the study on which this drug is based. Any of those names can be translated by using the *lang* attribute to specify the target language. It is to be noted that the *lang* attribute is mandatory for such translatable strings.





.. _drugModel:

DrugModel
=========

Used by :ref:`model`.

This third part of a model file embed every information required for any computation.



.. code-block:: xml

	<drugModel>
	  <drugId>vancomycin</drugId>
	  <drugModelId>ch.heig-vd.vancomycin</drugModelId>
	  <domain></domain>
	  <covariates></covariates>
	  <activeMoieties></activeMoieties>
	  <analyteGroups></analyteGroups>
	  <formulationAndRoutes></formulationAndRoutes>
	  <timeConsiderations></timeConsiderations>
	  <comments />
	</drugModel>

|


.. csv-table:: drugModel content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

	 "<drugModel>",  ,  , Everything needed for any calculation
   "____<drugId>", string , 1:1 , Unique identifier of the drug
   "____<drugModelId>", string , 1:1 , Identifier of the model described in the file
   "____<domain>", :ref:`domain` , 1:1 , Usage domain for this model. TB
   "____<covariates>", :ref:`covariates` , 1:1 , List of covariates used by the model
   "____<activeMoieties>", :ref:`activeMoieties`, 1:1 , List of active moieties. TB
   "____<analyteGroups>", :ref:`analyteGroups`, 1:1 , List of groups of analytes
   "____<formulationAndRoutes>", :ref:`formulationAndRoutes`, 1:1 , List of formulation and routes of administration
   "____<timeConsiderations>", :ref:`timeConsiderations`, 1:1 , "Some information about time, such as halflife"
   "____<comments>", :ref:`comments` , 1:1 , General comments on the model


The *drugId* shall uniquely identify the drug. It shall come out of a dictionary.

The *drugModelId* is the identifier of the model proposed in the file. It shall be unique. Some conventions will help maintain such models. It shall consist of alphanumeric characters separated by dots, such as **ch.heig-vd.vancomycin**. The first part is the country identifier, the second the instution identifier, and the third the drug identifier. A fourth one can be added in case various models for the same drug are proposed by the same institution: **ch.heig-vd.vancomycin.model2**.

Except the *drugId*, the *drugModelId*, and the *comments* fields, the other ones are described in specific sections.

The :ref:`domain` represents the validity of the drug model for a specific patient. So, depending on his covariates, the model should not be used.

The :ref:`covariates` list all the covariates used in various parts of the drugModel.

The :ref:`activeMoieties` list the active moieties of the drug. For a majority of drugs there should be only a single active moiety, and so filling this part should be straightforward.

The :ref:`analyteGroups` list the groups of analytes. This concept of groups allow to have independent or dependent analytes in terms of Pk models.

The :ref:`formulationAndRoutes` list the formulations and routes of administrations. As such it mainly embeds absorption parameters descriptions.

The :ref:`timeConsiderations` allows to indicate the half life of the drug and the validity time of measures.

.. _domain:

Domain
======

The domain of application gives information about the population on which the model can be applied.


.. literalinclude:: drug_description/examples/domain_example.xml
	:language: xml




.. csv-table:: domain content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

   "<description>",        , 1:1 , The translated descriptions of the domain
   "___<desc>", string , 1:∞ , Description of the domain for the specified language
   "<constraints>", string , 1:1 , List of constraints allowing the model to be used
   "___<constraint>", :ref:`constraint`, 1:∞ , A constraint for using the model
   "______<constraintType>", :ref:`contraintType` , 1:1 , Importance of the constraint: soft or hard
   "______<errorMessage>",        , 1:1 , The translated error message for the constraint
   "_________<text>", string , 1:∞ , Error message in the specified language
   "______<requiredCovariates>", , 1:1 , List of required covariates for checking this constraint
   "_________<covariateId>", string , 1:∞ , Id of a covariate required by this constraint
   "______<checkOperation>", :ref:`operation` , 1:1 , formula used to check the validity of covariates

The description shall allow to understand the domain of application of the model. It shall be as complete as possible.

The constraints then allow the software to check wether the model can be used for a specific patient or not.

For instance, if the model is valid for adults from 20 to 60 years old, the covariate *ageInYears* of the patient shall be available and checked in order to avoir the use of the model for children.

In case a specific constraint is not met, then the errorMessage is used to notify the user.


.. _constraint:

Constraint
^^^^^^^^^^

A constraint allows to check for the possibility to use the model with a specific patient, depending on his covariates validity.

.. csv-table:: constraint content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

   "<constraint>", ,  , A constraint for using the model
   "____<constraintType>", :ref:`contraintType` , 1:1 , Importance of the constraint: soft or hard
   "____<errorMessage>",        , 1:1 , The translated error message for the constraint
   "________<text>", string , 1:∞ , Error message in the specified language
   "____<requiredCovariates>", , 1:1 , List of required covariates for checking this constraint
   "________<covariateId>", string , 1:∞ , Id of a covariate required by this constraint
   "____<checkOperation>", :ref:`operation` , 1:1 , formula used to check the validity of covariates

A constraint can be *soft* or *hard*. In case of a *soft* one, only a warning is issued, while a *hard* constraint shall impose a rejection of a priori and a posteriori calculations.

The list of required covariates allows to check wether all covariates are available, and the formula is used to check the constraint validity. This formula shall return a boolean returning *true* in case the constraint is met, and *false* otherwise.


.. _contraintType:

ConstraintType
^^^^^^^^^^^^^^

Used by :ref:`constraint`.


Type of constraint



.. csv-table:: constraintType
   :header: "Value", "Description"
   :widths: 3, 30

   soft , "If the patient covariate can not be retrieved, a warning is issued, but the model can be used"
   hard , "If the patient covariate can not be retrieved, a warning is issued, and the model can not be used"



.. _covariates:

.. _covariate:

Covariates
==========

A covariate is a medical information about the patient. A drug can contain from 0 to an unlimited number of covariates that can then be used by Tucuxi to compute and adapt the parameters for the given patient. They are defined in this manner:



.. literalinclude:: drug_description/examples/covariate_example.xml
	:language: xml

|


.. csv-table:: covariates content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<covariate>             ",          , 0:∞ , Description of a covariate
	 "____<covariateId>                 ", string   , 1:1 , The covariate's unique identifier
	 "____<covariateName>               ",          , 1:1 , The translated covariate's names
	 "_______<name>            ", string   , 1:1 , Name of the covariate for the specified language
	 "____<description>        ",          , 1:1 , The translated covariate's descriptions
	 "_______<desc>            ", string   , 1:1 , Description of the covariate for the specified language
	 "____<unit>               ", string   , 1:1 , The covariate's unit
	 "____<covariateType>               ", :ref:`covariateType` , 1:1 , The covariate's type
	 "____<dataType>           ", :ref:`covariateDataType` , 1:1 , The covariate's type
	 "____<interpolationType>", :ref:`interpolationType` , 1:1 , The covariate's type
     "____<refreshPeriod>                 ", , 0:1 , "The refresh period for the covariate value"
     "________<unit>                 ", , 1:1 , "The unit of the refresh period"
     "________<value>                 ", , 1:1 , "The duration of the refresh period"
	 "____<covariateValue>", :ref:`stdAprioriValue`   , 1:1 , "The covariate's value, that can be modified by other covariates"
	 "____<validation>", ref:`validation` , 1:1 , Potential validation function for the covariate value
	 "____<comments>", :ref:`comments` , 1:1 , Comments about the covariate

|

The interpolation type allow to decide how the calculation interprets covariates changing over time.

The covariate type allows to indicate if the covariate is standard, related to birth, or related to sex.

The covariate data type allows to indicate the type in terms of int, double, bool or date.

The refresh period is used when the interpolation type is not direct. The period defines the time between two modifications of the real covariate value to be used for adjusting other parameters.

TODO : Check the next sentences

A covariate ID must identify the covariate. Some conventions allow to handle generic covariates and are described below. The name and desciption of the covariate can be translated using the *lang* attribute of the <name> and <desc> tags.

The <unit> of the covariate is the unit presented to the user - for example *kg* for the covariate *weight*. The <type>, on the other hand, is the internal type used to store the covariate's value. It can be either an int, a double or a boolean. In case of boolean, then the term is *bool*, and the value shall be 1 for true and 0 for false.


For genetic factors, it is suggested to use *gene_XXX* for the covariate ID, where *XXX* is the gene identification.

+-----------------+--------+-----+------------------------------------------------------+
| Covariate ID    |  Description                                                        |
+=================+========+=====+======================================================+
| birthbodyweight | weight of the patient at birth                                      |
+-----------------+---------------------------------------------------------------------+
| bodyweight      | current body weight of the patient                                  |
+-----------------+---------------------------------------------------------------------+
| age             | current age of the patient, in year                                 |
+-----------------+---------------------------------------------------------------------+
| pna             | for neonates, post natal age, in days                               |
+-----------------+---------------------------------------------------------------------+
| ga              | gestational age, in weeks                                           |
+-----------------+---------------------------------------------------------------------+
| clcr            | clearance of creatinine                                             |
+-----------------+---------------------------------------------------------------------+
| gist            | boolean indicating the presence of a gastrointestinal stromal tumor |
+-----------------+---------------------------------------------------------------------+
| sex             | sex of the patient (0: female, 1: male)                             |
+-----------------+---------------------------------------------------------------------+
| mic             | minimal inhibitory concentration                                    |
+-----------------+---------------------------------------------------------------------+

When it is not possible to know the value of a covariate for a specific patient, the default value is used instead. It corresponds to the value of the average individual, also called the typical patient.

.. _covariateType:

covariateType
^^^^^^^^^^^^^

Used by :ref:`covariate`.


Type of covariate



.. csv-table:: covariateType
   :header: "Value", "Description"
   :widths: 15, 30

   standard , "A normal covariate"
   sex , "The covariate represents the sex of the patient. Can be automatically retrieved from administrative data"
   ageInYears , "The age of the patient, in years. Can be automatically retrieved from administrative data"
   ageInMonths , "The age of the patient, in months. Can be automatically retrieved from administrative data"
   ageInDays , "The age of the patient, in days. Can be automatically retrieved from administrative data"

.. _covariateDataType:

covariateDataType
^^^^^^^^^^^^^^^^^

Used by :ref:`covariate`.

.. csv-table:: covariateDataType
   :header: "Value", "Description"
   :widths: 15, 30

   int , An integer
   double , A 64-bit double
   bool , A boolean value
   date , A date in format TODO


.. _interpolationType:

interpolationType
^^^^^^^^^^^^^^^^^

Used by :ref:`covariate`.

.. csv-table:: interpolationType
   :header: "Value", "Description"
   :widths: 15, 30

   direct , "As soon as a new covariate value exists, its value is applied"
   linear , "If the covariate has two values at two different times, a linear interpolation is applied between both time points"
   sigmoid , "If the covariate has two values at two different times, a sigmoid interpolation is applied between both time points"
   tanh , "If the covariate has two values at two different times, an hyperbolic tangent interpolation is applied between both time points"


.. _activeMoieties:

ActiveMoieties
==============



.. literalinclude:: drug_description/examples/activeMoiety_example.xml
	:language: xml


.. csv-table:: activeMoiety content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<activeMoiety>             ",          ,  , "An active moiety"
	 "____<activeMoietyid>                 ", string   , 1:1 , "active moiety unique identifier"
	 "____<activeMoietyName>               ",          , 1:1 , "The translated active moiety's names"
	 "_______<name>            ", string   , 1:∞ , "Name of the active moiety for the specified language"
	 "____<unit>               ", string   , 1:1 , "The active moiety's unit"
	 "____<analyteIdList>               ",          , 1:1 , "The list of analytes influencing this active moiety"
	 "_______<analyteId>            ", string   , 1:∞ , "The Id of an analyte required to compute this active moiety"
	 "____<analytesToMoietyFormula>               ", :ref:`operation` , 1:1 , "The formula for calculating the active moiety concentration based on the analytes"
	 "____<targets>           ", :ref:`targets` , 1:1 , "A list of targets"
	 "________<target>           ", :ref:`target` , 0:∞ , "A target to be reached"


TODO : Comment on activeMoieties


.. _targets:
.. _target:

Targets
^^^^^^^


The targets of an active moiety are described in the <target> tag. It contains all the data about the targeted residual concentration, peak concentration and mean concentration.
The structure is as follows:


.. literalinclude:: drug_description/examples/target_example.xml
	:language: xml



.. csv-table:: target content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<target>             ",          ,  , "A target"
	 "____<targetType>                 ", :ref:`targetType`   , 1:1 , "Type of target, from an enumaration"
	 "____<targetValues>               ",          , 1:1 , "The translated active moiety's names"
	 "____<unit>               ", string   , 1:1 , "The target unit"
	 "________<min>            ", :ref:`stdAprioriValue`   , 1:1 , "Minimum targeted value"
	 "________<max>            ", :ref:`stdAprioriValue`   , 1:1 , "Maximum targeted value"
	 "________<best>            ", :ref:`stdAprioriValue`   , 1:1 , "Best targeted value"
	 "________<toxicityAlarm>            ", :ref:`stdAprioriValue`   , 1:1 , "Threshold over which an alarm shall be triggered"
	 "________<inefficacyAlarm>            ", :ref:`stdAprioriValue`   , 1:1 , "Threshold under which an alarm shall be triggered"
	 "________<mic>            ",    , 0:1 , "The MIC value, optional"
	 "____________<unit>               ", string   , 1:1 , "The MIC unit"
	 "____________<micValue>            ", :ref:`stdAprioriValue`   , 1:1 , "The MIC value"
	 "____<times>               ",          , 0:1 , "Time targets when required by the target type"
	 "________<unit>               ", string   , 1:1 , "The time unit"
	 "________<min>            ", :ref:`stdAprioriValue`   , 1:1 , "Minimum targeted time"
	 "________<max>            ", :ref:`stdAprioriValue`   , 1:1 , "Maximum targeted time"
	 "________<best>            ", :ref:`stdAprioriValue`   , 1:1 , "Best targeted time"
 	 "____<comments>       ", :ref:`comments`   , 1:1  , "Comments about the target"


.. _targetType:

TargetType
^^^^^^^^^^

A target can be of any of these types:

.. csv-table:: targetType
   :header: "Value", "Description"
   :widths: 15, 30

   peak , "The target is the peak concentration. Times are to be added in the target to define when the peak should be found"
   residual , "The target is the residual concentration"
	 mean , "The target is the mean concentration"
	 auc , "The target is the area under curve, for a single intake cycle"
	 auc24 , "The target is the area under curve on 24h"
	 cumulativeAuc , "The target is the cumulative area under curve since the beginning of the treatment"
     aucOverMic , "The target is the area under the concentration curve, but only the portion over the MIC, for a single intake cycle"
     auc24OverMic , "The target is the area under the concentration curve, but only the portion over the MIC, for 24h"
	 timeOverMic , "The target is the time spent over the MIC, for a single intake cycle"
	 aucDividedByMic , "The target is the area under the concentration curve divided by the MIC, for a single intake cycle"
	 auc24DividedByMic , "The target is the area under the concentration curve for 24h divided by the MIC"
	 peakDividedByMic , "The target is the value of the peak concentration divided by the MIC"


In case of an aucOverMic, timeOverMix, aucDividedByMic and peakDividedByMic target, a covariate called *mic* has to be added in the drugModel.


	A target can be one of the three types *residual*, *peak* or *auc*. Consequently, there can be a maximum of four different targets for a single drug. All four must contain a minimum, maximum and best targeted concentration, but only the *peak* target will use the targeted times. The <concentrations> and <times> tags possess a *unit* attribute that must be filled in.


	The formulae are used to adapt the targeted values using, for example, the patient's covariates, but can be left blank if the values themselves are sufficient. Please note that appart from being used to display indicators in the curve panel, the targets will also be used by the reverse engines to find the appropriate dosage. If you do not provide any, the reverse engines will not be able to provide a solution.




.. _analyteGroups:

Analyte groups
==============



.. literalinclude:: drug_description/examples/analyteGroups_example.xml
	:language: xml


.. csv-table:: analyteGroups content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<analyteGroups>             ",          , 0:∞ , List of groups of analytes
	 "____<analyteGroup>                 ", string   , 1:∞ , A group a analytes
	 "________<groupId>               ",    string      , 1:1 , A unique Id for the group of analytes
	 "________<pkModelId>            ", string   , 1:1 , The Id of the Pk Model to be used for computation related to this group
	 "________<analytes>        ",          , 1:1 , The list of analytes of the group
	 "____________<analyte>            ", :ref:`analyte`   , 1:∞ , An analyte
	 "________<dispositionParameters>               ", , 1:1 , A set of disposition parameters
	 "____________<parameters>            ",   , 1:1 , List of parameters
	 "________________<parameter>            ", :ref:`parameter`   , 1:∞ , A disposition parameter
	 "____________<correlations>            ", :ref:`correlations`   , 1:1 , correlation between disposition parameters

|


The *groupId* uniquely identifies the group within the drugModel. It is required by the :ref:`formulationAndRoutes` in order to indicate on which group the various parameters are connected to.

The drug's Pk model ID must reference an existing pharmacokinetic model.
The list of models and their corresponding parameters are presented below.



+------------------------+---------------------------------------------+
| Pk Model ID            | Description                                 |
+========================+=============================================+
| std.linear.1comp.macro | linear elimination, 1 compartment           |
+------------------------+---------------------------------------------+
| std.linear.1comp.micro | linear elimination, 1 compartment           |
+------------------------+---------------------------------------------+
| std.linear.2comp.macro | linear elimination, 2 compartment           |
+------------------------+---------------------------------------------+
| std.linear.2comp.micro | linear elimination, 2 compartment           |
+------------------------+---------------------------------------------+
| std.linear.3comp.macro | linear elimination, 3 compartment           |
+------------------------+---------------------------------------------+
| std.linear.3comp.micro | linear elimination, 3 compartment           |
+------------------------+---------------------------------------------+

The difference between *macro* and *micro* model consists in the parameters supplied. In case of *macro* the parameters are typically clearance and volume, while in case of *micro* the parameters are the micro constants k.

.. _analytes:

.. _analyte:

Analyte
^^^^^^^^



.. literalinclude:: drug_description/examples/analyte_example.xml
	:language: xml


.. csv-table:: analyte content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<analyte>             ",          , 0:∞ , An analyte
	 "____<analyteId>                 ", string   , 1:∞ , The Id of the analyte
	 "________<unit>               ",    string      , 1:1 , The unit used to do calculation with the analyte
	 "________<molarMass>            ", string   , 1:1 , The molar mass of the analyte
	 "____________<value>        ",          , 1:1 , Value of the molar mass
	 "____________<unit>            ", :ref:`analyte`   , 1:∞ , Unit of the molar mass
	 "________<description>               ",    , 1:1 , The translated descriptions of the analyte
	 "____________<desc>", string , 1:∞ , Description of the analyte
	 "________<errorModel>               ", :ref:`errorModel`   , 1:1 , The error model corresponding to this analyte
	 "________<comments>               ", :ref:`comments`   , 1:1 , Comments about this analyte

|



.. _errorModel:

Error model
^^^^^^^^^^^

The error model is the representation of the intra-individual error.
It is used by the post engines and the percentiles engines to regulate the patient's measures and intra-individual variability.

The structure is the following:


.. literalinclude:: drug_description/examples/errorModel_example.xml
	:language: xml

|



.. csv-table:: errorModel content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<errorModel>             ",          ,  , Some time considerations
		 "____<errorModelType>                 ", :ref:`errorModelType`   , 1:1 , "Type of error model, an enum"
		 "____<errorModelFormula>               ",  :ref:`operation`        , 0:1 , A formula if required by the errorModelType
		 "____<sigmas>            ",    , 1:1 , A list of sigmas
		 "________<sigma>        ",     double     , 1:∞ , A sigma used by the error model
		 "____<comments>            ", :ref:`comments`   , 1:1 , Comments about the error model

|

.. _errorModelType:

errorModelType
^^^^^^^^^^^^^^

Used by :ref:`errorModel`.

.. csv-table:: errorModelType
   :header: "Value", "Description"
   :widths: 15, 30

	 additive , "An additive error model. Requires a single sigma"
	 proportional , "A proportional error model. Requires a single sigma"
	 exponential , "An exponential error model. Requires a single sigma"
	 mixed , "A mixed error model. Requires two sigmas"
   softcoded , "The error model is defined by the formula following the declaration of the errorModelType"

For each error model except the **softcoded** one, the model is implemented in the software. For a **softcoded**, the formula supplied in the file is used instead.



.. _formulationAndRoutes:

.. _formulationAndRoute:

Formulation and route
=====================


.. literalinclude:: drug_description/examples/formulationAndRoute_example.xml
	:language: xml



.. csv-table:: formulationAndRoute content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<formulationAndRoute>             ",          ,  , Formulation and route
		 "____<formulationAndRouteId>                 ", string  , 1:1 , "Id of the formulation and route"
		 "____<formulation>               ",  string     , 1:1 , "the formulation. Taken from a dictionary"
		 "____<administrationName>            ",  string  , 1:1 , "A free field to discriminate vendors"
		 "____<administrationRoute>        ",     string     , 1:1 , "The route of administration, taken from a dictionary"
		 "____<absorptionModelId>            ", :ref:`absorptionModelId`   , 1:1 , Id of the absorption model
		 "____<dosages>            ", :ref:`dosages`   , 1:1 , Possible dosages
		 "____<absorptionParameters>            ",  , 1:1 , Sets of absorption parameters
		 "________<parameterSet>            ",    , 1:∞ , A set of absorption parameters for an analyte group
		 "________<analyteGroupId>            ", string   , 1:1 , Id of the analyte group
		 "________<parameters>            ", :ref:`comments`   , 1:1 , List of parameters
		 "____________<parameter>            ", :ref:`comments`   , 0:∞ , A parameter
		 "________<correlations>            ", :ref:`correlations`   , 1:1 , Correlations between absorption parameters or between absorption and disposition parameters
		 "____________<correlation>            ", :ref:`correlation`   , 1:1 , Correlations between absorption parameters or between absorption and disposition parameters


The *formulationAndRouteId* is an Id identifying this formulation and route.

The *formulation* is taken from a dictionary that still has to be defined. Example: *parenteral solution*.

For now it supports the following values:

* "undefined"
* "parenteral solution"
* "oral solution"

The *administrationName* is a free string, and can allow to differentiate between identical formulations offered by different vendors.

The *administrationRoute* is taken from a dictionary that still has to be defined. Example : *i.v.*.

For now it supports the following values:

* "undefined"
* "intramuscular"
* "intravenousBolus"
* "intravenousDrip"
* "nasal"
* "oral"
* "rectal"
* "subcutaneous"
* "sublingual"
* "transdermal"
* "vaginal"

The *absorptionModelId* can be either *extra*, *infu* or *bolus*, respectiveley for extravascular, infusion or bolus.

.. _absorptionModelId:

absorptionModelId
^^^^^^^^^^^^^^^^^

.. csv-table:: absorptionModelId
   :header: "Value", "Description"
   :widths: 15, 30

   extra , "Extravascular route. Can be used for various administration routes, like in a muscle, oral, anal, ..."
   infusion , "Infusion in the central compartment"
   bolus , "Immediate availability of the drug in the blood, like an intravenous bolus."


The *dosages* are the possible dosages, used for proposing dosage adaptation.

Finally, the *absorptionParameters* are the absorption parameters corresponding to the *absorptionModelId* selected.

For these absorption parameters, the *analyteGroupId* allows to identify the analytes group related to the parameter set.

.. _dosages:

Dosages
^^^^^^^

This section contains all the information about the dosages, such as the default units and values of the doses, intervals and infusions. It also contains the lists of doses, intervals and infusions used by the dosage adaptation module [#d1]_. The dosages structure looks like this:



.. literalinclude:: drug_description/examples/dosages_example.xml
	:language: xml



.. csv-table:: dosages content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<dosages>             ",          ,  , "Available dosages associated with a formulation and route"
		 "____<standardTreatment>                 ", , 0:1 , "A potential standard treatment"
		 "________<isFixedDuration>                 ", bool, 1:1 , "Has the treatment a fixed duration?"
		 "________<timeValue>                 ", , 0:1 , "The duration of the fixed duration"
		 "____________<unit>                 ", , 1:1 , "The unit of the duration"
		 "____________<value>                 ", , 1:1 , "The duration value"
		 "____<analyteConversions>                 ", , 1:1 , "List of analyte conversions"
		 "________<analyteConversion>               ",       , 1:∞ , "Conversion from the quantity of drug to the quantity of analyte"
		 "____________<analyteId>            ",  string  , 1:1 , "The Id of the analyte"
		 "____________<factor>        ",     double     , 1:1 , "The factor to be multiplied to the drug quantity to obtain the analyte quantity"
		 "____<availableDoses>            ", :ref:`availableDoses`   , 1:1 , Available doses
		 "____<availableIntervals>            ", :ref:`availableIntervals`   , 1:1 , Available intervals
		 "____<availableInfusions>            ", :ref:`availableInfusions`   , 0:1 , Available infusion times


The analyte conversion is important if not all the drug is part of a single analyte. In that case the factor allows to link the quantity of analyte corresponding to a certain quantity of drug.

The available doses, intervals and infusions are used by the dosage adaptation engine to propose a suitable dosage.


.. _availableDoses:

AvailableDoses
^^^^^^^^^^^^^^

.. csv-table:: dosages content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<availableDoses>             ",          ,  , "Available doses"
		 "____<unit>                 ", , 1:1 , "Unit of the doses"
		 "____<default>               ",  ref:`stdAprioriValue`     , 1:1 , "Default dose"
		 "____<rangeValues>               ",      , 0:1 , "Available doses represented as a range"
		 "________<from>               ",  ref:`stdAprioriValue`     , 1:1 , "Starting value of the range"
		 "________<to>               ",  ref:`stdAprioriValue`     , 1:1 , "Ending value of the range"
		 "________<step>               ",  ref:`stdAprioriValue`     , 1:1 , "Step to be applied between from and to"
		 "____<fixedValues>            ",    , 0:1 , "A list of fixed doses"
		 "________<value>        ",     double     , 1:1 , "A dose value"

The idea here is to use *rangeValues* OR *fixedValues*, but not both at the same time. However the software support both at the same time. Using :ref:`stdAprioriValue` fields allow the dose range to be very flexible, depending on the patient covariates.

If *rangeValues* is used, then the dosage adaptation engine will try every dose between the boundaries *from* and *to*, using *step*. For instance, if *from=5*, *to=25*, and *step=10*, then the values will be 5, 15, and 25.

.. _availableIntervals:

AvailableIntervals
^^^^^^^^^^^^^^^^^^


.. csv-table:: dosages content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<availableIntervals>             ",          ,  , "Available intervals"
		 "____<unit>                 ", , 1:1 , "Unit of the intervals"
		 "____<default>               ",  ref:`stdAprioriValue`     , 1:1 , "Default interval"
		 "____<rangeValues>               ",      , 0:1 , "Available intervals represented as a range"
		 "________<from>               ",  ref:`stdAprioriValue`     , 1:1 , "Starting value of the range"
		 "________<to>               ",  ref:`stdAprioriValue`     , 1:1 , "Ending value of the range"
		 "________<step>               ",  ref:`stdAprioriValue`     , 1:1 , "Step to be applied between from and to"
		 "____<fixedValues>            ",    , 0:1 , "A list of fixed intervals"
		 "________<value>        ",     double     , 1:1 , "An interval value"

The idea here is to use *rangeValues* OR *fixedValues*, or both at the same time. Using :ref:`stdAprioriValue` fields allow the dose range to be very flexible, depending on the patient covariates.

If *rangeValues* is used, then the dosage adaptation engine will try every interval between the boundaries *from* and *to*, using *step*. For instance, if *from=5*, *to=25*, and *step=10*, then the values will be 5, 15, and 25.

.. _availableInfusions:

AvailableInfusions
^^^^^^^^^^^^^^^^^^


.. csv-table:: dosages content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<availableInfusions>             ",          ,  , "Available infusion times"
		 "____<unit>                 ", , 1:1 , "Unit of the infusion times"
		 "____<default>               ",  ref:`stdAprioriValue`     , 1:1 , "Default infusion time"
		 "____<rangeValues>               ",      , 0:1 , "Available infusion times represented as a range"
		 "________<from>               ",  ref:`stdAprioriValue`     , 1:1 , "Starting value of the range"
		 "________<to>               ",  ref:`stdAprioriValue`     , 1:1 , "Ending value of the range"
		 "________<step>               ",  ref:`stdAprioriValue`     , 1:1 , "Step to be applied between from and to"
		 "____<fixedValues>            ",    , 0:1 , "A list of fixed infusion times"
		 "________<value>        ",     double     , 1:1 , "An infusion time value"

The idea here is to use *rangeValues* OR *fixedValues*, or both at the same time. Using :ref:`stdAprioriValue` fields allow the infusion time range to be very flexible, depending on the patient covariates.

If *rangeValues* is used, then the dosage adaptation engine will try every infusion time between the boundaries *from* and *to*, using *step*. For instance, if *from=5*, *to=25*, and *step=10*, then the values will be 5, 15, and 25.


.. rubric:: Notes

.. [#d1] A dosage adaptation module is used by Tucuxi to propose and adapt the dosage of a drug, given its targets and the patient's data.




.. _timeConsiderations:

Time considerations
===================


Used by :ref:`drugModel`.

Time considerations can help the software to optimize some computations, and also to get information about the relevance of a measure.

Here is an example of time consideration:

.. literalinclude:: drug_description/examples/timeConsiderations_example.xml
	:language: xml


The half-life describes the time it takes for the plasma concentration, or the amount of drug in the body, to be reduced by 50%. Therefore, in each succeeding half-life, less drug is eliminated. After one half-life the amount of drug remaining in the body is 50%, after two half-lives 25%, etc. After 4 half-lives the amount of drug (6.25%) is considered to be negligible regarding its therapeutic effects.

The second part of the time considerations consists in the time after which a measure is considered irrelevant, and shall not be used for a posteriori computations.

.. csv-table:: timeConsiderations content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<timeConsideration>             ",          ,  , Some time considerations
		 "____<halfLife>                 ", halfLife   , 1:1 , Half life of the drug
		 "_______<unit>               ",          , 1:1 , Time unit of the half life
		 "_______<duration>            ", :ref:`stdAprioriValue`   , 1:1 , value of the half life
		 "_______<multiplier>        ",          , 1:1 , Number of half lifes to reach steady state
		 "_______<comments>            ", :ref:`comments`   , 1:1 , "Comments about the half life"
		 "____<outdatedMeasure>            ", outdatedMeasure   , 1:1 , Indication about the relevance of a measure
		 "_______<unit>               ", string   , 1:1 , Time unit
		 "_______<duration>               ", :ref:`stdAprioriValue` , 1:1 , Time after which a measure shall be considered as irrelevant
		 "_______<comments>            ", :ref:`comments`   , 1:1 , "Comments about the outdate measure fields"



The half-life was used to determine the residual concentration of a drug at steady-state.
The half-life duration given above is multiplied by the cycle multiplier in order to find out how many cycles need to be completed before reaching the steady-state. It is then possible to compute the residual concentration of the drug at steady-state. In most cases, a multiplier of 10 is sufficient.

















.. _parameter:

Parameter
=========

The parameters are used by the models to compute the curves and their value depend on the type of the prediction.
If the prediction is made for the typical patient, the model will use the population parameters.
If the prediction is made *a priori* for a specific patient, the population parameters will be adapted using patient's covariates.
Finally, if the prediction is made *a posteriori*, the *a priori* parameters will be adapted using the patient's measures and a post engine. The parametrs are declared this way:



.. literalinclude:: drug_description/examples/parameter_example.xml
	:language: xml



.. csv-table:: parameter content
   :header: "Tag name", "Format", "Occ.", "Description"
   :widths: 15, 10, 5, 30

 	 "<parameter>             ",          ,  , "A Pk parameter"
		 "____<parameterId>                 ", string  , 1:1 , "Id of the parameter"
		 "____<unit>               ",  string     , 1:1 , "the unit of the parameter value"
		 "____<parameterValue>            ",  :ref:`stdAprioriValue`  , 1:1 , "The parameter value and its optional apriori computation"
		 "____<bsv>        ",     string     , 1:1 , "Between Subject Variability (BSV) "
		 "________<bsvType>            ", :ref:`bsvType`   , 1:1 , "Type of BSV"
		 "________<stdDevs>            ",  , 1:1 , "A list of standard deviations"
		 "____________<stdDev>            ", double , 1:∞ , "A standard deviation"
		 "____<validation>            ",  , 1:1 , "A potential validation of the parameter value"
		 "________<errorMessage>            ",    , 1:∞ , "A translated list of error messages"
		 "____________<text>            ", string   , 1:1 , "An error message, translated in a specific language"
		 "________<formula>            ", :ref:`operation`   , 1:1 , "A formula to check the validity of the parameter"
		 "________<comments>            ", :ref:`comments`   , 1:1 , "Comments about the validation"
		 "____<comments>            ", :ref:`comments`   , 1:1 , "Comments about the parameter"


The default value of the parameter corresponds to the typical patient's value, and is used by the model as the population parameter. The parameters identifier must match one of the parameter IDs used by the model.

.. warning:: Please check carefully the ID of the parameter with respect to the selected model.


.. _bsvType:

BsvType
^^^^^^^

.. csv-table:: bsvType
   :header: "Value", "Description"
   :widths: 15, 30

	 none , "The parameter does not have variability. It is fixed"
	 normal , "The parameter variability follows a Normal distribution"
	 lognormal , "The parameter variability follows a LogNormal distribution"

For bsv type, the model is implemented in the software.




.. _correlation:

.. _correlations:

Correlations
============

With the help of the BSV (Between Subject Variability) of each parameter, the correlations are used to build the correlation and covariance matrices. It is possible to express correlations between two parameters in the following structure:

.. code-block:: xml

	<correlations>
	  <correlation>
	    <param1>CL</param1>
	    <param2>V</param2>
	    <value>0.798</value>
	    <comments></comments>
	  </correlation>
	</correlations>

|

+---------------+--------+-----+-----------------------------------------+
| Tag name      | Format | Occ.| Description                             |
+===============+========+=====+=========================================+
| <correlation> |        | 0:∞ | Description of the correlation          |
+---------------+--------+-----+-----------------------------------------+
|____<param1>   | string | 1:1 | The first parameter's ID                |
+---------------+--------+-----+-----------------------------------------+
|____<param2>   | string | 1:1 | The second parameter's ID               |
+---------------+--------+-----+-----------------------------------------+
|____<value>    | double | 1:1 | The correlation's value                 |
+---------------+--------+-----+-----------------------------------------+
|____<comments> |        | 1:1 | Comments about the correlation          |
+---------------+--------+-----+-----------------------------------------+

|

The first and second parameters IDs must match those in the parameters section.
