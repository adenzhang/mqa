#include "mqa/mqa_global.h"
#include "mqa/RtcpPacketParser.h"

// 16byte swap
#define Swap16(swap_nValue) \
    ((UINT16)((((swap_nValue) >> 8) & 0x00FF) | (((swap_nValue) << 8) & 0xFF00)))

// 32byte swap
#define Swap32(swap_nValue) \
    ((UINT32)((((swap_nValue) >> 24) & 0x000000FF) |  \
    (((swap_nValue) >>  8) & 0x0000FF00) |   \
    (((swap_nValue) <<  8) & 0x00FF0000) |  \
    (((swap_nValue) << 24) & 0xFF000000)))

// min value
#define Min(nMcA,nMcB) ((nMcA)>(nMcB)?(nMcB):(nMcA))
#define Max(nMcA,nMcB) ((nMcA)>(nMcB)?(nMcA):(nMcB))
#define Abs(nMcA)      ((nMcA)<0?-(nMcA):(nMcA))

namespace mqa {

    ////////////////////////////////////////////////////////////////////////////
    //
    // RTP Header (12 bytes)
    //
    // |--2--|--1--|--1--|--4-|-1-|----7----|--16-|-----32----|--32--| length in bits
    // | ver | pad | ext | cc | m | payload | seq | timestamp | ssrc |
    //
    //
    ////////////////////////////////////////////////////////////////////////////
    bool RtcpPacketParser::Parse()
    {
        // parse 8 bytes header
        ver        = (packetBuffer[0]&0xC0) >>6;
        bPadding   = (packetBuffer[0]&0x20) >>5;

        reportCount   = (packetBuffer[0]&0x1F);

        reportType =  packetBuffer[1];
        length     = Swap16(*(UINT16*)(packetBuffer + 2));
        ssrc       = Swap32(*(UINT32*)(packetBuffer + 4));

        rtcpLength = (length+1)*4;

        if( !IsValidHeader() ) return false;

        pData = (const UINT8*)packetBuffer + 8;
        switch( reportType ) {
            case SENDER_REPORT:
                ParseSenderReport();
                break;
            case RECIEVER_REPORT:
                ParseRecieverReport();
                break;
        }
        return true;
    }
    bool RtcpPacketParser::ParseSenderReport()
    {
        UINT32 nReportLength = SIZE_SENDER_REPORT_SENDER_INFO + reportType*SIZE_REPORT_BLOCK;
        if( nReportLength > rtcpLength ) return false;

        /////////////////////////////////////////////////////////////////////////
        //
        // RTCP Sender Information (20 bytes)
        //
        // |----------32---------|----------32---------|-------32------| length in bits
        // | ntp timestamp (msw) | ntp timestamp (msw) | rtp timestamp |
        //
        // |-----------32----------|----------32---------| length in bits
        // | sender's packet count | sender's byte count |
        //
        /////////////////////////////////////////////////////////////////////////
        //-- ignore sender's info

        pData += SIZE_SENDER_REPORT_SENDER_INFO;
        return ParseReceptionReports();
    }
    bool RtcpPacketParser::ParseRecieverReport()
    {
        return ParseReceptionReports();
    }

    bool RtcpPacketParser::ParseReceptionReports()
    {
        /////////////////////////////////////////////////////////////////////////
        //
        // RTCP Reception Report (24 bytes)
        //
        // |--32--|--------8-------|---------24---------| length in bits
        // | SSRC | % lost packets | total lost packets |
        //
        // |------------32------------|---32---|---------32--------| length in bits
        // | extended highest seq num | jitter | last SR timestamp |
        //
        // |---------32----------| length in bits
        // | delay since last SR |
        //
        /////////////////////////////////////////////////////////////////////////

        reports.resize(reportCount);
        int nReports = 0;
        for(int i = 0; i< reportCount; ++i) {
            UINT32 nSSRC                = Swap32(*(UINT32*)(pData +  0));
            if( nSSRC > 0) {
                StatsBlock *pBlock = new StatsBlock;
                pBlock->ssrc = nSSRC;
                pBlock->fractionLost        =                  *(pData +  4);
                pBlock->cumulativeLost    = Swap32(*(UINT32*)(pData +  4)) & 0x00FFFFFF;
                pBlock->highestSequenceNumber = Swap32(*(UINT32*)(pData +  8));
                pBlock->interarrivalJitter              = Swap32(*(UINT32*)(pData + 12));
                pBlock->lxr = Swap32(*(UINT32*)(pData + 16));
                pBlock->dlxr               = Swap32(*(UINT32*)(pData + 20));

                reports[nReports++] = ReportBlockPtr(pBlock);
            }

            pData += SIZE_REPORT_BLOCK;
        }
        return true;
    }
    bool RtcpPacketParser::IsValidHeader() const
    {
        if  ((length < rtcpLength) || 
            (ver != 0x02)     ||
            ((reportType < 200)          || 
            (reportType > 207)))
        {
            return false;
        }
        return true;
    }
    bool RtcpPacketParser::IsValid() const
    {
        return IsValidHeader();

    }
    const char *RtcpPacketParser::GetPayload(int* len) const
    {
        return NULL;
    }

}  // namespace mqa
