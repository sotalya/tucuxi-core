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


#ifndef XMLIMPORTER_H
#define XMLIMPORTER_H

#include <vector>

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/iimport.h"
#include "tucucommon/unit.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/xmlnode.h"

namespace Tucuxi {
namespace Common {

class XMLImporter : public IImport
{

public:
    std::string getErrorMessage() const override;

protected:
    enum class EmptynessAllowed
    {
        AllowEmpty = 0,
        DoNotAllowEmpty
    };

    enum class CheckUnit
    {
        Check = 0,
        DoNotCheck
    };

    virtual const std::vector<std::string>& ignoredTags() const = 0;

    template<typename T>
    void setNodeError(T _nodeIterator)
    {
        std::string errorMessage;
        Tucuxi::Common::XmlNode node = _nodeIterator->getParent();
        while (node.isValid()) {
            if (!node.getName().empty()) {
                errorMessage = std::string("<").append(node.getName()).append(">").append(errorMessage);
            }
            node = node.getParent();
        }


        if (_nodeIterator->getValue().empty()) {
            errorMessage += '<' + _nodeIterator->getName() + "> contains an empty value.";
        }
        else {
            errorMessage +=
                    "<" + _nodeIterator->getName() + "> contains an invalid value : " + _nodeIterator->getValue();
        }


        setStatus(Status::Error, errorMessage);
    }


    template<typename T>
    void setNodeError(T _nodeIterator, const std::string& _message)
    {
        std::string errorMessage;
        Tucuxi::Common::XmlNode node = _nodeIterator->getParent();
        while (node.isValid()) {
            if (!node.getName().empty()) {
                errorMessage = std::string("<").append(node.getName()).append(">").append(errorMessage);
            }
            node = node.getParent();
        }

        errorMessage += '<' + _nodeIterator->getName() + "> " + _message;

        setStatus(Status::Error, errorMessage);
    }


    void setErrorMessage(std::string _errorMessage);

    void setStatus(Status _status, const std::string& _errorMessage = "");

    Status getStatus() const;

    void unexpectedTag(const std::string& _tagName);

    std::string checkNodeIterator(Common::XmlNodeIterator _rootIterator, const std::string& _tagName);

    //Extract data from XML file
    TucuUnit extractUnit(Common::XmlNodeIterator _rootIterator, CheckUnit _checkUnit = CheckUnit::DoNotCheck);
    double extractDouble(Common::XmlNodeIterator _rootIterator);
    bool extractBool(Common::XmlNodeIterator _rootIterator);
    int extractInt(Common::XmlNodeIterator _rootIterator);
    DateTime extractDateTime(Common::XmlNodeIterator _rootIterator);
    Duration extractDuration(Common::XmlNodeIterator _rootIterator);
    std::string extractString(Common::XmlNodeIterator _rootIterator);


    TucuUnit getChildUnit(
            Common::XmlNodeIterator _rootIterator,
            const std::string& _childName,
            CheckUnit _checkunit = CheckUnit::DoNotCheck);
    double getChildDouble(Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    double getChildDoubleOptional(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName, double _defaultValue);
    bool getChildBool(Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    bool getChildBoolOptional(Common::XmlNodeIterator _rootIterator, const std::string& _childName, bool _defaultValue);
    int getChildInt(Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    DateTime getChildDateTime(
            Common::XmlNodeIterator _rootIterator,
            const std::string& _childName,
            EmptynessAllowed _allowEmpty = EmptynessAllowed::DoNotAllowEmpty);
    Duration getChildDuration(Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    std::string getChildString(Common::XmlNodeIterator _rootIterator, const std::string& _childName);


private:
    Status m_status{Status::Ok};

    std::string m_errorMessage;
};

} // namespace Common
} // namespace Tucuxi

#endif // XMLIMPORTER_H
