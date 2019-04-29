**********************
Command line interface
**********************

The command line interface allows to perform calculations by submitting a query
file. The output is then written in files in a folder specified as a parameter.

A typical call looks like this:

.. code-block:: bash

    ./tucucli -d <drugPath> -i <inputFile> -o <outputPath>

The help is displayed by calling

.. code-block:: bash

    ./tucucli --Help

The input file and the output folder are mandatory. The input file is a full
query, as specified by file `xml_query.xsd`. This file can be found in `test/data/queries` .

The drug path is optional, and allows to specify the folder in which the drug files are located.
If unspecified, the drug files are supposed to be in `<app_folder>/drugs2`.

A query corresponds to the specification of Nadir's bachelor thesis.

The output folder shall exist prior to the execution of tucucli. It will be
populated with a single file per request, as a query can contain more than one
request. The name of each file is `<query_id>_<request_id>.xml`. It allows to
create testcases for automated tests.
