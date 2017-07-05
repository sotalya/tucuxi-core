/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_ILOGGER_H
#define TUCUXI_TUCUCOMMON_ILOGGER_H

#include "tucucommon/interface.h"

namespace Tucuxi {
namespace Common {

class ILogger : public Tucuxi::Common::Interface
{
public:
    virtual void debug(const char* _msg) = 0;
    virtual void info(const char* _msg) = 0;
    virtual void warn(const char* _msg) = 0;
    virtual void error(const char* _msg) = 0;
    virtual void critical(const char* _msg) = 0;
};

}
}

#endif // TUCUXI_TUCUCOMMON_ILOGGER_H
