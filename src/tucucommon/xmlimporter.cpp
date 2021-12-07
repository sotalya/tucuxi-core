#include "xmlimporter.h"

using namespace std;

namespace Tucuxi {
namespace Common {


static const int MINUTE = 60;
static const int SECONDE = MINUTE;
static const string DATE_FORMAT = "%Y-%m-%dT%H:%M:%S"; // NOLINT(readability-identifier-naming)


void XMLImporter::unexpectedTag(const std::string& _tagName)
{

    std::vector<std::string> ignored = ignoredTags();
    for (const auto& s : ignored) {
        if (s == _tagName) {
            return;
        }
    }
    LoggerHelper logHelper;
    logHelper.warn("Unexpected tag <{}>", _tagName);
}

void XMLImporter::setStatus(Status _status, const string& _errorMessage)
{
#ifdef EASY_DEBUG
    // Totally unuseful test, but good to add a breakpoint in the else during debugging
    if (_status == Status::Ok) { // NOLINT(bugprone-branch-clone)
        m_status = _status;
    }
    else {
        m_status = _status;
    }
#else
    m_status = _status;
#endif // EASY_DEBUG
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


    if (_nodeIterator->getValue().empty()) {
        errorMessage += '<' + _nodeIterator->getName() + "> contains an empty value.";
    }
    else {
        errorMessage += "<" + _nodeIterator->getName() + "> contains an invalid value : " + _nodeIterator->getValue();
    }


    setStatus(Status::Error, errorMessage);
}

TucuUnit XMLImporter::extractUnit(Tucuxi::Common::XmlNodeIterator _rootIterator, CheckUnit _checkUnit)
{
    std::string unitString = _rootIterator->getValue();

    if (_checkUnit == CheckUnit::Check) {
        if (!UnitManager::isUnitTolerated(unitString)) {
            if (!UnitManager::isKnown(unitString)) {
                setNodeError(_rootIterator);
            }
        }
    }

    return TucuUnit(unitString);
}

double XMLImporter::extractDouble(Tucuxi::Common::XmlNodeIterator _rootIterator)
{
    double result;
    try {
        std::size_t pos;
        result = std::stod(_rootIterator->getValue(), &pos);
        if (pos != _rootIterator->getValue().size()) {
            setNodeError(_rootIterator);
            result = 0.0;
        }
    }
    catch (...) {
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
    if ((nodeValue == "false") || (nodeValue == "False") || (nodeValue == "0")) {
        return false;
    }

    setNodeError(_rootIterator);
    return false;
}

int XMLImporter::extractInt(Common::XmlNodeIterator _rootIterator)
{
    try {
        std::size_t pos = 0;
        int result = std::stoi(_rootIterator->getValue(), &pos);
        if (pos != _rootIterator->getValue().size()) {
            setNodeError(_rootIterator);
            result = 0;
        }
        return result;
    }
    catch (...) {
        setNodeError(_rootIterator);
        return 0;
    }
}

DateTime XMLImporter::extractDateTime(Common::XmlNodeIterator _rootIterator)
{
    try {
        DateTime dateTime(_rootIterator->getValue(), DATE_FORMAT);
        return dateTime;
    }
    catch (std::runtime_error& e) {
        setNodeError(_rootIterator);
        return DateTime::undefinedDateTime();
    }
}

Duration XMLImporter::extractDuration(Common::XmlNodeIterator _rootIterator)
{
    string s = _rootIterator->getValue();

    std::vector<int> values;

    if (regex_match(s, regex("(([0-9]{1,}:)([0-9]{1,2}:)([0-9]{1,2}))"))) {
        std::string delimiter = ":";

        size_t pos = 0;
        std::string token;

        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            values.push_back(std::stoi(token));
            s.erase(0, pos + delimiter.length());
        }
        values.push_back(std::stoi(s));

        if (!(values[1] < MINUTE && values[2] < SECONDE)) {
            setNodeError(_rootIterator);
            return Common::Duration();
        }
    }
    else {
        setNodeError(_rootIterator);
        return Common::Duration();
    }


    return Common::Duration(
            std::chrono::hours(values[0]), std::chrono::minutes(values[1]), std::chrono::seconds(values[2]));
}

string XMLImporter::extractString(Common::XmlNodeIterator _rootIterator)
{
    return _rootIterator->getValue();
}


TucuUnit XMLImporter::getChildUnit(
        Common::XmlNodeIterator _rootIterator, const string& _childName, CheckUnit _checkunit)
{
    auto child = _rootIterator->getChildren(_childName);

    //TUCUXI tolerate empty unit

    return extractUnit(child, _checkunit);
}

double XMLImporter::getChildDouble(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    if (checkNodeIterator(child, _childName).empty()) {
        setNodeError(child);
        return 0.0;
    }
    return extractDouble(child);
}

double XMLImporter::getChildDoubleOptional(
        Common::XmlNodeIterator _rootIterator, const std::string& _childName, double _defaultValue)
{
    auto child = _rootIterator->getChildren(_childName);

    if (child == Common::XmlNodeIterator::none()) {
        return _defaultValue;
    }
    return extractDouble(child);
}

bool XMLImporter::getChildBool(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    if (checkNodeIterator(child, _childName).empty()) {
        setNodeError(child);
        return false;
    }
    return extractBool(child);
}

bool XMLImporter::getChildBoolOptional(
        Common::XmlNodeIterator _rootIterator, const std::string& _childName, bool _defaultValue)
{
    auto child = _rootIterator->getChildren(_childName);

    if (child == Common::XmlNodeIterator::none()) {
        return _defaultValue;
    }
    return extractBool(child);
}

int XMLImporter::getChildInt(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    if (checkNodeIterator(child, _childName).empty()) {
        setNodeError(child);
        return 0;
    }
    return extractInt(child);
}

DateTime XMLImporter::getChildDateTime(
        Common::XmlNodeIterator _rootIterator, const std::string& _childName, EmptynessAllowed _allowEmpty)
{
    auto child = _rootIterator->getChildren(_childName);

    if (checkNodeIterator(child, _childName).empty()) {
        if (_allowEmpty == EmptynessAllowed::DoNotAllowEmpty) {
            setNodeError(child);
        }
        return DateTime::undefinedDateTime();
    }
    return extractDateTime(child);
}

Duration XMLImporter::getChildDuration(Common::XmlNodeIterator _rootIterator, const std::string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    if (checkNodeIterator(child, _childName).empty()) {
        setNodeError(child);
        return Common::Duration();
    }
    return extractDuration(child);
}

string XMLImporter::getChildString(Common::XmlNodeIterator _rootIterator, const string& _childName)
{
    auto child = _rootIterator->getChildren(_childName);

    if (checkNodeIterator(child, _childName).empty()) {
        return "";
    }
    return extractString(child);
}


string XMLImporter::checkNodeIterator(Common::XmlNodeIterator _rootIterator, const string& _tagName)
{
    if (_rootIterator == Common::XmlNodeIterator::none()) {
        setStatus(Status::Error, "<" + _tagName + "> not found in xml input");

        LoggerHelper logHelper;
        logHelper.warn("Tag <{}> not found in xml input", _tagName);
        return "";
    }
    return _rootIterator->getValue();
}

} // namespace Common
} // namespace Tucuxi
