/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include "tucucommon/componentlogger.h"

virtual void Logger::debug(const char* msg)
{

}

template <typename Arg1, typename... Args>
virtual void Logger::debug(const char* fmt, const Arg1 &arg1, const Args&... args)
{

}

virtual void Logger::info(const char* msg)
{

}

template <typename Arg1, typename... Args>
virtual void Logger::info(const char* fmt, const Arg1 &arg1, const Args&... args)
{

}

virtual void Logger::warn(const char* msg)
{

}

template <typename Arg1, typename... Args>
virtual void Logger::warn(const char* fmt, const Arg1 &arg1, const Args&... args)
{

}

virtual void Logger::error(const char* msg)
{

}

template <typename Arg1, typename... Args>
virtual void Logger::error(const char* fmt, const Arg1 &arg1, const Args&... args)
{

}

virtual void Logger::critical(const char* msg)
{

}

template <typename Arg1, typename... Args>
virtual void Logger::critical(const char* fmt, const Arg1 &arg1, const Args&... args)
{

}

virtual Tucuxi::Common::Interface* Logger::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

Logger::Logger()
{
    registerInterface(dynamic_cast<ILogger*>(this));
}
