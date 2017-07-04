#include "loggerhelper.h"

namespace Tucuxi {
namespace Common {

LoggerHelper::LoggerHelper()
{
    ComponentManager* pCmpMgr = ComponentManager::getInstance();
    if (pCmpMgr != nullptr) { 
        m_Logger = pCmpMgr->getComponent<ILogger>("Logger");
    }
}


void LoggerHelper::debug(const char* msg)
{
    m_Logger->debug(msg);
}


template <typename Arg> 
void LoggerHelper::debug(const Arg& arg)
{
}


template <typename Arg1, typename... Args> 
void LoggerHelper::debug(const char* fmt, const Arg1 &arg1, const Args&... args)
{
}


void LoggerHelper::info(const char* msg)
{
    m_Logger->info(msg);
}


template <typename Arg> 
void LoggerHelper::info(const Arg& arg)
{
}


template <typename Arg1, typename... Args> 
void LoggerHelper::info(const char* fmt, const Arg1 &arg1, const Args&... args)
{
}


void LoggerHelper::warn(const char* msg)
{
    m_Logger->warn(msg);
}


template <typename Arg> 
void LoggerHelper::warn(const Arg& arg)
{
}


template <typename Arg1, typename... Args> 
void LoggerHelper::warn(const char* fmt, const Arg1 &arg1, const Args&... args)
{
}


void LoggerHelper::error(const char* msg)
{
    m_Logger->error(msg);
}


template <typename Arg> 
void LoggerHelper::error(const Arg& arg)
{
}


template <typename Arg1, typename... Args> 
void LoggerHelper::error(const char* fmt, const Arg1 &arg1, const Args&... args)
{
}


void LoggerHelper::critical(const char* msg)
{
    m_Logger->critical(msg);
}


template <typename Arg> 
void LoggerHelper::critical(const Arg& arg)
{
}


template <typename Arg1, typename... Args> 
void LoggerHelper::critical(const char* fmt, const Arg1 &arg1, const Args&... args)
{
}

}
}