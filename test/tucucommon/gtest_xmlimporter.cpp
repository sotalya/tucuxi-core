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


#include <string>

#include <gtest/gtest.h>

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/unit.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/xmlnode.h"

class TestXmlImporter : Tucuxi::Common::XMLImporter
{
public:
    TestXmlImporter()
    {
        setStatus(Tucuxi::Common::IImport::Status::Ok);
    }
    const std::vector<std::string>& ignoredTags() const override
    {
        static std::vector<std::string> tags;
        return tags;
    }

    void checkIntExtraction(bool _conversionState)
    {
        static const std::string INT = "int";
        static const std::string INT_1 = "int1";
        static const std::string INT_2 = "int2";
        static const std::string INT_3 = "int3";
        static const std::string INT_4 = "int4";

        std::string xmlString = "<root>"
                                "<int>"
                                "<int1>salut</int1>"
                                "<int2>1.34</int2>"
                                "<int3></int3>"
                                "<int4>23</int4>"
                                "</int>"
                                "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if (_conversionState) {
            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);

            TestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(INT);

            //BAD
            ASSERT_EQ(xmlImporterTestClass.getChildInt(xmlIterator, INT_1), 0);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_EQ(xmlImporterTestClass.getChildInt(xmlIterator, INT_2), 0);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_EQ(xmlImporterTestClass.getChildInt(xmlIterator, INT_3), 0);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //GOOD
            ASSERT_EQ(xmlImporterTestClass.getChildInt(xmlIterator, INT_4), 23);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);
        }
        else {

            xmlString += "<wrong>";
            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);
        }
    }

    void checkDoubleExtraction(bool _conversionState)
    {
        static const std::string DOUBLE = "double";
        static const std::string DOUBLE_1 = "double1";
        static const std::string DOUBLE_2 = "double2";
        static const std::string DOUBLE_3 = "double3";
        static const std::string DOUBLE_4 = "double4";

        std::string xmlString = "<root>"
                                "<double>"
                                "<double1>salut</double1>"
                                "<double2>23.23.23</double2>"
                                "<double3></double3>"
                                "<double4>12.12</double4>"
                                "</double>"
                                "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if (_conversionState) {

            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);

            TestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(DOUBLE);

            //BAD
            ASSERT_DOUBLE_EQ(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_1), 0.0);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_DOUBLE_EQ(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_2), 0.0);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_DOUBLE_EQ(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_3), 0.0);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //GOOD
            ASSERT_DOUBLE_EQ(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_4), 12.12);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);
        }
        else {

            xmlString += "<wrong>";
            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);
        }
    }

    void checkBoolExtraction(bool _conversionState)
    {
        static const std::string BOOL = "bool";
        static const std::string BOOL_1 = "bool1";
        static const std::string BOOL_2 = "bool2";
        static const std::string BOOL_3 = "bool3";
        static const std::string BOOL_4 = "bool4";
        static const std::string BOOL_5 = "bool5";
        static const std::string BOOL_6 = "bool6";
        static const std::string BOOL_7 = "bool7";
        static const std::string BOOL_8 = "bool8";
        static const std::string BOOL_9 = "bool9";
        static const std::string BOOL_10 = "bool10";

        std::string xmlString = "<root>"
                                "<bool>"
                                "<bool1></bool1>"
                                "<bool2>FALSE</bool2>"
                                "<bool3>FASLE</bool3>"
                                "<bool4>tRUE</bool14>"
                                "<bool5>true</bool5>"
                                "<bool6>True</bool6>"
                                "<bool7>1</bool7>"
                                "<bool8>false</bool8>"
                                "<bool9>False</bool9>"
                                "<bool10>0</bool10>"
                                "</bool>"
                                "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if (_conversionState) {

            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);

            TestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(BOOL);

            //BAD
            ASSERT_FALSE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_1));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_FALSE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_2));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_FALSE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_3));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_FALSE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_4));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);


            //GOOD
            ASSERT_TRUE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_5));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_TRUE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_6));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_TRUE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_7));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_FALSE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_8));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_FALSE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_9));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_FALSE(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_10));
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);
        }
        else {

            xmlString += "<wrong>";
            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);
        }
    }

    void checkUnitExtraction(bool _conversionState)
    {

        static const std::string UNIT = "unit";
        static const std::string UNIT_1 = "unit1";
        static const std::string UNIT_2 = "unit2";

        std::string xmlString = "<root>"
                                "<unit>"
                                "<unit1>mg</unit1>"
                                "<unit2>min</unit2>"
                                "</unit>"
                                "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if (_conversionState) {

            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);

            TestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(UNIT);

            Tucuxi::Common::TucuUnit u1("mg");
            Tucuxi::Common::TucuUnit u2("min");

            //GOOD
            ASSERT_EQ(xmlImporterTestClass.getChildUnit(xmlIterator, UNIT_1), u1);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_EQ(xmlImporterTestClass.getChildUnit(xmlIterator, UNIT_2), u2);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //BAD --> TODO : NO ERROR FOR THE TIME
        }
        else {

            xmlString += "<wrong>";
            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);
        }
    }

    void checkDateTimeExtraction(bool _conversionState)
    {
        static const std::string DATE_TIME = "dateTime";
        static const std::string DATE_TIME_1 = "dateTime1";
        static const std::string DATE_TIME_2 = "dateTime2";
        static const std::string DATE_TIME_3 = "dateTime3";
        static const std::string DATE_TIME_4 = "dateTime4";
        static const std::string DATE_TIME_5 = "dateTime5";
        static const std::string DATE_TIME_6 = "dateTime6";

        std::string xmlString = "<root>"
                                "<dateTime>"
                                "<dateTime1>1993-11-11T12:34:56</dateTime1>"
                                "<dateTime2>199.3-11-11T12:23:34</dateTime2>"
                                "<dateTime3>salut</dateTime3>"
                                "<dateTime4>1992 3 11 11:22:22</dateTime4>"
                                "<dateTime5>TODO</dateTime5>"
                                "<dateTime6>TODO</dateTime6>"
                                "</dateTime>"
                                "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if (_conversionState) {

            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);

            TestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(DATE_TIME);

            Tucuxi::Common::DateTime currentDateTime;
            Tucuxi::Common::DateTime dt1("1993-11-11 12:34:56", "%Y-%m-%d %H:%M:%S");

            //GOOD
            ASSERT_EQ(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_1), dt1);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //BAD
            Tucuxi::Common::DateTime::disableChecks();
            checkDateTime(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_2), currentDateTime);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            checkDateTime(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_3), currentDateTime);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            checkDateTime(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_4), currentDateTime);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);
            Tucuxi::Common::DateTime::enableChecks();
        }
        else {

            xmlString += "<wrong>";
            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);
        }
    }

    void checkDurationExtraction(bool _conversionState)
    {

        static const std::string DURATION = "duration";
        static const std::string DURATION_1 = "duration1";
        static const std::string DURATION_2 = "duration2";
        static const std::string DURATION_3 = "duration3";
        static const std::string DURATION_4 = "duration4";
        static const std::string DURATION_5 = "duration5";
        static const std::string DURATION_6 = "duration6";

        std::string xmlString = "<root>"
                                "<duration>"
                                "<duration1>12:34:56</duration1>"
                                "<duration2>12:61:45</duration2>"
                                "<duration3>12:29:69</duration3>"
                                "<duration4>12:62:63</duration4>"
                                "<duration5>12:62:63:34</duration5>"
                                "<duration6>2 jours</duration6>"
                                "</duration>"
                                "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if (_conversionState) {

            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);

            TestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(DURATION);


            Tucuxi::Common::Duration emptyDuration;
            Tucuxi::Common::Duration d1(std::chrono::hours(12), std::chrono::minutes(34), std::chrono::seconds(56));

            //GOOD
            ASSERT_EQ(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_1), d1);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //BAD
            ASSERT_EQ(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_2), emptyDuration);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_EQ(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_3), emptyDuration);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_EQ(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_4), emptyDuration);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_EQ(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_5), emptyDuration);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            ASSERT_EQ(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_6), emptyDuration);
            ASSERT_EQ(xmlImporterTestClass.getStatus(), Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);
        }
        else {

            xmlString += "<wrong>";
            ASSERT_EQ(xmlDocument.fromString(xmlString), _conversionState);
        }
    }

    void checkDateTime(const Tucuxi::Common::DateTime& _date, const Tucuxi::Common::DateTime& _currentDate)
    {
        ASSERT_EQ(_date.year(), _currentDate.year());
        ASSERT_EQ(_date.month(), _currentDate.month());
        ASSERT_EQ(_date.day(), _currentDate.day());
        ASSERT_EQ(_date.hour(), _currentDate.hour());
        ASSERT_EQ(_date.minute(), _currentDate.minute());
        ASSERT_EQ(_date.second(), _currentDate.second());
    }
};

TEST(Common_TestXmlImporter, XmlImporter)
{

    bool conversionSuccess = true;
    bool conversionIssue = false;

    TestXmlImporter xmlImporter;


    //Good xml format
    xmlImporter.checkIntExtraction(conversionSuccess);
    xmlImporter.checkDoubleExtraction(conversionSuccess);
    xmlImporter.checkBoolExtraction(conversionSuccess);
    xmlImporter.checkUnitExtraction(conversionSuccess);
    xmlImporter.checkDateTimeExtraction(conversionSuccess);
    xmlImporter.checkDurationExtraction(conversionSuccess);

    //Bad xml format
    xmlImporter.checkIntExtraction(conversionIssue);
    xmlImporter.checkDoubleExtraction(conversionIssue);
    xmlImporter.checkBoolExtraction(conversionIssue);
    xmlImporter.checkUnitExtraction(conversionIssue);
    xmlImporter.checkDateTimeExtraction(conversionIssue);
    xmlImporter.checkDurationExtraction(conversionIssue);
}
