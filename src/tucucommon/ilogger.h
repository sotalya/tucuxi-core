//@@lisence@@

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
