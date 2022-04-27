//@@lisence@@

#include <iostream>
#include <memory>

#include "tucucommon/logger.h"

#include "spdlog/spdlog.h"

namespace Tucuxi {
namespace Common {

Logger::~Logger()
{
    // Under VisualStudio, this must be called before main finishes to workaround a known VS issue
    spdlog::drop_all();
}


void Logger::setLevel(spdlog::level::level_enum _level)
{
    m_logger->set_level(_level);
}


void Logger::debug(const char* _msg)
{
    if (m_logger != nullptr) {
        m_logger->debug(_msg);
        m_logger->flush();
    }
}


void Logger::info(const char* _msg)
{
    if (m_logger != nullptr) {
        m_logger->info(_msg);
        m_logger->flush();
    }
}


void Logger::warn(const char* _msg)
{
    if (m_logger != nullptr) {
        m_logger->warn(_msg);
        m_logger->flush();
    }
}


void Logger::error(const char* _msg)
{
    if (m_logger != nullptr) {
        m_logger->error(_msg);
        m_logger->flush();
    }
}


void Logger::critical(const char* _msg)
{
    if (m_logger != nullptr) {
        m_logger->critical(_msg);
        m_logger->flush();
    }
}


Tucuxi::Common::Interface* Logger::getInterface(const std::string& _name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

Logger::Logger(const std::string& _filename) : m_logger(nullptr)
{
    registerInterface(dynamic_cast<ILogger*>(this));

    // Spdlog will not throw exceptions while logging but it might throw during
    // the construction logger.
    try {
        // Creating loggers with multiple sinks. Every log are written in
        // multiple target (e.g file, console).

        // Sinks are the objects that actually write the log to their target.
        // Each sink should be responsible for only single target
        // (e.g file, console, db)

        // Thread safe sinks: sinks ending with _mt (e.g daily_file_sink_mt)
        // Non thread safe sinks: sinks ending with _st (e.g daily_file_sink_st)

        std::vector<spdlog::sink_ptr> sinks;

#ifdef _WIN32
        // Windows console
        spdlog::sink_ptr sinkColorConsole = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
        sinks.push_back(sinkColorConsole);
#else
        // Linux console
        spdlog::sink_ptr sinkColorConsole = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
        sinks.push_back(sinkColorConsole);
#endif

        // Sink for daily file
        //sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(_filename, "txt", 23, 59));

        // Sink for rotating file
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_filename, 1024 * 1024 * 5, 3));

        m_logger = std::make_shared<spdlog::logger>("m_logger", begin(sinks), end(sinks));

        // Debug should be display
        m_logger->set_level(spdlog::level::debug);

        // Set custom pattern
        m_logger->set_pattern("[%C-%m-%d %H:%M:%S.%e] %t %l: %v");

        // Register it if you need to access it globally. In this case is useless.
        spdlog::register_logger(m_logger);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

} // namespace Common
} // namespace Tucuxi
