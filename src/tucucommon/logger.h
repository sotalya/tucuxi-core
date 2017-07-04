/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LOGGER_H
#define TUCUXI_TUCUCOMMON_LOGGER_H

#include "spdlog/spdlog.h"

#include "tucucommon/component.h"
#include "tucucommon/ilogger.h"

namespace Tucuxi {
namespace Common {

class Logger : public Tucuxi::Common::Component, public ILogger
{
public:
    ~Logger();
    void setLevel(spdlog::level::level_enum level);

    virtual void debug(const char* msg);
    virtual void info(const char* msg);
    virtual void warn(const char* msg);
    virtual void error(const char* msg);
    virtual void critical(const char* msg);

protected:
    virtual Tucuxi::Common::Interface* getInterface(const std::string &_name);

private:
    Logger(const std::string &_filename);
    friend class LoggerHelper;

private:
    std::shared_ptr<spdlog::logger> m_logger;
};

}
}

#endif // TUCUXI_TUCUCOMMON_LOGGER_H
