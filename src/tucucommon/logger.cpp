/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include "tucucommon/logger.h"

Logger::~Logger()
{
    // Under VisualStudio, this must be called before main finishes to workaround a known VS issue
    spdlog::drop_all();
}

virtual void Logger::debug(const char* msg)
{
    m_logger->debug(msg);
}

virtual void Logger::info(const char* msg)
{
    m_logger->info(msg);
}

virtual void Logger::warn(const char* msg)
{
    m_logger->warn(msg);
}

virtual void Logger::error(const char* msg)
{
    m_logger->error(msg);
}

virtual void Logger::critical(const char* msg)
{
    m_logger->critical(msg);
}

virtual Tucuxi::Common::Interface* Logger::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

Logger::Logger()
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

        // Sink for console

        // Windows
        sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());

        // Linux
        // sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());

        // Sink for rotating file
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logfile", 1024 * 1024 * 5, 3));

        // Sink for daily file
        // sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("logfile", "txt", 23, 59));

        // Create logger
        m_logger = std::make_shared<spdlog::logger>("tucuxi_logger", begin(sinks), end(sinks));

        // Register it if you need to access it globally. In this case is useless.
        // spdlog::register_logger(m_logger);
        // m_logger = spdlog::get("tucuxi_logger");

        // We cas use multiple loggers with the same sink.
        // E.g multiple pattern can be written in the same file.
        // auto sharedFileSink = std::make_shared<spdlog::sinks::simple_file_sink_mt>("fileName.txt");
        // auto firstLogger = std::make_shared<spdlog::logger>("firstLoggerName", sharedFileSink);
        // auto secondLogger = std::make_unique<spdlog::logger>("secondLoggerName", sharedFileSink);

        m_logger.set_pattern("TUCUXI [%H:%M:%S %z] [thread %t] %v");

    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}
