/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <string>
#include <iostream>
#include <stdexcept>
#include <array>

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


        testString(
        "<root>"
         "<treatment>"
          "<dosageHistory>"
           "<dosageTimeRange>"
            "<start>2018-10-02T10:00:00</start>"
             "<end>2018-10-13T10:00:00</end>"
             "<dosage>"
              "<dosageLoop>"
               "<lastingDosage>"
                "<interval>24:00:00</interval>"
                "<dose>"
                 "<value>400.000000</value>"
                 "<unit>mg</unit>"
                 "<infusionTimeInMinutes>30.000000</infusionTimeInMinutes>"
                "</dose>"
                "<formulationAndRoute>"
                 "<formulation>parenteralSolution</formulation>"
                 "<administrationName>foo bar</administrationName>"
                 "<administrationRoute>intravenousDrip</administrationRoute>"
                 "<absorptionModel>infusion</absorptionModel>"
                "</formulationAndRoute>"
               "</lastingDosage>"
              "</dosageLoop>"
             "</dosage>"
            "</dosageTimeRange>"
           "</dosageHistory>"
          "</treatment>"
         "</root>");

        //TEST2

        testString(
                "<root>"
                "<treatment>"
                "<dosageHistory>"
                    "<dosageTimeRange>"
                        "<start>2018-07-06T13:45:30</start>"
                        "<end>2018-07-08T23:45:30</end>"
                        "<dosage>"
                            "<dosageLoop>"
                                "<lastingDosage>"
                                    "<interval>12:00:00</interval>"
                                    "<dose>"
                                        "<value>400.000000</value>"
                                        "<unit>mg</unit>"
                                        "<infusionTimeInMinutes>60.000000</infusionTimeInMinutes>"
                                    "</dose>"
                                    "<formulationAndRoute>"
                                        "<formulation>parenteralSolution</formulation>"
                                        "<administrationName>foo bar</administrationName>"
                                        "<administrationRoute>oral</administrationRoute>"
                                        "<absorptionModel>extravascular</absorptionModel>"
                                    "</formulationAndRoute>"
                                "</lastingDosage>"
                            "</dosageLoop>"
                        "</dosage>"
                    "</dosageTimeRange>"
                        "<dosageTimeRange>"
                            "<start>2018-07-08T23:45:30</start>"
                            "<end>2018-07-10T23:45:30</end>"
                            "<dosage>"
                                "<dosageLoop>"
                                    "<lastingDosage>"
                                        "<interval>12:00:00</interval>"
                                        "<dose>"
                                           "<value>200.000000</value>"
                                            "<unit>mg</unit>"
                                            "<infusionTimeInMinutes>60.000000</infusionTimeInMinutes>"
                                        "</dose>"
                                        "<formulationAndRoute>"
                                            "<formulation>parenteralSolution</formulation>"
                                            "<administrationName>foo bar</administrationName>"
                                            "<administrationRoute>oral</administrationRoute>"
                                            "<absorptionModel>extravascular</absorptionModel>"
                                        "</formulationAndRoute>"
                                    "</lastingDosage>"
                                "</dosageLoop>"
                            "</dosage>"
                        "</dosageTimeRange>"
                "</dosageHistory>"
            "</treatment>"
            "</root>"
                    );

        // Test steady state
        testString(
        "<root>"
         "<treatment>"
          "<dosageHistory>"
           "<dosageTimeRange>"
            "<start/>"
             "<end>2018-10-13T10:00:00</end>"
             "<dosage>"
              "<dosageSteadyState>"
               "<lastDoseDate>2018-10-13T10:00:00</lastDoseDate>"
               "<lastingDosage>"
                "<interval>24:00:00</interval>"
                "<dose>"
                 "<value>400.000000</value>"
                 "<unit>mg</unit>"
                 "<infusionTimeInMinutes>30.000000</infusionTimeInMinutes>"
                "</dose>"
                "<formulationAndRoute>"
                 "<formulation>parenteralSolution</formulation>"
                 "<administrationName>foo bar</administrationName>"
                 "<administrationRoute>intravenousDrip</administrationRoute>"
                 "<absorptionModel>infusion</absorptionModel>"
                "</formulationAndRoute>"
               "</lastingDosage>"
              "</dosageSteadyState>"
             "</dosage>"
            "</dosageTimeRange>"
           "</dosageHistory>"
          "</treatment>"
         "</root>");
    }

private:


    void testString(std::string _xmlInput)
    {
        std::unique_ptr<Tucuxi::Query::Treatment> pTreatment1 = importXml(_xmlInput);

        std::string exportedXmlString1;

        exportXml(std::move(pTreatment1), exportedXmlString1);

        removeSpecialsCharacters(exportedXmlString1);

        int comparison1 = _xmlInput.compare(exportedXmlString1);

        fructose_assert(comparison1 == 0);

        if(comparison1 != 0)
        {
           printImportExportString(_xmlInput, exportedXmlString1);
        }
    }

    void printImportExportString(std::string &_importString, std::string &_exportString)
    {
        std::cout << std::endl;
        std::cout << "Imported String : " << std::endl << std::endl;
        std::cout << _importString << std::endl;
        std::cout << std::endl;
        std::cout << "Exported String : " << std::endl << std::endl;
        std::cout << _exportString << std::endl << std::endl;
    }

    std::unique_ptr<Tucuxi::Query::Treatment> importXml(std::string _xmlString)
    {
        Tucuxi::Query::QueryImport queryImport;
        Tucuxi::Common::XmlDocument xmlDocument;

        fructose_assert(xmlDocument.fromString(_xmlString) == true);

        Tucuxi::Common::XmlNodeIterator xmlNodeIterator = xmlDocument.getRoot().getChildren("treatment");

        return queryImport.createTreatment(xmlNodeIterator);
    }

    void exportXml(std::unique_ptr<Tucuxi::Query::Treatment> _pTreatment, std::string &_xmlString)
    {
        Tucuxi::Query::ComputingQueryResponseXmlExport computingXmlExport;

        Tucuxi::Common::XmlDocument xmlDocument;

        Tucuxi::Common::XmlNode root = xmlDocument.createNode(Tucuxi::Common::EXmlNodeType::Element, "root");
        xmlDocument.setRoot(root);

        Tucuxi::Common::XmlNode treatment = xmlDocument.createNode(Tucuxi::Common::EXmlNodeType::Element, "treatment");
        root.addChild(treatment);

        fructose_assert(computingXmlExport.exportDosageHistory(std::move(_pTreatment->getpDosageHistory()), treatment) == true);

        xmlDocument.toString(_xmlString, true);

    }

    void removeSpecialsCharacters(std::string &_exportedXmlString)
    {
        std::vector<char> specialCharacters{'\t', '\n', '\r'};

        for(char characters : specialCharacters)
        {
             _exportedXmlString.erase(std::remove(_exportedXmlString.begin(), _exportedXmlString.end(), characters), _exportedXmlString.end());
        }

    }
};
