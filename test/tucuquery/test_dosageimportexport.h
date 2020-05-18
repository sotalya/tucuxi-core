/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <string>
#include <iostream>
#include <stdexcept>

#include "fructose/fructose.h"

#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucuquery/queryimport.h"


struct TestDosageImportExport : public fructose::test_base<TestDosageImportExport>
{

    void testall(const std::string& _testName)
    {
        // The tests are exploiting the importer and the exporter, to validate the
        // dosage import/export.
        // From an XML string, import it to a dosage history, and then
        // export it to another XML string.
        // Then use string comparison to check wether they are identical or not


    }

};
