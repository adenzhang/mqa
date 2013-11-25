#include "LogUtility.h"
#include <stdarg.h>
#include <stdio.h>
#include <boost/thread.hpp>

#ifdef WIN32
#include <mqa/mqa_global.h>
#endif

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
long double GetTimeMicroSec()
{
    long double atime=0;
#ifdef WIN32
    static long long freq = 0;
    long long a;
    if( 0 == freq )
        QueryPerformanceFrequency((LARGE_INTEGER*) &freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&a);
    atime = (long double)a/(long double)freq * (long double)1.0E6;
#else
    timespec t;
    if( 0 == clock_gettime(CLOCK_MONOTONIC, &t) )
        atime = (long double)t.tv_sec * (long double)1.0E6 + (long double)t.tv_nsec / (long double)1.0E3;
#endif
    return atime;
}
