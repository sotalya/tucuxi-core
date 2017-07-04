/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include "tucucommon/logger.h"
#include "spdlog/spdlog.h"

#include <memory>
#include <iostream>

namespace Tucuxi {
namespace Common {

Logger::~Logger()
{
    // Under VisualStudio, this must be called before main finishes to workaround a known VS issue
    spdlog::drop_all();
}

void setLevel(spdlog::level level)
{
    m_logger->set_level(level);
}


void Logger::debug(const char* msg)
{
    m_logger->debug(msg);
    m_logger->flush();
}

void Logger::info(const char* msg)
{
    m_logger->info(msg);
    m_logger->flush();
}

void Logger::warn(const char* msg)
{
    m_logger->warn(msg);
    m_logger->flush();
}

void Logger::error(const char* msg)
{
    m_logger->error(msg);
    m_logger->flush();
}

void Logger::critical(const char* msg)
{
    m_logger->critical(msg);
    m_logger->flush();
}

Tucuxi::Common::Interface* Logger::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

Logger::Logger(const std::string &_filename)
{
    registerInterface(dynamic_cast<ILogger*>(this));

    // Spdlog will not throw exceptions while logging but it might throw during
    // the construction logger.
    try
    {
        // Creating loggers with multiple sinks. Every log are written in
        // multiple target (e.g file, console).

        // Sinks are the objects that actually write the log to their target.
        // Each sink should be responsible for only single target
        // (e.g file, console, db)

        // Thread safe sinks: sinks ending with _mt (e.g daily_file_sink_mt)
        // Non thread safe sinks: sinks ending with _st (e.g daily_file_sink_st)

        std::vector<spdlog::sink_ptr> sinks;

        // Linux console
        auto sink_unix = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();

        // Windows console
        //auto sink_win = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();

        sinks.push_back(sink_unix);

        // Sink for daily file
        //sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(_filename, "txt", 23, 59));

        // Sink for rotating file
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_filename, 1024 * 1024 * 5, 3));

        m_logger = std::make_shared<spdlog::logger>("m_logger", begin(sinks), end(sinks));

        // Debug should be display
        m_logger->set_level(spdlog::level::debug);

        // Set custom pattern
        //m_logger->set_pattern("TUCUXI [%H:%M:%S %z] [thread %t] %v");

        // Register it if you need to access it globally. In this case is useless.
        spdlog::register_logger(m_logger);

    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

}
}
