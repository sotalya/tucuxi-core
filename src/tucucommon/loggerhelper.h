#ifndef TUCUXI_COMMON_LOOGER
#define TUCUXI_COMMON_LOOGER

#include "spdlog/fmt/bundled/format.h"

#include "tucucommon/logger.h"
#include "tucucommon/ilogger.h"

namespace Tucuxi {
namespace Common {

class LoggerHelper
{
public:
    LoggerHelper()
    {
        // Get the "Logger" component from the component manager
        ComponentManager* pCmpMgr = ComponentManager::getInstance();
        if (pCmpMgr != nullptr) {
            m_Logger = pCmpMgr->getComponent<ILogger>("Logger");
        }
    }

    static bool init(const std::string& _logFilePath)
    {
        // Initialization of the logger
        // Create the singleton instance if not found in the component manager
        ComponentManager* pCmpMgr = ComponentManager::getInstance();
        if (pCmpMgr != nullptr) {
            ILogger* iLogger = pCmpMgr->getComponent<ILogger>("Logger");
            if (iLogger == nullptr) {
                // The "Logger" component does not exist yet, let's create and register it
                Logger* pLogger = new Logger();
                pCmpMgr->registerComponent("Logger", dynamic_cast<ILogger*>(pLogger));
            }
            return true;
        }        
        return false;
    }

public:
    void debug(const char* _msg)
    {
        m_Logger->debug(_msg);
    }
    
    template<typename... Args> void debug(const char* _fmt, const Args&... _args)
    {
        fmt::MemoryWriter writer;
        writer.write(_fmt, _args...);
        debug(writer.c_str());
    }

    void info(const char* _msg)
    {
        m_Logger->info(_msg);
    }

    template<typename... Args> void info(const char* _fmt, const Args&... _args)
    {
        fmt::MemoryWriter writer;
        writer.write(_fmt, _args...);
        info(writer.c_str());
    }

    void warn(const char* _msg)
    {
        m_Logger->warn(_msg);
    }

    template<typename... Args> void warn(const char* _fmt, const Args&... _args)
    {
        fmt::MemoryWriter writer;
        writer.write(_fmt, _args...);
        warn(writer.c_str());
    }

    void error(const char* _msg)
    {
        m_Logger->error(_msg);
    }

    template<typename... Args> void error(const char* _fmt, const Args&... _args)
    {
        fmt::MemoryWriter writer;
        writer.write(_fmt, _args...);
        error(writer.c_str());
    }

    void critical(const char* _msg)
    {
        m_Logger->critical(_msg);
    }

    template<typename... Args> void critical(const char* _fmt, const Args&... _args)
    {
        fmt::MemoryWriter writer;
        writer.write(_fmt, _args...);
        critical(writer.c_str());
    }

private:
    ILogger *m_Logger;
};

}
}

#endif // TUCUXI_COMMON_LOOGER