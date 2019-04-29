*******************
Drug file validator
*******************

The drugfilevalidator application is meant to validate the drug files to ensure their data are correctly set. The drug file editor does some basic checks but is not able to deeply challenge the data. A second step of validation consists in a simple checker within the GUI application. Pressing CTRL+SHIFT+F2 allows to perform some checks on the drug file.

At the current stage the validator is able to check some integrity of the drug model and the a priori parameters computations. It requires two inputs: a drug file and a test file. A test file has the .dvt extension and contains test vectors in a json format.

The command line is the following:

.. code-block:: bash

    Usage:
        ./tucuvalidator [OPTION...]

            -d, --drugfile arg  Drug file
            -t, --testfile arg  Tests to be conducted
            -l, --logfile arg   Log file
            --help          Print help

The drug file is a .tdd file containing a drug model. The test file is a .dvt file (described in the next section). The log file will contain the output log.

DVT files
#########

The content of such a drug file is as follows:

.. code-block:: javascript

	{
	    "parameterstests": [
	        {
	            "testId": "1",
	            "sampleDate": "2018-01-03T0:0:0",
	            "parameters": [
	            ],
	            "comment": "test 1",
	            "covariates": [
	            ],
	        },
	        {
	            "testId": "2",
	            ...
	        },
	        ...
	    ]
	}

The testId is used to report errors. The comment is currently not used but serves as a documentation for the test file.

The sample date is in the form YYYY-MM-DDTHH:MM:SS". This field is important if age is a covariate of the drug file. If this is the case, the test file will have to contain a birthdate variate. The birthdate and the sample date are used by Tucuxi to build the age (in days, weeks or years). The birthdate is in the form YYYY-MM-DD.

The covariates have the following fields:

.. code-block:: javascript

	"covariates": [
		{
		    "dataType": "date",
		    "unit": "-",
		    "value": "1948-01-03",
		    "id": "birthdate"
		},
		{
		    "dataType": "double",
		    "unit": "-",
		    "value": "0",
		    "id": "sex"
		},
		{
		    "dataType": "double",
		    "unit": "kg",
		    "value": "70.0",
		    "id": "bodyweight"
		},
	]


Their id correspond to the Id expected as patient variates . They normally are the same as the covariates in the drug file except for the birthdate. A birthdate is not a drug file covariate, as the drug file embeds an age covariate for calculation.

Not all covariates need to be set. If this is not the case then Tucuxi will perform computation with the default values.

The parameters are simply identified by their Id and associated with the expected value. They have the following fields:

.. code-block:: javascript

	"parameters": [
	{
	    "value": 11.292000000000002,
	    "id": "CL"
	},
	{
	    "value": 300.8,
	    "id": "V"
	}
	],

The validator checks the parameter value with the one it calculates and issues an error if they do not match. As these values are floating point numbers a delta of 1e-4 is tolerated for the comparison.


python scripts
##############

A dvt file can be hand written, but this task is quite long and not very efficient. Therefore python scripts are used to generate these files. There is one file per drug model, each of them following the same template.

Here we explain the content of a standard python file following the example of imatimib.

First some imports:

.. code-block:: python

    import json;
    from commonfunctions import newTest, newFullTest, setDefaults,
                                ageInYears, ageInDays, ageInWeeks, getBoolean;

The file **commonfunctions.py** contains useful functions used by every drug python file.

Then we define the default covariates values:

.. code-block:: python


    defCovariates = [
        {
            "id" : "birthdate",
            "dataType" : "date",
            "value" : "1968-01-01",
            "unit" : "-"
        },
        {
            "id" : "sex",
            "dataType" : "double",
            "value" : "0.5",
            "unit" : "-"
        },
        {
            "id" : "bodyweight",
            "dataType" : "double",
            "value" : "70",
            "unit" : "kg"
        },
        {
            "id" : "gist",
            "dataType" : "bool",
            "value" : "0",
            "unit" : "-"
        }
    ];



Here we can notice that the drug file uses an age covariate, and so here we define a birthdate covariate. We have to be careful with this date as it has to be coherent with the sample date. The difference between these two dates has to correspond to the default value of the age covariate of the drug file.!!

We then define the default parameters values:

.. code-block:: python

    defParameters = [
        {
            "id" : "CL",
            "value" : 15.0474
        },
        {
            "id" : "V",
            "value" : 393.2
        }
        ];


They simply correspond to the values found in the drug file.

We then set the defaults, by calling a function:

.. code-block:: python

    setDefaults(defCovariates, defParameters);

Then, we need to declare and implement a function to calculate parameters based on the covariates and the sample date:

.. code-block:: python

    def calculateParameters(sampleDate, covariates):
        # Get the covariates
        age = ageInYears(covariates[0], sampleDate);
        sex = float(covariates[1]);
        bodyweight = float(covariates[2]);
        gist = getBoolean(covariates[3]);

        # Define some thetas
        theta1 = 14.3;
        theta4 = 5.42;
        theta5 = 1.49;
        theta6 = -5.81;
        theta7 = -0.806;

        # Perform the calculation thanks to python math
        MEANBW = 70;
        FBW = (bodyweight - MEANBW) / MEANBW;

        MEANAG = 50.0;
        FAGE = (age - MEANAG) / MEANAG;

        if (gist):
            PATH = 1;
        else:
            PATH = 0;

        MALE = sex;

        CL = theta1 + theta4 * FBW + theta5 * MALE-theta5 * (1-MALE) + theta6 * FAGE + theta7 * PATH - theta7 * (1 - PATH);

        theta2 = 347;
        theta8 = 46.2;
        V = theta2 + theta8 * sex - theta8 * (1 - sex);

        # Create the parameters list
        parameters = [];
        parameters.append(CL);
        parameters.append(V);
        return parameters;

This calculation function is the core of the python file. It should implement the equations found in the published paper.
The covariates have an order that needs to respect the order of the default covariates previously defined. The same apply for the parameters.

Now we can simply add the tests we want to perform, in an array, using the **newFullTest** function.

This function takes 5 parameters:

1. A test Id
2. A comment
3. A sample date
4. An array of covariate values. The array has the same size as the default covariates array, and shall respect the same order. It is possible to set a covariate value to "-" in order to use the default value.
5. A function for calculating the parameters (here we use the one previously defined)

.. code-block:: python

    parameterstests = [];
    parameterstests.append(newFullTest("1","test 1","2018-01-03T0:0:0",["1948-01-03","-","70.0","0"], calculateParameters));
    parameterstests.append(newFullTest("2","test 2","2018-01-03T0:0:0",["1968-01-03","1","50.0","0"], calculateParameters));
    parameterstests.append(newFullTest("3","test 3","2018-01-03T0:0:0",["1968-01-03","0","50.0","1"], calculateParameters));

Finally, create the final json from the parameterstests and print it on stdout:

.. code-block:: python

    jsonTest = {"parameterstests": parameterstests};

    print(json.dumps(jsonTest,indent=4))
