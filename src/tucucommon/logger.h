/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LOGGER_H
#define TUCUXI_TUCUCOMMON_LOGGER_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include "spdlog/spdlog.h"

#include "tucucommon/component.h"
#include "tucucommon/ilogger.h"

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Singleton class implement the logging component.
///
/// The logging component use spdlog library to create a console logger and a
/// file rotating logger. Every log messages written throuh the logging methods
/// are displayed on console and saved in the log file.
///
/// The functions under spdlog are thread safe.
///
/// The instance of LoggerComponent is managed by LoggerHelper class.
///
/// \sa Interface, Component
class Logger : public Tucuxi::Common::Component, public ILogger
{
public:
    /// \brief Constructor call by LoggerHelper
    ~Logger();

    /// \brief Set minimal level of logging message.
    /// \param _level Minimal level
    /// \return None
    void setLevel(spdlog::level::level_enum _level);

    virtual void debug(const char* _msg);
    virtual void info(const char* _msg);
    virtual void warn(const char* _msg);
    virtual void error(const char* _msg);
    virtual void critical(const char* _msg);

protected:
    /// \brief Access other interfaces of the same component.
    virtual Tucuxi::Common::Interface* getInterface(const std::string &_name);

private:
    /// \brief Constructor call by LoggerHelper
    Logger(const std::string &_filename);
    friend class LoggerHelper;

private:
    std::shared_ptr<spdlog::logger> m_logger; /// The pointer to the instance of spdlog logger
};

}
}

#endif // TUCUXI_TUCUCOMMON_LOGGER_H
