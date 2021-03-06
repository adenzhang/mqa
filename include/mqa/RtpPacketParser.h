#ifndef MQA_RTPPACKET_H_
#define MQA_RTPPACKET_H_

#include "PacketParser.h"
#include "mqa_shared.h"

namespace mqa {
    struct MQA_SHARED RtpPacketParser 
        : public PacketParser
    {
        enum {MAX_SEQ_NUM=(1<<(sizeof(UINT16)*8))-1, MIN_SEQ_NUM=0};
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
        virtual bool IsValid() const ;

        virtual const char *GetPayload(int* len) const ;
    };

}  // namespace mqa
#endif  // MQA_RTPPACKET_H_
