#ifndef TUCUXI_COMMON_LOOGER
#define TUCUXI_COMMON_LOOGER

#include "tucucommon/componentmanager.h"
//#include "tucucommon/ilogger.h"

class Logger
{
public:
	Logger() 
	{
	}

public:
	void debug(const char* msg) {}
	template <typename Arg1, typename... Args> 
	void debug(const char* fmt, const Arg1 &arg1, const Args&... args) { /* m_Logger->debug(frm, arg1, args, ...); */ }

	void info(const char* msg) {}
	template <typename Arg1, typename... Args> 	
	void info(const char* fmt, const Arg1 &arg1, const Args&... args) { /* m_Logger->info(frm, arg1, args, ...); */ }

	void warn(const char* msg) {}
	template <typename Arg1, typename... Args> 	
	void warn(const char* fmt, const Arg1 &arg1, const Args&... args) { /* m_Logger->warning(frm, arg1, args, ...); */ }

	void error(const char* msg) {}
	template <typename Arg1, typename... Args> 	
	void error(const char* fmt, const Arg1 &arg1, const Args&... args) { /* m_Logger->error(frm, arg1, args, ...); */ }

	void critical(const char* msg) {}
	template <typename Arg1, typename... Args> 
	void critical(const char* fmt, const Arg1 &arg1, const Args&... args) { /* m_Logger->critical(frm, arg1, args, ...); */ }

private:
	// ILogger *m_Logger;
};

#endif // TUCUXI_COMMON_LOOGER