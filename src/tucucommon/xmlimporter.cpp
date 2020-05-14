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

void XMLImporter::setResult(Result _result, std::string _errorMessage) {
    // Totally unuseful test, but good to add a breakpoint in the else during debugging
    if (_result == Result::Ok) {
        m_result = _result;
    }
    else {
        m_result = _result;
    }
    m_errorMessage += _errorMessage + "\n";
}

IImport::Result XMLImporter::getResult() const
{
    return m_result;
}

std::string XMLImporter::getErrorMessage() const
{
    return m_errorMessage;
}

void XMLImporter::setNodeError(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string errorMessage;
    Tucuxi::Common::XmlNode node = _node->getParent();
    while (node.isValid()) {
        if (!node.getName().empty()) {
            errorMessage = "<" + node.getName() + ">" + errorMessage;
        }
        node = node.getParent();
    }
    if (_node->getValue() == "") {
        errorMessage += "<" + _node->getName() + "> contains an empty value.";
    }
    else {
        errorMessage += "<" + _node->getName() + "> contains an invalid value : " + _node->getValue();
    }
    setResult(Result::Error, errorMessage);

}


Tucuxi::Core::Unit XMLImporter::extractUnit(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string unitString = _node->getValue();
    if (unitString == "min") {
        unitString = "m";
    }

    Tucuxi::Core::Unit result(unitString);
    return result;
}

double XMLImporter::extractDouble(Tucuxi::Common::XmlNodeIterator _node)
{
    double result;
    try {
        std::size_t pos;
        result = std::stod(_node->getValue(),&pos);
        if (pos != _node->getValue().size()) {
            setNodeError(_node);
            result = 0.0;
        }
    } catch (...) {
        setNodeError(_node);
        result = 0.0;
    }
    return result;
}

bool XMLImporter::extractBool(Tucuxi::Common::XmlNodeIterator _node)
{

    const string nodeValue = _node->getValue();

    if ((nodeValue == "true") || (nodeValue == "True") || (nodeValue == "1")) {
        return true;
    }
    else if ((nodeValue == "false") || (nodeValue == "False") || (nodeValue == "0")) {
        return false;
    }

    setNodeError(_node);
    return false;
}

int XMLImporter::extractInt(Common::XmlNodeIterator _node)
{
    int result;
    try {
        std::size_t pos;
        result = std::stoi(_node->getValue(),&pos);
        if (pos != _node->getValue().size()) {
            setNodeError(_node);
            result = 0;
        }
    }
    catch (...) {
        setNodeError(_node);
        result = 0;
    }
    return result;
}

DateTime XMLImporter::extractDateTime(Common::XmlNodeIterator _node)
{
    try{
        DateTime dateTime(_node->getValue(), DATE_FORMAT);
        return dateTime;
    }catch (std::runtime_error &e){
        setNodeError(_node);
        return DateTime();
    }

}

Duration XMLImporter::extractDuration(Common::XmlNodeIterator _node)
{
    string s = _node->getValue();

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
            setNodeError(_node);
            return Common::Duration();
        }
    }
    else{
        setNodeError(_node);
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
