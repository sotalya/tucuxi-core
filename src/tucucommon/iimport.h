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

    virtual std::string getErrorMessage() const = 0 ;

protected:

    virtual ~IImport() = default;




};


} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_IIMPORT_H
