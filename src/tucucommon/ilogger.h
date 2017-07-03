/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_ILOGGER_H
#define TUCUXI_TUCUCOMMON_ILOGGER_H


class ILogger : public Tucuxi::Common::Interface
{
public:
    virtual void debug(const char* msg) = 0;

    template <typename Arg1, typename... Args>
    virtual void debug(const char* fmt, const Arg1 &arg1, const Args&... args) = 0;

    virtual void info(const char* msg) = 0;

    template <typename Arg1, typename... Args>
    virtual void info(const char* fmt, const Arg1 &arg1, const Args&... args) = 0;

    virtual void warn(const char* msg) = 0;

    template <typename Arg1, typename... Args>
    virtual void warn(const char* fmt, const Arg1 &arg1, const Args&... args) = 0;

    virtual void error(const char* msg) = 0;

    template <typename Arg1, typename... Args>
    virtual void error(const char* fmt, const Arg1 &arg1, const Args&... args) = 0;

    virtual void critical(const char* msg) = 0;

    template <typename Arg1, typename... Args>
    virtual void critical(const char* fmt, const Arg1 &arg1, const Args&... args) = 0;
};

#endif // TUCUXI_TUCUCOMMON_ILOGGER_H
