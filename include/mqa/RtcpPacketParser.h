#ifndef MQA_RTCPPACKET_H_
#define MQA_RTCPPACKET_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include "PacketParser.h"
#include "mqa_shared.h"

namespace mqa {
    struct MQA_SHARED RtcpPacketParser 
        : public PacketParser
    {
        enum {MAX_REPORTS = ((1<<5)-1), SIZE_SENDER_REPORT_SENDER_INFO = 20, SIZE_REPORT_BLOCK=24};
        enum REPORT_TYPE{SENDER_REPORT=200, RECIEVER_REPORT=201, SOURCE_DESCIPTION = 202};
        struct ReportBlock {
            virtual ~ReportBlock(){}
        };
        typedef boost::shared_ptr<ReportBlock> ReportBlockPtr;

        struct StatsBlock: public ReportBlock {
            UINT32 ssrc;
            UINT8  fractionLost;
            UINT32 cumulativeLost;
            UINT32 highestSequenceNumber;
            UINT32 interarrivalJitter;
            UINT32 lxr; //last SR/RR
            UINT32 dlxr; // delay since last SR/RR
        };
        RtcpPacketParser(const char *buf=NULL, int len=0, PacketParser *lower=0)
            : PacketParser(buf, len, kRTP_PACKET, lower) 
            , timestamp(0), ssrc(0)
            , ver(0)
            , reportType(0)
        {}

        UINT8     reportType;
        UINT32    timestamp;
        UINT32    ssrc;
        UINT16    length;
        UINT8     reportCount;

        typedef std::vector<ReportBlockPtr> ReportList;
        ReportList     reports;

        UINT8     ver, bPadding;

        UINT16    rtcpLength; // RTCP packet length

        // currently header extension is not parsed.
        virtual bool Parse();
        virtual bool IsValid() const ;
        virtual bool IsValidHeader() const ;

        virtual const char *GetPayload(int* len) const ;

        bool ParseSenderReport();
        bool ParseRecieverReport();
        bool ParseReceptionReports();

        const UINT8 *pData;
    };

}  // namespace mqa
#endif  // MQA_RTCPPACKET_H_
