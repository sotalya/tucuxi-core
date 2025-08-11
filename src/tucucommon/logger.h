/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_TUCUCOMMON_LOGGER_H
#define TUCUXI_TUCUCOMMON_LOGGER_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include "tucucommon/component.h"
#include "tucucommon/ilogger.h"

#include "spdlog/spdlog.h"

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
    ~Logger() override;

    /// \brief Set minimal level of logging message.
    /// \param _level Minimal level
    /// \return None
    void setLevel(spdlog::level::level_enum _level);

    void debug(const char* _msg) override;
    void info(const char* _msg) override;
    void warn(const char* _msg) override;
    void error(const char* _msg) override;
    void critical(const char* _msg) override;

protected:
    /// \brief Access other interfaces of the same component.
    Tucuxi::Common::Interface* getInterface(const std::string& _name) override;

private:
    /// \brief Constructor call by LoggerHelper
    Logger(const std::string& _filename);
    friend class LoggerHelper;

    std::shared_ptr<spdlog::logger> m_logger; /// The pointer to the instance of spdlog logger
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_LOGGER_H
