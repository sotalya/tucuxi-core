#ifndef IIMPORT_H
#define IIMPORT_H

#include <string>
#include <iostream>
#include <vector>

#include "tucucommon/loggerhelper.h"

namespace Tucuxi {
namespace Common {

class IImport
{
public:
    enum class Result {
        Ok = 0,
        Error,
        CantOpenFile
    };

    std::string getErrorMessage() const { return m_errorMessage;}

protected:

    virtual ~IImport() = default;

    void setResult(Result _result, std::string _errorMessage = "") {
        // Totally unuseful test, but good to add a breakpoint in the else during debugging
        if (_result == Result::Ok) {
            m_result = _result;
        }
        else {
            m_result = _result;
        }
        m_errorMessage = _errorMessage;
    }

    void unexpectedTag(std::string _tagName) {
        std::vector<std::string> ignored = ignoredTags();
        for(const auto & s : ignored) {
            if (s == _tagName) {
                return;
            }
        }
        LoggerHelper logHelper;
        logHelper.warn("Unexpected tag <{}>", _tagName);
    }

    Result getResult() const { return m_result;}

    virtual const std::vector<std::string> &ignoredTags() const = 0;

private:

    Result m_result;

    std::string m_errorMessage;
};


} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_IIMPORT_H
