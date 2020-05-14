#include "xmlimporter.h"

using namespace std;

namespace Tucuxi {
namespace Common {


static const int MINUTE = 60;
static const int SECONDE = MINUTE;
static const string DATE_FORMAT = "%Y-%m-%dT%H:%M:%S"; // NOLINT(readability-identifier-naming)


void XMLImporter::unexpectedTag(std::string _tagName) {

    std::vector<std::string> ignored = ignoredTags();
    for(const auto & s : ignored) {
        if (s == _tagName) {
            return;
        }
    }
    LoggerHelper logHelper;
    logHelper.warn("Unexpected tag <{}>", _tagName);
}

void XMLImporter::setStatus(Status _result, std::string _errorMessage) {
    // Totally unuseful test, but good to add a breakpoint in the else during debugging
    if (_result == Status::Ok) {
        m_status = _result;
    }
    else {
        m_status = _result;
    }
    m_errorMessage += _errorMessage + "\n";
}

IImport::Status XMLImporter::getStatus() const
{
    return m_status;
}

std::string XMLImporter::getErrorMessage() const
{
    return m_errorMessage;
}

void XMLImporter::setNodeError(Tucuxi::Common::XmlNodeIterator _nodeIterator)
{
    std::string errorMessage;
    Tucuxi::Common::XmlNode node = _nodeIterator->getParent();
    while (node.isValid()) {
        if (!node.getName().empty()) {
            errorMessage = "<" + node.getName() + ">" + errorMessage;
        }
        node = node.getParent();
    }
    if (_nodeIterator->getValue() == "") {
        errorMessage += "<" + _nodeIterator->getName() + "> contains an empty value.";
    }
    else {
        errorMessage += "<" + _nodeIterator->getName() + "> contains an invalid value : " + _nodeIterator->getValue();
    }
    setStatus(Status::Error, errorMessage);

}


Tucuxi::Core::Unit XMLImporter::extractUnit(Tucuxi::Common::XmlNodeIterator _rootIterator)
{
    std::string unitString = _rootIterator->getValue();
    if (unitString == "min") {
        unitString = "m";
    }

    Tucuxi::Core::Unit result(unitString);
    return result;
}

double XMLImporter::extractDouble(Tucuxi::Common::XmlNodeIterator _rootIterator)
{
    double result;
    try {
        std::size_t pos;
        result = std::stod(_rootIterator->getValue(),&pos);
        if (pos != _rootIterator->getValue().size()) {
            setNodeError(_rootIterator);
            result = 0.0;
        }
    } catch (...) {
        setNodeError(_rootIterator);
        result = 0.0;
    }
    return result;
}

bool XMLImporter::extractBool(Tucuxi::Common::XmlNodeIterator _rootIterator)
{

    const string nodeValue = _rootIterator->getValue();

    if ((nodeValue == "true") || (nodeValue == "True") || (nodeValue == "1")) {
        return true;
    }
    else if ((nodeValue == "false") || (nodeValue == "False") || (nodeValue == "0")) {
        return false;
    }

    setNodeError(_rootIterator);
    return false;
}

int XMLImporter::extractInt(Common::XmlNodeIterator _rootIterator)
{
    int result;
    try {
        std::size_t pos;
        result = std::stoi(_rootIterator->getValue(),&pos);
        if (pos != _rootIterator->getValue().size()) {
            setNodeError(_rootIterator);
            result = 0;
        }
    }
    catch (...) {
        setNodeError(_rootIterator);
        result = 0;
    }
    return result;
}

DateTime XMLImporter::extractDateTime(Common::XmlNodeIterator _rootIterator)
{
    try{
        DateTime dateTime(_rootIterator->getValue(), DATE_FORMAT);
        return dateTime;
    }catch (std::runtime_error &e){
        setNodeError(_rootIterator);
        return DateTime();
    }

}

Duration XMLImporter::extractDuration(Common::XmlNodeIterator _rootIterator)
{
    string s = _rootIterator->getValue();

    std::vector<int> values;

    if( regex_match(s, regex("(([0-9]{2}:){2}([0-9]{2}))")))
    {
        std::string delimiter = ":";

        size_t pos = 0;
        std::string token;

        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            values.push_back(std::stoi(token));
            s.erase(0, pos + delimiter.length());
        }
        values.push_back(std::stoi(s));

        if(!(values[1] < MINUTE && values[2] < SECONDE))
        {
            setNodeError(_rootIterator);
            return Common::Duration();
        }
    }
    else{
        setNodeError(_rootIterator);
        return Common::Duration();
    }


    return Common::Duration(std::chrono::hours(values[0]), std::chrono::minutes(values[1]), std::chrono::seconds(values[2]));

}


Core::Unit XMLImporter::getChildUnit(Common::XmlNodeIterator _rootIterator, const string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    isNodeIteratorValid(child);

    return extractUnit(child);
}

double XMLImporter::getChildDouble(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    isNodeIteratorValid(child);

    return extractDouble(child);
}

bool XMLImporter::getChildBool(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    isNodeIteratorValid(child);

    return extractBool(child);
}

int XMLImporter::getChildInt(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    isNodeIteratorValid(child);

    return extractInt(child);
}

DateTime XMLImporter::getChildDateTime(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    isNodeIteratorValid(child);

    return extractDateTime(child);
}

Duration XMLImporter::getChildDuration(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    isNodeIteratorValid(child);

    return extractDuration(child);
}

void XMLImporter::isNodeIteratorValid(Common::XmlNodeIterator _rootIterator)
{
    if(_rootIterator == Common::XmlNodeIterator::none())
    {
        setNodeError(_rootIterator);
    }
}

} // namespace Common
} // namespace Tucuxi
