/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <gtest/gtest.h>

#include "tucucommon/utils.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/xmlnode.h"

extern std::string appFolder;

static int countNodes(Tucuxi::Common::XmlNodeIterator& iter)
{
    int n = 0;
    while (iter != iter.none()) {
        n++;
        iter++;
    }
    return n;
}

TEST(Common_TestXml, Read)
{
    Tucuxi::Common::XmlDocument xmlDoc;
    xmlDoc.fromString(
            "<ROOT><CHILD text='abc' id='1'>Salut</CHILD><CHILD text='def' id='2'>Tcho</CHILD><AAA>bbb</AAA></ROOT>");

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
    ASSERT_EQ(test, "CHILDSaluttextabcid1CHILDTchotextdefid2AAAbbb");

    iterNodes = root.getChildren("CHILD");
    ASSERT_EQ(countNodes(iterNodes), 2);

    iterNodes = root.getChildren("AAA");
    ASSERT_EQ(countNodes(iterNodes), 1);

    iterNodes = root.getChildren("ZZZ");
    ASSERT_EQ(countNodes(iterNodes), 0);
}

TEST(Common_TestXml, Write)
{
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

    ASSERT_EQ(strXml, "<ROOT><CHILD text='abc' id='1'>Salut</CHILD><CHILD text='def' id='2'>Tcho</CHILD></ROOT>");
}

TEST(Common_TestXml, Files)
{
    int nChildren = 5000;
    std::string fileName = Tucuxi::Common::Utils::strFormat("%s/test.xml", appFolder.c_str());

    { // Create a big document and save it into a text file
        Tucuxi::Common::XmlDocument doc;
        Tucuxi::Common::XmlNode root = doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "ROOT");
        ASSERT_TRUE(root.isValid());
        doc.setRoot(root);

        // Add a ton of children
        for (int i = 0; i < nChildren; i++) {
            Tucuxi::Common::XmlNode node = doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "CHILD", "Salut");
            node.addAttribute(doc.createAttribute("text", "abc"));
            node.addAttribute(doc.createAttribute("id", Tucuxi::Common::Utils::strFormat("%i", i)));
            root.addChild(node);
        }

        ASSERT_TRUE(doc.save(fileName));
    }

    { // Create a new document by reading the content of the previously generated file.
        Tucuxi::Common::XmlDocument doc;
        ASSERT_TRUE(doc.open(fileName));

        Tucuxi::Common::XmlNode root = doc.getRoot();
        ASSERT_TRUE(root.isValid());

        int n = 0;
        Tucuxi::Common::XmlNodeIterator iterNodes = root.getChildren();
        while (iterNodes != iterNodes.none()) {
            ASSERT_EQ(iterNodes->getAttribute("text").getValue(), "abc");
            ASSERT_EQ(iterNodes->getAttribute("id").getValue(), Tucuxi::Common::Utils::strFormat("%i", n));
            n++;
            iterNodes++;
        }
        ASSERT_EQ(n, nChildren);
    }
}

TEST(Common_TestXml, Operators)
{
    Tucuxi::Common::XmlDocument xmlDoc;
    xmlDoc.fromString("<ROOT><CHILD text='abc' id='1'>Salut</CHILD><CHILD text='def' id='2'>Tcho</CHILD></ROOT>");

    Tucuxi::Common::XmlNodeIterator iterNodes = xmlDoc.getRoot().getChildren("CHILD");
}
