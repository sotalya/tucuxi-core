/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_COMMON_LOOGER
#define TUCUXI_COMMON_LOOGER

#include "spdlog/fmt/bundled/format.h"

#include "tucucommon/componentmanager.h"
#include "tucucommon/logger.h"
#include "tucucommon/ilogger.h"

namespace Tucuxi {
namespace Common {

    /// \ingroup TucuCommon
    /// \brief Helper class to simplify logging
    ///
    /// This is a wrapper around the ILogger interface of the Logger component. This idea is to make it as 
    /// simple as possible to log messages. To log messages, just create an instance of LoggerWrapper locally or 
    /// as a member of your class and call its logging methods.
    ///
    /// Example:
    /// \code
    ///	LoggerHelper logger;
    /// logger.info("Hello, my name is {}", "Roger");
    /// \endcode
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
    }

    /// \brief Initialize the Logger component 
    /// Call this method once at startup to enable logging.
    /// @param _logFilePath Path of the generated rotating log file
    /// @return True if initialization succeeded
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

public:
    /// \brief Log a "debug" message.
    /// @param _msg The message to log
    void debug(const char* _msg)
    {
        if (m_pLogger != nullptr) {
            m_pLogger->debug(_msg);
        }
    }
    
    /// \brief Log a formatted "debug" message.
    /// @param _fmt The message format
    /// @param _args List of values referenced by _fmt
    template<typename... Args> void debug(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            debug(writer.c_str());
        }
        catch (const std::exception &ex) {
        }
    }

    /// \brief Log an "information" message.
    /// @param _msg The message to log
    void info(const char* _msg)
    {
        if (m_pLogger != nullptr) {
            m_pLogger->info(_msg);
        }
    }

    /// \brief Log a formatted "information" message.
    /// @param _fmt The message format
    /// @param _args List of values referenced by _fmt
    template<typename... Args> void info(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            info(writer.c_str());
        }
        catch (const std::exception &ex) {
        }
    }

    /// \brief Log a "warning" message.
    /// @param _msg The message to log
    void warn(const char* _msg)
    {
        if (m_pLogger != nullptr) {
            m_pLogger->warn(_msg);
        }
    }

    /// \brief Log a formatted "warning" message.
    /// @param _fmt The message format
    /// @param _args List of values referenced by _fmt
    template<typename... Args> void warn(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            warn(writer.c_str());
        }
        catch (const std::exception &ex) {
        }
    }

    /// \brief Log an "error" message.
    /// @param _msg The message to log
    void error(const char* _msg)
    {
        if (m_pLogger != nullptr) {
            m_pLogger->error(_msg);
        }
    }

    /// \brief Log a formatted "error" message.
    /// @param _fmt The message format
    /// @param _args List of values referenced by _fmt
    template<typename... Args> void error(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            error(writer.c_str());
        }
        catch (const std::exception &ex) {
        }
    }

    /// \brief Log a "critical error" message.
    /// @param _msg The message to log
    void critical(const char* _msg)
    {
        if (m_pLogger != nullptr) {
            m_pLogger->critical(_msg);
        }
    }

    /// \brief Log a formatted "critical" message.
    /// @param _fmt The message format
    /// @param _args List of values referenced by _fmt
    template<typename... Args> void critical(const char* _fmt, const Args&... _args)
    {
        try {
            fmt::MemoryWriter writer;
            writer.write(_fmt, _args...);
            critical(writer.c_str());
        }
        catch (const std::exception &ex) {
        }
    }

private:
    ILogger *m_pLogger; /// The pointer to the actual logger component
};

}
}

#endif // TUCUXI_COMMON_LOOGER