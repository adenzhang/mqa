#pragma once

#ifndef _OPTION_CONFIG_H_
#define _OPTION_CONFIG_H_

#include "stdint.h"
#include <string>
#include <iostream>

typedef struct _VqtConfigTraffic_s
{
    std::string sFile;
    uint32_t nStreams;
    uint32_t nThroughput;
} VqtConfigTraffic;

class CVqtConfig
{
public:
    CVqtConfig()
        : nThread(1)
        , bParseIpInfo(false)
        , bIdleRun(false)
        , bMeasureLog(false)
        , nPacketsPerPlay(10)
    {
    }

    std::vector<VqtConfigTraffic> vTraffic;
    uint32_t nThread;
    bool bParseIpInfo;
    bool bIdleRun;
    bool bMeasureLog;
    uint32_t nPacketsPerPlay;

    bool Parse(int argc, char** argv);

    friend std::ostream& operator<<(std::ostream& os, const CVqtConfig& Config);
};

#endif //_OPTION_CONFIG_H_
