#pragma once

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <string>

#ifndef WIN32
#include <stdarg.h>
#endif

#define VQT_LOG_ERROR   1
#define VQT_LOG_INFO    2
#define VQT_LOG_DEBUG   3

void VqtSetLogLevel(int nLevel);
int VqtGetLogLevel(void);

void VqtLogV(int nLevel, const char* sFmt, va_list Args);
void VqtLog(int nLevel, const char* sMsg, ... );
#define VqtError(sMsg, ...) VqtLog(VQT_LOG_ERROR, sMsg, __VA_ARGS__)
#define VqtInfo(sMsg, ...) VqtLog(VQT_LOG_INFO, sMsg, __VA_ARGS__)
#define VqtDebug(sMsg, ...) VqtLog(VQT_LOG_DEBUG, sMsg, __VA_ARGS__)

#define SetMinMax(nSample, nMin, nMax) \
    if ((nSample < nMin)) nMin = nSample; \
    if (nSample > nMax) nMax = nSample;

#define InitMinMax(nSample, nMin, nMax) \
    nMin = nSample; \
    nMax = nSample;

long double GetTimeMicroSec();

////=================== timer begin ==========================
#define USE_CHRONO
#ifdef USE_CHRONO
#include <boost/chrono.hpp>
typedef boost::chrono::steady_clock::time_point TimePoint;

#define  ToMicro(tp)   boost::chrono::duration_cast<boost::chrono::microseconds>(tp).count()
#define ToMilli(tp)    boost::chrono::duration_cast<boost::chrono::milliseconds>(tp).count()
TimePoint Now() ;
#else
typedef long double TimePoint;
#define ToMicro(x) int( x )
#define ToMilli(x) int( x/1000 )
long double Now();
#endif
#ifndef WIN32  // linux
#include <unistd.h>
#define Sleep(milli) usleep(milli*1000)
#endif
////=================== timer end ==========================

#endif //_UTILITY_H_
