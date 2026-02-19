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


#ifndef TUCUXI_TEST_MOCKLOGGER_H
#define TUCUXI_TEST_MOCKLOGGER_H

#include <string>
#include <vector>

#include "tucucommon/component.h"
#include "tucucommon/componentmanager.h"
#include "tucucommon/ilogger.h"

namespace Tucuxi {
namespace Common {

/// \brief Severity levels matching the ILogger methods
enum class LogLevel
{
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

/// \brief A single log entry captured by MockLogger
struct LogEntry
{
    LogLevel level;
    std::string message;
};

/// \brief A mock logger that captures log messages for test assertions.
///
/// This logger implements ILogger and stores all logged messages in an
/// internal vector. Tests can then inspect the messages and their severity
/// to verify that the code under test logs the expected information.
///
/// Usage:
/// \code
///     MockLogger mockLogger;
///     mockLogger.install();
///     // ... run code that logs ...
///     ASSERT_TRUE(mockLogger.hasEntryWithLevel(LogLevel::Error));
///     ASSERT_TRUE(mockLogger.hasEntryContaining("something went wrong"));
///     ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Error, "something went wrong"));
///     mockLogger.uninstall();
/// \endcode
class MockLogger : public Tucuxi::Common::Component, public ILogger
{
public:
    MockLogger()
    {
        registerInterface(dynamic_cast<ILogger*>(this));
    }

    /// \brief Registers this mock as the "Logger" component, replacing any existing one.
    /// Stores the previous logger so it can be restored by uninstall().
    void install()
    {
        ComponentManager* pCmpMgr = ComponentManager::getInstance();
        if (pCmpMgr != nullptr) {
            m_previousLogger = pCmpMgr->getComponent<ILogger>("Logger");
            if (m_previousLogger != nullptr) {
                pCmpMgr->unregisterComponent("Logger");
            }
            pCmpMgr->registerComponent("Logger", dynamic_cast<ILogger*>(this));
        }
    }

    /// \brief Unregisters this mock and restores the previous logger if any.
    void uninstall()
    {
        ComponentManager* pCmpMgr = ComponentManager::getInstance();
        if (pCmpMgr != nullptr) {
            pCmpMgr->unregisterComponent("Logger");
            if (m_previousLogger != nullptr) {
                pCmpMgr->registerComponent("Logger", m_previousLogger);
                m_previousLogger = nullptr;
            }
        }
    }

    // ILogger implementation

    void debug(const char* _msg) override
    {
        m_entries.push_back({LogLevel::Debug, _msg});
    }

    void info(const char* _msg) override
    {
        m_entries.push_back({LogLevel::Info, _msg});
    }

    void warn(const char* _msg) override
    {
        m_entries.push_back({LogLevel::Warn, _msg});
    }

    void error(const char* _msg) override
    {
        m_entries.push_back({LogLevel::Error, _msg});
    }

    void critical(const char* _msg) override
    {
        m_entries.push_back({LogLevel::Critical, _msg});
    }

    // Query methods

    /// \brief Returns all captured log entries
    const std::vector<LogEntry>& getEntries() const
    {
        return m_entries;
    }

    /// \brief Returns the number of captured log entries
    size_t size() const
    {
        return m_entries.size();
    }

    /// \brief Clears all captured log entries
    void clear()
    {
        m_entries.clear();
    }

    /// \brief Returns true if at least one entry has the given severity level
    bool hasEntryWithLevel(LogLevel _level) const
    {
        for (const auto& entry : m_entries) {
            if (entry.level == _level) {
                return true;
            }
        }
        return false;
    }

    /// \brief Returns the number of entries with the given severity level
    size_t countEntriesWithLevel(LogLevel _level) const
    {
        size_t count = 0;
        for (const auto& entry : m_entries) {
            if (entry.level == _level) {
                count++;
            }
        }
        return count;
    }

    /// \brief Returns true if at least one entry's message contains the given substring
    bool hasEntryContaining(const std::string& _substring) const
    {
        for (const auto& entry : m_entries) {
            if (entry.message.find(_substring) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    /// \brief Returns true if at least one entry matches both the level and contains the substring
    bool hasEntry(LogLevel _level, const std::string& _substring) const
    {
        for (const auto& entry : m_entries) {
            if (entry.level == _level && entry.message.find(_substring) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

protected:
    Interface* getInterface(const std::string& _name) override
    {
        return Tucuxi::Common::Component::getInterfaceImpl(_name);
    }

private:
    std::vector<LogEntry> m_entries;
    ILogger* m_previousLogger{nullptr};
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TEST_MOCKLOGGER_H
