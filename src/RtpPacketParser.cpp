#include "mqa_global.h"
#include "RtpPacketParser.h"

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
    bool RtpPacketParser::Parse()
    {
        ver        = (packetBuffer[0]&0xC0) >>6;
        bPadding   = (packetBuffer[0]&0x20) >>5;
        bExt       = (packetBuffer[0]&0x10) >>4;
        nCC        = (packetBuffer[0]&0x0F);
        bMarker    = (packetBuffer[1]&0x80) >>7;

        payloadType = (packetBuffer[1]&0x7F);
        sequenceNum = Swap16(*(UINT16*)(packetBuffer+2));
        timestamp   = Swap32(*(UINT32*)(packetBuffer+4));
        ssrc        = Swap32(*(UINT32*)(packetBuffer+8));

        nHeaderLength = 12 + nCC * 4;

        // Do the RTP sanity checks
        return !IsValid();
    }
    bool RtpPacketParser::IsValid() const
    {
        if  (packetLength < nHeaderLength || (ver != 0x80)  || (nCC > 3)) {
            return false;
        }
        return true;
    }
    const char *RtpPacketParser::GetPayload(int* len) const
    {
        if(!IsValid()) return NULL;

        if(len) *len = packetLength - nHeaderLength;
        return &packetBuffer[nHeaderLength];
    }

}  // namespace mqa
