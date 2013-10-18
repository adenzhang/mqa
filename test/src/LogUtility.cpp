#include "LogUtility.h"
#include <stdarg.h>
#include <stdio.h>
#include <boost/thread.hpp>

#define VQT_LOG_MAX_LEN     1024

int g_VqtLogLevel = 1;

void VqtSetLogLevel(int nLevel)
{
    g_VqtLogLevel = nLevel;
}
int VqtGetLogLevel(void)
{
    return g_VqtLogLevel;
}

void VqtLog(int nLevel, const char* sFmt, ... )
{
    va_list Args;
    va_start(Args, sFmt);
    VqtLogV(nLevel, sFmt, Args);
    va_end(Args);
}

void VqtLogV(int nLevel, const char* sFmt, va_list Args)
{
    static char LogBuffer[VQT_LOG_MAX_LEN];
    static boost::mutex Mutex;

    if (g_VqtLogLevel >= nLevel)
    {
        boost::unique_lock<boost::mutex> lock(Mutex);
#ifdef WIN32
        _vsnprintf_s(LogBuffer, VQT_LOG_MAX_LEN, _TRUNCATE, sFmt, Args);
#else
        vsnprintf(LogBuffer, VQT_LOG_MAX_LEN, sFmt, Args);
#endif
        printf(LogBuffer);
    }
}
