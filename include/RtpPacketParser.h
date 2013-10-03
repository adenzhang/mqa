#ifndef MQA_RTPPACKET_H_
#define MQA_RTPPACKET_H_

#include "PacketParser.h"

namespace mqa {
    struct RtpPacketParser 
        : public PacketParser
    {
        RtpPacketParser(const char *buf=NULL, int len=0, PacketParser *lower=0)
            : PacketParser(buf, len, kRTP_PACKET, lower) 
            , payloadType(0), sequenceNum(0), timestamp(0), ssrc(0)
            , nHeaderLength(0)
            , ver(0), nCC(0)
        {}

        UINT8     payloadType;
        UINT16    sequenceNum;
        UINT32    timestamp;
        UINT32    ssrc;

        UINT8     ver, bPadding, bExt, nCC, bMarker;
        UINT16    nHeaderLength;

        // currently header extension is not parsed.
        virtual bool Parse();
        virtual bool IsValid();

        virtual const char *GetPayload(int* len);
    };

}  // namespace mqa
#endif  // MQA_RTPPACKET_H_