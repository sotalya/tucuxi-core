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

#include "tucucore/unit.h"
#include "tucucore/residualerrormodel.h"

struct TestXmlImporter;

using namespace std;

namespace Tucuxi {
namespace Common {

class XMLImporter : public IImport
{

public:

    string getErrorMessage() const override;

protected:

    virtual const std::vector<string> &ignoredTags() const = 0;

    void setNodeError(Common::XmlNodeIterator _node);

    void setStatus(Status _result, string _errorMessage = "");

    Status getStatus() const;

    void unexpectedTag(string _tagName);

    void isNodeIteratorValid(Common::XmlNodeIterator _rootIterator);

    //Extract data from XML file
    Core::Unit extractUnit(Common::XmlNodeIterator _rootIterator);
    double extractDouble(Common::XmlNodeIterator _rootIterator);
    bool extractBool(Common::XmlNodeIterator _rootIterator);
    int extractInt(Common::XmlNodeIterator _rootIterator);
    DateTime extractDateTime(Common::XmlNodeIterator _rootIterator);
    Duration extractDuration(Common::XmlNodeIterator _rootIterator);


    Core::Unit getChildUnit(Common::XmlNodeIterator _rootIterator, const string& _childName);
    double getChildDouble(Common::XmlNodeIterator _rootIterator, const string& _childName);
    bool getChildBool(Common::XmlNodeIterator _rootIterator, const string& _childName);
    int getChildInt(Common::XmlNodeIterator _rootIterator, const string& _childName);
    DateTime getChildDateTime(Common::XmlNodeIterator _rootIterator, const string& _childName);
    Duration getChildDuration(Common::XmlNodeIterator _rootIterator, const string& _childName);


private:

    Status m_status;

    std::string m_errorMessage;

};

} // namespace Common
} // namespace Tucuxi

#endif // XMLIMPORTER_H
