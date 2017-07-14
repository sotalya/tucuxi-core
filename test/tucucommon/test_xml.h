/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <regex>

#include "fructose/fructose.h"

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"

struct TestXml : public fructose::test_base<TestXml>
{
    void read(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::XmlDocument xmlDoc;
        xmlDoc.fromString("<ROOT><CHILD text='abc' id='1'>Salut</CHILD><CHILD text='def' id='2'>Tcho</CHILD></ROOT>");

        std::string test = "";
        Tucuxi::Common::XmlNode root = xmlDoc.getRoot();
        Tucuxi::Common::XmlNodeIterator iterNodes = root.getChildren();
        while (iterNodes != iterNodes.none()) {
            test += iterNodes->getName();
            test += iterNodes->getValue();
            Tucuxi::Common::XmlAttributeIterator iterAttributes = iterNodes->getAttributes();
            while (iterAttributes != iterAttributes.none()) {
                test += iterAttributes->getName();
                test += iterAttributes->getValue();
                iterAttributes++;
            }
            iterNodes++;
        }
        fructose_assert(test == "CHILDSaluttextabcid1CHILDTchotextdefid2");
    }

    void write(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::XmlDocument doc;
        Tucuxi::Common::XmlNode root = doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "ROOT");
        doc.setRoot(root);        
        Tucuxi::Common::XmlNode node1 = doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "CHILD", "Salut");
        node1.addAttribute(doc.createAttribute("text", "abc"));
        node1.addAttribute(doc.createAttribute("id", "1"));
        root.addChild(node1);
        Tucuxi::Common::XmlNode node2 = doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "CHILD", "Tcho");
        node2.addAttribute(doc.createAttribute("text", "def"));
        node2.addAttribute(doc.createAttribute("id", "2"));
        root.addChild(node2);
        
        std::string strXml;
        doc.toString(strXml);        
        strXml = std::regex_replace(strXml, std::regex("\n"), "");
        strXml = std::regex_replace(strXml, std::regex("\t"), "");
        strXml = std::regex_replace(strXml, std::regex("\""), "'");        

        fructose_assert(strXml == "<ROOT><CHILD text='abc' id='1'>Salut</CHILD><CHILD text='def' id='2'>Tcho</CHILD></ROOT>");
    }
};
