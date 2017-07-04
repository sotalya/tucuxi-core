#ifndef TUCUXI_COMMON_LOOGER
#define TUCUXI_COMMON_LOOGER

#include "tucucommon/componentmanager.h"
#include "tucucommon/ilogger.h"

namespace Tucuxi {
namespace Common {

class LoggerHelper
{
public:
    LoggerHelper();

public:
    void debug(const char* msg);
    template <typename Arg> void debug(const Arg& arg);
    template <typename Arg1, typename... Args> void debug(const char* fmt, const Arg1 &arg1, const Args&... args);

    void info(const char* msg);
    template <typename Arg> void info(const Arg& arg);
    template <typename Arg1, typename... Args> void info(const char* fmt, const Arg1 &arg1, const Args&... args);

    void warn(const char* msg);
    template <typename Arg> void warn(const Arg& arg);
    template <typename Arg1, typename... Args> void warn(const char* fmt, const Arg1 &arg1, const Args&... args);

    void error(const char* msg);
    template <typename Arg> void error(const Arg& arg);
    template <typename Arg1, typename... Args> void error(const char* fmt, const Arg1 &arg1, const Args&... args);

    void critical(const char* msg);
    template <typename Arg> void critical(const Arg& arg);
    template <typename Arg1, typename... Args> void critical(const char* fmt, const Arg1 &arg1, const Args&... args);

private:
    ILogger *m_Logger;
};

}
}

#endif // TUCUXI_COMMON_LOOGER