//@@license@@

#include <gtest/gtest.h>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"

#include "tucucore/dosage.h"

#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucuquery/queryimport.h"

#include "gtest_query.h"

std::unique_ptr<Tucuxi::Query::Treatment> GtestTucuQuery::importXml(std::string _xmlString)
{
    Tucuxi::Query::QueryImport queryImport;
    Tucuxi::Common::XmlDocument xmlDocument;

    EXPECT_TRUE(xmlDocument.fromString(_xmlString));

    Tucuxi::Common::XmlNodeIterator xmlNodeIterator = xmlDocument.getRoot().getChildren("treatment");

    return queryImport.createTreatment(xmlNodeIterator);
}

void GtestTucuQuery::exportXml(std::unique_ptr<Tucuxi::Query::Treatment> _pTreatment, std::string& _xmlString)
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

void GtestTucuQuery::removeSpecialsCharacters(std::string& _exportedXmlString)
{
    std::vector<char> specialCharacters{'\t', '\n', '\r'};

    for (char characters : specialCharacters) {
        _exportedXmlString.erase(
                std::remove(_exportedXmlString.begin(), _exportedXmlString.end(), characters),
                _exportedXmlString.end());
    }
}

void GtestTucuQuery::printImportExportString(std::string& _importString, std::string& _exportString)
{
    std::cout << std::endl;
    std::cout << "Imported String : " << std::endl << std::endl;
    std::cout << _importString << std::endl;
    std::cout << std::endl;
    std::cout << "Exported String : " << std::endl << std::endl;
    std::cout << _exportString << std::endl << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
