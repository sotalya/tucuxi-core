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


#ifndef TUCUXI_TUCUCOMMON_ILOGGER_H
#define TUCUXI_TUCUCOMMON_ILOGGER_H

#include "tucucommon/interface.h"

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Interface for logging messages
/// This is the main interface of the Logger component.
class ILogger : public Tucuxi::Common::Interface
{
public:
    virtual ~ILogger() {}

    /// \brief Log a "debug" message.
    /// \param _msg The message to log
    /// \return None
    virtual void debug(const char* _msg) = 0;

    /// \brief Log an "information" message.
    /// \param _msg The message to log
    /// \return None
    virtual void info(const char* _msg) = 0;

    /// \brief Log a "warning" message.
    /// \param _msg The message to log
    /// \return None
    virtual void warn(const char* _msg) = 0;

    /// \brief Log an "error" message.
    /// \param _msg The message to log
    /// \return None
    virtual void error(const char* _msg) = 0;

    /// \brief Log a "critical" message.
    /// \param _msg The message to log
    /// \return None
    virtual void critical(const char* _msg) = 0;
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_ILOGGER_H
