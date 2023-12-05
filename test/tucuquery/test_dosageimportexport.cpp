//@@license@@

#include "tucucommon/xmldocument.h"

#include "tucuquery/queryimport.h"

#include "gtest_queryinputstrings.h"
#include "gtest_query.h"

#define PRINT_MESSAGE(x)    (std::cout << "\033[1;36m" << x << "\033[0m" << std::endl)


TEST (TucuTestQuery, DosageImportExportTests){
    Tucuxi::Query::QueryImport queryImport;
    Tucuxi::Common::XmlDocument xmlDocument;
    GtestTucuQuery gTestTucuQuery;

    int iteration = 0;
    for (std::string xmlInput : xmlInputTests){
        PRINT_MESSAGE("Testing string: " << iteration);

        std::unique_ptr<Tucuxi::Query::Treatment> xmlTreatment = gTestTucuQuery.importXml(xmlInput);

        std::string exportedXmlString;

        gTestTucuQuery.exportXml(std::move(xmlTreatment), exportedXmlString);

        gTestTucuQuery.removeSpecialsCharacters(exportedXmlString);
        gTestTucuQuery.removeSpecialsCharacters(xmlInput);

        int comparison1 = xmlInput.compare(exportedXmlString);

        EXPECT_EQ(comparison1, 0);

        if (comparison1 != 0) {
            gTestTucuQuery.printImportExportString(xmlInput, exportedXmlString);
        }
        iteration++;
    }
}
