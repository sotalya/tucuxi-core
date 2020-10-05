/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <string>
#include <iostream>
#include <stdexcept>

#include "date/date.h"

#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/utils.h"
#include "tucucommon/unit.h"


#include "fructose/fructose.h"

using namespace std::chrono_literals;
using namespace date;


class ClassTestXmlImporter : Tucuxi::Common::XMLImporter
{
public:
    ClassTestXmlImporter()
    {
        setStatus(Tucuxi::Common::IImport::Status::Ok);
    }
    const std::vector<std::string> & ignoredTags() const override
    {
        static std::vector<std::string> tags;
        return tags;
    }
private:

    friend struct TestXmlImporter;

};

struct TestXmlImporter : public fructose::test_base<TestXmlImporter>
{

    void xmlImporter(const std::string& _testName)
    {

        std::cout << _testName << std::endl;

        bool conversionSuccess = true;
        bool conversionIssue = false;



        //Good xml format
        checkIntExtraction(conversionSuccess);
        checkDoubleExtraction(conversionSuccess);
        checkBoolExtraction(conversionSuccess);
        checkUnitExtraction(conversionSuccess);
        checkDateTimeExtraction(conversionSuccess);
        checkDurationExtraction(conversionSuccess);

        //Bad xml format
        checkIntExtraction(conversionIssue);
        checkDoubleExtraction(conversionIssue);
        checkBoolExtraction(conversionIssue);
        checkUnitExtraction(conversionIssue);
        checkDateTimeExtraction(conversionIssue);
        checkDurationExtraction(conversionIssue);



    }



private:

    void checkIntExtraction(bool _conversionState)
    {
        static const std::string INT            = "int";
        static const std::string INT_1          = "int1";
        static const std::string INT_2          = "int2";
        static const std::string INT_3          = "int3";
        static const std::string INT_4          = "int4";

        std::string xmlString = "<root>"
                                  "<int>"
                                    "<int1>salut</int1>"
                                    "<int2>1.34</int2>"
                                    "<int3></int3>"
                                    "<int4>23</int4>"
                                  "</int>"
                                "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if(_conversionState)
        {
            fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

            ClassTestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(INT);

            //BAD
            fructose_assert_eq(xmlImporterTestClass.getChildInt(xmlIterator, INT_1), 0);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildInt(xmlIterator, INT_2), 0);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildInt(xmlIterator, INT_3), 0);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //GOOD
            fructose_assert_eq(xmlImporterTestClass.getChildInt(xmlIterator, INT_4), 23);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);
        }
        else{

            xmlString += "<wrong>";
            fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

        }

    }

    void checkDoubleExtraction(bool _conversionState)
    {
        static const std::string DOUBLE         = "double";
        static const std::string DOUBLE_1       = "double1";
        static const std::string DOUBLE_2       = "double2";
        static const std::string DOUBLE_3       = "double3";
        static const std::string DOUBLE_4       = "double4";

        std::string xmlString = "<root>"
                        "<double>"
                           "<double1>salut</double1>"
                           "<double2>23.23.23</double2>"
                           "<double3></double3>"
                           "<double4>12.12</double4>"
                        "</double>"
                    "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if(_conversionState)
        {

            fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

            ClassTestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(DOUBLE);

            //BAD
            fructose_assert_eq(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_1), 0.0);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_2), 0.0);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_3), 0.0);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //GOOD
            fructose_assert_eq(xmlImporterTestClass.getChildDouble(xmlIterator, DOUBLE_4), 12.12);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

        }
        else{

        xmlString += "<wrong>";
        fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

        }
    }

    void checkBoolExtraction(bool _conversionState)
    {
        static const std::string BOOL           = "bool";
        static const std::string BOOL_1         = "bool1";
        static const std::string BOOL_2         = "bool2";
        static const std::string BOOL_3         = "bool3";
        static const std::string BOOL_4         = "bool4";
        static const std::string BOOL_5         = "bool5";
        static const std::string BOOL_6         = "bool6";
        static const std::string BOOL_7         = "bool7";
        static const std::string BOOL_8         = "bool8";
        static const std::string BOOL_9         = "bool9";
        static const std::string BOOL_10        = "bool10";

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

        if(_conversionState)
        {

            fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

            ClassTestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(BOOL);

            //BAD
            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_1), false);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_2), false);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_3), false);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_4), false);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);


            //GOOD
            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_5), true);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_6), true);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_7), true);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_8), false);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_9), false);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildBool(xmlIterator, BOOL_10), false);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

        }
        else{

        xmlString += "<wrong>";
        fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

        }
    }

    void checkUnitExtraction(bool _conversionState)
    {

        static const std::string UNIT             = "unit";
        static const std::string UNIT_1           = "unit1";
        static const std::string UNIT_2           = "unit2";

        std::string xmlString = "<root>"
                                "<unit>"
                                    "<unit1>mg</unit1>"
                                    "<unit2>min</unit2>"
                                "</unit>"
                            "</root>";

        Tucuxi::Common::XmlDocument xmlDocument;

        if(_conversionState)
        {

            fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

            ClassTestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(UNIT);

            Tucuxi::Common::TucuUnit u1("mg");
            Tucuxi::Common::TucuUnit u2("min");

            //GOOD
            fructose_assert(xmlImporterTestClass.getChildUnit(xmlIterator, UNIT_1) ==  u1);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert(xmlImporterTestClass.getChildUnit(xmlIterator, UNIT_2) ==  u2);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //BAD --> TODO : NO ERROR FOR THE TIME

        }
        else{

        xmlString += "<wrong>";
        fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

        }
    }

    void checkDateTimeExtraction(bool _conversionState)
    {
        static const std::string DATE_TIME           = "dateTime";
        static const std::string DATE_TIME_1         = "dateTime1";
        static const std::string DATE_TIME_2         = "dateTime2";
        static const std::string DATE_TIME_3         = "dateTime3";
        static const std::string DATE_TIME_4         = "dateTime4";
        static const std::string DATE_TIME_5         = "dateTime5";
        static const std::string DATE_TIME_6         = "dateTime6";

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

        if(_conversionState)
        {

            fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

            ClassTestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(DATE_TIME);

            Tucuxi::Common::DateTime currentDateTime;
            Tucuxi::Common::DateTime dt1("1993-11-11 12:34:56", "%Y-%m-%d %H:%M:%S");

            //GOOD
            fructose_assert(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_1) == dt1);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //BAD
            checkDateTime(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_2), currentDateTime);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            checkDateTime(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_3), currentDateTime);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            checkDateTime(xmlImporterTestClass.getChildDateTime(xmlIterator, DATE_TIME_4), currentDateTime);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

        }
        else{

        xmlString += "<wrong>";
        fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

        }


    }

    void checkDurationExtraction(bool _conversionState)
    {

        static const std::string DURATION       = "duration";
        static const std::string DURATION_1     = "duration1";
        static const std::string DURATION_2     = "duration2";
        static const std::string DURATION_3     = "duration3";
        static const std::string DURATION_4     = "duration4";
        static const std::string DURATION_5     = "duration5";
        static const std::string DURATION_6     = "duration6";

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

        if(_conversionState)
        {

            fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

            ClassTestXmlImporter xmlImporterTestClass;

            Tucuxi::Common::XmlNode xmlNode = xmlDocument.getRoot();

            Tucuxi::Common::XmlNodeIterator xmlIterator = xmlNode.getChildren(DURATION);


            Tucuxi::Common::Duration emptyDuration;
            Tucuxi::Common::Duration d1(std::chrono::hours(12), std::chrono::minutes(34), std::chrono::seconds(56));

            //GOOD
            fructose_assert_eq(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_1), d1);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Ok);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            //BAD
            fructose_assert_eq(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_2), emptyDuration);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_3), emptyDuration);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_4), emptyDuration);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_5), emptyDuration);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

            fructose_assert_eq(xmlImporterTestClass.getChildDuration(xmlIterator, DURATION_6), emptyDuration);
            fructose_assert(xmlImporterTestClass.getStatus() == Tucuxi::Common::IImport::Status::Error);
            xmlImporterTestClass.setStatus(Tucuxi::Common::IImport::Status::Ok);

        }
        else{

        xmlString += "<wrong>";
        fructose_assert(xmlDocument.fromString(xmlString) == _conversionState);

        }

    }

    void checkDateTime(const Tucuxi::Common::DateTime& _date, const Tucuxi::Common::DateTime& _currentDate)
    {
        fructose_assert_eq(_date.year() , _currentDate.year());
        fructose_assert_eq(_date.month() , _currentDate.month());
        fructose_assert_eq(_date.day() , _currentDate.day());
        fructose_assert_eq(_date.hour() , _currentDate.hour());
        fructose_assert_eq(_date.minute() , _currentDate.minute());
        fructose_assert_eq(_date.second() , _currentDate.second());
    }

};
