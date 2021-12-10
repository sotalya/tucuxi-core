#ifndef IIMPORT_H
#define IIMPORT_H

#include <iostream>
#include <string>
#include <vector>

#include "tucucommon/loggerhelper.h"

namespace Tucuxi {
namespace Common {

class IImport
{
public:
    // clang-format off
    enum class [[nodiscard]] Status {
        Ok = 0,
        Error,
        CantOpenFile,
        CantCreateXmlDocument
    };
    // clang-format on

    virtual std::string getErrorMessage() const = 0;
};

std::ostream& operator<<(std::ostream& _stream, const IImport::Status& _e);

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_IIMPORT_H
