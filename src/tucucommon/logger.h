/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LOGGER_H
#define TUCUXI_TUCUCOMMON_LOGGER_H

#include "tucucommon/component.h"
#include "tucucommon/ilogger.h"

class Logger : public Tucuxi::Common::Component, ILogger
{

public:
    virtual void debug(const char* msg);

    template <typename Arg1, typename... Args>
    virtual void debug(const char* fmt, const Arg1 &arg1, const Args&... args);

    virtual void info(const char* msg);

    template <typename Arg1, typename... Args>
    virtual void info(const char* fmt, const Arg1 &arg1, const Args&... args);

    virtual void warn(const char* msg);

    template <typename Arg1, typename... Args>
    virtual void warn(const char* fmt, const Arg1 &arg1, const Args&... args);

    virtual void error(const char* msg);

    template <typename Arg1, typename... Args>
    virtual void error(const char* fmt, const Arg1 &arg1, const Args&... args);

    virtual void critical(const char* msg);

    template <typename Arg1, typename... Args>
    virtual void critical(const char* fmt, const Arg1 &arg1, const Args&... args);

protected:
    virtual Tucuxi::Common::Interface* getInterface(const std::string &_name);

private:
    Logger();
};

#endif // TUCUXI_TUCUCOMMON_LOGGER_H
