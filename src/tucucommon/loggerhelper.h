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


#ifndef TUCUXI_COMMON_LOOGER
#define TUCUXI_COMMON_LOOGER

#include "tucucommon/componentmanager.h"
#include "tucucommon/ilogger.h"
#include "tucucommon/logger.h"

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Helper class to simplify logging
///
/// This is a wrapper around the ILogger interface of the Logger component. This idea is to make it as
/// simple as possible to log messages. To log messages, just create an instance of LoggerHelper locally or
/// as a member of your class and call its logging methods.
///
/// Example:
/// \code
///	LoggerHelper logger;
/// logger.info("Hello, my name is {}", "Roger");
/// \endcode
///
/// Format documentation: http://fmtlib.net/latest/syntax.html
///
/// \sa ILogger
class LoggerHelper
{
public:
    /// \brief Constructor
    LoggerHelper()
    {
        // Get the "Logger" component from the component manager
        ComponentManager* pCmpMgr = ComponentManager::getInstance();
        if (pCmpMgr != nullptr) {
            m_pLogger = pCmpMgr->getComponent<ILogger>("Logger");
        }
        else {
            m_pLogger = nullptr;
        }
    }

    /// \brief Initialize the Logger component
    /// Call this method once at startup to enable logging.
    /// \param _logFilePath Path of the generated rotating log file
    /// \return True if initialization succeeded
    static bool init(const std::string& _logFilePath)
    {
        // Initialization of the logger
        // Create the singleton instance if not present yet in the component manager
        ComponentManager* pCmpMgr = ComponentManager::getInstance();
        if (pCmpMgr != nullptr) {
            ILogger* iLogger = pCmpMgr->getComponent<ILogger>("Logger");
            if (iLogger == nullptr) {
                // The "Logger" component does not exist yet, let's create and register it
                Logger* pLogger = new Logger(_logFilePath);
                if (pLogger != nullptr) {
                    pCmpMgr->registerComponent("Logger", dynamic_cast<ILogger*>(pLogger));
                }
            }
            return true;
        }
        return false;
    }

    ///
    /// \brief Deletes the Logger component created by init()
    ///
    /// This method shall be called just before exiting the application to release
    /// the Logger allocated by init().
    /// It is not mandatory to use it, as memory will be released by the OS, but
    /// it allows to clean everything and avoid a memory leak warning when using
    /// valgrind --leak-ckeck=full
    ///
    static void beforeExit()
    {
        ComponentManager* pCmpMgr = ComponentManager::getInstance();
        if (pCmpMgr != nullptr) {
            ILogger* iLogger = pCmpMgr->getComponent<ILogger>("Logger");
            if (iLogger != nullptr) {
                // If a logger is found, unregister and delete it
                pCmpMgr->unregisterComponent("Logger");
                delete iLogger;
            }
        }
    }

    /// \brief Log a "debug" message.
    /// \param _msg The message to log
    void debug(const char* _msg)
    {
        if (!enabled()) {
            return;
        }
        if (m_pLogger != nullptr) {
            m_pLogger->debug(_msg);
        }
    }

    /// \brief Log a "debug" message.
    /// \param _msg The message to log
    void debug(const std::string& _msg)
    {
        debug(_msg.c_str());
    }

    /// \brief Log a formatted "debug" message.
    /// \param _fmt The message format
    /// \param _args List of values referenced by _fmt
    template<typename... Args>
    void debug(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            debug(writer.c_str());
        }
        catch (const std::exception& ex) {
        }
    }

    /// \brief Log an "information" message.
    /// \param _msg The message to log
    void info(const char* _msg)
    {
        if (!enabled()) {
            return;
        }
        if (m_pLogger != nullptr) {
            m_pLogger->info(_msg);
        }
    }

    /// \brief Log an "information" message.
    /// \param _msg The message to log
    void info(const std::string& _msg)
    {
        info(_msg.c_str());
    }

    /// \brief Log a formatted "information" message.
    /// \param _fmt The message format
    /// \param _args List of values referenced by _fmt
    template<typename... Args>
    void info(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            info(writer.c_str());
        }
        catch (const std::exception&) {
        }
    }

    /// \brief Log a "warning" message.
    /// \param _msg The message to log
    void warn(const char* _msg)
    {
        if (!enabled()) {
            return;
        }
        if (m_pLogger != nullptr) {
            m_pLogger->warn(_msg);
        }
    }

    /// \brief Log a "warning" message.
    /// \param _msg The message to log
    void warn(const std::string& _msg)
    {
        warn(_msg.c_str());
    }

    /// \brief Log a formatted "warning" message.
    /// \param _fmt The message format
    /// \param _args List of values referenced by _fmt
    template<typename... Args>
    void warn(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            warn(writer.c_str());
        }
        catch (const std::exception&) {
        }
    }

    /// \brief Log an "error" message.
    /// \param _msg The message to log
    void error(const char* _msg)
    {
        if (!enabled()) {
            return;
        }
        if (m_pLogger != nullptr) {
            m_pLogger->error(_msg);
        }
    }

    /// \brief Log an "error" message.
    /// \param _msg The message to log
    void error(const std::string& _msg)
    {
        error(_msg.c_str());
    }

    /// \brief Log a formatted "error" message.
    /// \param _fmt The message format
    /// \param _args List of values referenced by _fmt
    template<typename... Args>
    void error(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            error(writer.c_str());
        }
        catch (const std::exception& /*ex*/) {
        }
    }

    /// \brief Log a "critical error" message.
    /// \param _msg The message to log
    void critical(const char* _msg)
    {
        if (!enabled()) {
            return;
        }
        if (m_pLogger != nullptr) {
            m_pLogger->critical(_msg);
        }
    }

    /// \brief Log a "critical" message.
    /// \param _msg The message to log
    void critical(const std::string& _msg)
    {
        critical(_msg.c_str());
    }

    /// \brief Log a formatted "critical" message.
    /// \param _fmt The message format
    /// \param _args List of values referenced by _fmt
    template<typename... Args>
    void critical(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            critical(writer.c_str());
        }
        catch (const std::exception&) {
        }
    }


    /// Allows to disable the logger, for testing purpose
    static void enable()
    {
        smEnable() = true;
    }

    static void disable()
    {
        smEnable() = false;
    }

    static bool& smEnable()
    {
        static bool sm_enable = true; // NOLINT(readability-identifier-naming)
        return sm_enable;
    }

    static bool enabled()
    {
        return smEnable();
    }

private:
    ILogger* m_pLogger; /// The pointer to the actual logger component
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_COMMON_LOOGER
