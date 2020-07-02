#ifndef XMLIMPORTER_H
#define XMLIMPORTER_H


#include "iimport.h"

#include "regex"

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/unit.h"

using namespace std;

namespace Tucuxi {
namespace Common {

class XMLImporter : public IImport
{

public:

    string getErrorMessage() const override;

protected:

    enum class EmptynessAllowed {
        AllowEmpty = 0,
        DoNotAllowEmpty
    };

    enum class CheckUnit {
        Check = 0,
        DoNotCheck
    };

    virtual const std::vector<string> &ignoredTags() const = 0;

    void setNodeError(Common::XmlNodeIterator _node);

    void setErrorMessage(string _errorMessage);

    void setStatus(Status _status, string _errorMessage = "");

    Status getStatus() const;

    void unexpectedTag(string _tagName);

    string checkNodeIterator(Common::XmlNodeIterator _rootIterator, string _tagName);

    //Extract data from XML file
    Unit extractUnit(Common::XmlNodeIterator _rootIterator, CheckUnit _checkUnit = CheckUnit::DoNotCheck);
    double extractDouble(Common::XmlNodeIterator _rootIterator);
    bool extractBool(Common::XmlNodeIterator _rootIterator);
    int extractInt(Common::XmlNodeIterator _rootIterator);
    DateTime extractDateTime(Common::XmlNodeIterator _rootIterator);
    Duration extractDuration(Common::XmlNodeIterator _rootIterator);
    string extractString(Common::XmlNodeIterator _rootIterator);


    Unit getChildUnit(Common::XmlNodeIterator _rootIterator, const string& _childName, CheckUnit _checkunit = CheckUnit::DoNotCheck);
    double getChildDouble(Common::XmlNodeIterator _rootIterator, const string& _childName);
    bool getChildBool(Common::XmlNodeIterator _rootIterator, const string& _childName);
    int getChildInt(Common::XmlNodeIterator _rootIterator, const string& _childName);
    DateTime getChildDateTime(Common::XmlNodeIterator _rootIterator, const string& _childName,
                              EmptynessAllowed _allowEmpty = EmptynessAllowed::DoNotAllowEmpty);
    Duration getChildDuration(Common::XmlNodeIterator _rootIterator, const string& _childName);
    string getChildString(Common::XmlNodeIterator _rootIterator, const string& _childName);


private:

    Status m_status;

    std::string m_errorMessage;

};

} // namespace Common
} // namespace Tucuxi

#endif // XMLIMPORTER_H
