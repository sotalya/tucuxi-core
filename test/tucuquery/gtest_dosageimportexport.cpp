//@@license@@

#include <gtest/gtest.h>

#include "tucucommon/xmldocument.h"

#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucuquery/queryimport.h"
#include "tucuquery/parametersdata.h"

#include "gtest_queryinputstrings.h"

#define PRINT_MESSAGE(x)    (std::cout << "\033[1;36m" << x << "\033[0m" << std::endl)



using namespace Tucuxi;
using namespace Query;

class GTestTucuQuery{
public:
    static std::unique_ptr<Tucuxi::Query::Treatment> importXml(std::string _xmlString)
    {
        Tucuxi::Query::QueryImport queryImport;
        Tucuxi::Common::XmlDocument xmlDocument;

        EXPECT_TRUE(xmlDocument.fromString(_xmlString));

        Tucuxi::Common::XmlNodeIterator xmlNodeIterator = xmlDocument.getRoot().getChildren("treatment");

        return queryImport.createTreatment(xmlNodeIterator);
    }

    static void exportXml(std::unique_ptr<Tucuxi::Query::Treatment> _pTreatment, std::string& _xmlString)
    {
        Tucuxi::Query::ComputingQueryResponseXmlExport computingXmlExport;

        Tucuxi::Common::XmlDocument xmlDocument;

        Tucuxi::Common::XmlNode root = xmlDocument.createNode(Tucuxi::Common::EXmlNodeType::Element, "root");
        xmlDocument.setRoot(root);

        Tucuxi::Common::XmlNode treatment = xmlDocument.createNode(Tucuxi::Common::EXmlNodeType::Element, "treatment");
        root.addChild(treatment);

        EXPECT_TRUE(computingXmlExport.exportDosageHistory(std::move(_pTreatment->getpDosageHistory()), treatment));

        xmlDocument.toString(_xmlString, true);
    }

    static void removeSpecialsCharacters(std::string& _exportedXmlString)
    {
        std::vector<char> specialCharacters{'\t', '\n', '\r'};

        for (char characters : specialCharacters) {
            _exportedXmlString.erase(
                    std::remove(_exportedXmlString.begin(), _exportedXmlString.end(), characters),
                    _exportedXmlString.end());
        }
    }

    static void printImportExportString(std::string& _importString, std::string& _exportString)
    {
        std::cout << std::endl;
        std::cout << "Imported String : " << std::endl << std::endl;
        std::cout << _importString << std::endl;
        std::cout << std::endl;
        std::cout << "Exported String : " << std::endl << std::endl;
        std::cout << _exportString << std::endl << std::endl;
    }

};


TEST (Query_DosageImportExportTests, Test1){
    Tucuxi::Query::QueryImport queryImport;
    Tucuxi::Common::XmlDocument xmlDocument;
    GTestTucuQuery gTestTucuQuery;

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
