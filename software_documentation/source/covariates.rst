**********
Covariates
**********

The covariates allow to adjust the prediction thanks to some information on the
patient.

Some specific covariates are:

- Sex
- Age

Sex
---


Sex is represented as a floating point value in the range [0,1]. The rationale
is that it allows to have continuous formula taking advantage of the mean value.
If we do not know the person's sex, then how would we choose if he/she is supposed
to be a man or a woman? Using a floating point allows to set the value to 0.5
if that information is missing. Therefore the prediction will correspond to
an individual being an intermediate between a man and a woman.

Sex is considered as a special covariate in the sense that it could be retrieved
from administrative data about the patient.

The following code illustrates a sex covariate. The only mandatory fields
are the `covariateType` (`sex`), and the `dataType` (`double`).

.. code-block:: xml

    <covariate>
        <covariateId>sex</covariateId>
        <covariateName>
            <name lang="en">Sex</name>
            <name lang="fr">Sexe</name>
        </covariateName>
        <description>
            <desc lang="en">Sex of the patient</desc>
            <desc lang="fr">Sexe du patient</desc>
        </description>
        <unit></unit>
        <covariateType>sex</covariateType>
        <dataType>double</dataType>
        <interpolationType>direct</interpolationType>
        <covariateValue>
            <standardValue>0.5</standardValue>
        </covariateValue>
        <validation>
            <errorMessage>
                <text lang="en">Sex should be in the range [0,1].</text>
            </errorMessage>
            <operation>
                <softFormula>
                    <inputs>
                        <input>
                            <id>sex</id>
                            <type>double</type>
                        </input>
                    </inputs>
                    <code><![CDATA[
                        return ((sex >= 0.0) && (sex <= 1.0));
                        ]]>
                    </code>
                </softFormula>
            <comments/>
            </operation>
            <comments/>
        </validation>
        <comments/>
    </covariate>

Age
---

The age of the patient corresponds to a special covariate, because of two factors:

1. As for the sex, it can be retrieved from administrative data, that is the birthdate.
2. In the drug models it can be used in years, months or days.

In a drug model, when an age is required, it shall be using the special type
to specify if it is ageInYears, ageInMonths or ageInDays. A default value has
to be indicated as well.

In the patient data that will be scanned to get the covariates used for calculation,
there are two means of specifiying the age. It can be of the same type as the
drug model covariates, or it can come from a birthdate.

In case the patient data contains the birthdate, it should have the id **birthdate**

Tucuxi is then able to calculate the age based on the birthdate, following the
granularity defined in the drug model covariate. It will therefore be automatically
updated by the software. For instance, in a model of gentamicin for neo-nates,
the ageInDays will be updated every day automatically.

Here is an example of an age covariate in a drug model file:


.. code-block:: xml

    <covariate>
        <covariateId>age</covariateId>
        <covariateName>
            <name lang="en">Age</name>
            <name lang="fr">Age</name>
        </covariateName>
        <description>
            <desc lang="en">Age of the patient, in years</desc>
            <desc lang="fr">Âge du patient, en années</desc>
        </description>
        <unit>y</unit>
        <covariateType>ageInYears</covariateType>
        <dataType>double</dataType>
        <interpolationType>direct</interpolationType>
        <covariateValue>
            <standardValue>50</standardValue>
        </covariateValue>
        <validation>
            <errorMessage><text lang="fr"></text></errorMessage>
            <operation>
                <softFormula>
                    <inputs>
                        <input>
                            <id>age</id>
                            <type>double</type>
                        </input>
                            </inputs>
                    <code><![CDATA[return ((age >= 20) && (age <= 88));
                        ]]>
                    </code>
                </softFormula>
                <comments/>
            </operation>
            <comments/>
        </validation>
        <comments/>
    </covariate>
