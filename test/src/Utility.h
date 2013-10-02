#pragma once

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <string>

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

#endif //_UTILITY_H_