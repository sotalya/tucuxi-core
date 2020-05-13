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

using namespace std;

namespace Tucuxi {
namespace Common {

class XMLImporter : public IImport
{

public:

    string getErrorMessage() const override;

protected:

    void setResult(Result _result, string _errorMessage = "");

    void unexpectedTag(string _tagName);

    Result getResult() const;

    virtual const std::vector<string> &ignoredTags() const = 0;

    void setNodeError(Common::XmlNodeIterator _node);


    Core::Unit extractUnit(Common::XmlNodeIterator _node) ;
    double extractDouble(Common::XmlNodeIterator _node) ;
    bool extractBool(Common::XmlNodeIterator _node) ;
    int extractInt(Common::XmlNodeIterator _node) ;
    DateTime extractDateTime(Common::XmlNodeIterator _node) ;
    Duration extractDuration(Common::XmlNodeIterator _node) ;


    Core::Unit getChildUnit(Common::XmlNodeIterator _rootIterator, const string& _childName);
    double getChildDouble(Common::XmlNodeIterator _rootIterator, const string& _childName);
    bool getChildBool(Common::XmlNodeIterator _rootIterator, const string& _childName);
    int getChildInt(Common::XmlNodeIterator _rootIterator, const string& _childName);
    DateTime getChildDateTime(Common::XmlNodeIterator _rootIterator, const string& _childName);
    Duration getChildDuration(Common::XmlNodeIterator _rootIterator, const string& _childName);

    void isNodeIteratorValid(Common::XmlNodeIterator _rootIterator);



private:

    Result m_result;

    std::string m_errorMessage;

};

} // namespace Common
} // namespace Tucuxi

#endif // XMLIMPORTER_H
