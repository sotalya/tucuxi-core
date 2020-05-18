/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <string>
#include <iostream>
#include <stdexcept>

#include "fructose/fructose.h"

#include "tucuquery/computingqueryresponsexmlexport.h"
#include "tucuquery/queryimport.h"

#include "tucucore/dosage.h"
#include "tucucore/definitions.h"

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/utils.h"



struct TestDosageImportExport : public fructose::test_base<TestDosageImportExport>
{

    void testall(const std::string& _testName)
    {

        std::cout << _testName << std::endl;

        std::string ImportedXmlString = "<root>"
                                    "<treatment>"
                                        "<dosageHistory>"
                                            "<test></test><test2></test2>"
                                        "</dosageHistory>"
                                    "</treatment>"
                                "</root>";

        unique_ptr<Tucuxi::Query::Treatment> pTreatment = importXml(ImportedXmlString);

        //TESTS de l'objet Treatment

        std::string exportedXmlString;

        exportXml(std::move(pTreatment), exportedXmlString);


        // The tests are exploiting the importer and the exporter, to validate the
        // dosage import/export.
        // From an XML string, import it to a dosage history, and then
        // export it to another XML string.
        // Then use string comparison to check wether they are identical or not



    }

private:
    unique_ptr<Tucuxi::Query::Treatment> importXml(std::string _xmlString)
    {
        Tucuxi::Query::QueryImport queryImport;
        Tucuxi::Common::XmlDocument xmlDocument;

        fructose_assert(xmlDocument.fromString(_xmlString) == true);

        Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

        Tucuxi::Common::XmlNodeIterator xmlNodeIterator = xmlNode.getChildren("treatment");

        return queryImport.createTreatment(xmlNodeIterator);
    }

    void exportXml(unique_ptr<Tucuxi::Query::Treatment> _pTreatment, std::string &_xmlString)
    {
        Tucuxi::Query::ComputingQueryResponseXmlExport computingXmlExport;

        Tucuxi::Common::XmlNode xmlNode;

        Tucuxi::Common::XmlDocument xmlDocument;

        Tucuxi::Common::XmlNode root = xmlDocument.createNode(Tucuxi::Common::EXmlNodeType::Element, "root");

        Tucuxi::Common::XmlNode treatment = xmlDocument.createNode(Tucuxi::Common::EXmlNodeType::Element, "treatment");
        root.addChild(treatment);

        fructose_assert(computingXmlExport.exportDosageHistory(std::move(_pTreatment->getpDosageHistory()), treatment) == true);

        xmlDocument.toString(_xmlString, true);

    }
};
