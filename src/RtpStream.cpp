#include "RtpStream.h"
#include <list>
#include <algorithm>

#define SetMinMax(nSample, nMin, nMax) \
    if ((nSample < nMin)) nMin = nSample; \
    if (nSample > nMax) nMax = nSample;

namespace mqa {

    struct RtpPacketSorter
    {
        bool operator()(RtpPacketParser& a, RtpPacketParser& b) {
            return a.sequenceNum < b.sequenceNum;
        }
    };
    struct RtpStreamImpl
        :public RtpStream
    {
        bool          bValidStream;

        UINT32 prevTx, currTx;
        ftl::timenano prevRx, currRx;

        ftl::timenano currDelay, prevDelay;
        ftl::timenano currJitter, prevJitter;
        double        currMOS, prevMOS;

        UINT64        nMinSeqNum, nMaxSeqNum, nRecvPackts;
        UINT32        nCodeFrameSize, nClockRate;
        int           lenPayload;

        RTPMediaType  mediaType;
        RTPCodec      codecType;
        RTPStreamType streamType;

        // following are used to detect stream
        const int NLASTPACKETS;
        typedef std::list<RtpPacketParser>    PacketList;
        PacketList                    lastPackets;  // save the last several packets.

        RtpStreamImpl() 
            : bValidStream(false)
            , nRecvPackts(0)
            , NLASTPACKETS(4)
            , currDelay(0), prevDelay(0)
            , currJitter(0), prevJitter(0)
            , currMOS(0), prevMOS(0)
        {
        }

        void pushPacket(RtpPacketParser& p) {
            lastPackets.push_back(p);
            while(lastPackets.size() > NLASTPACKETS) {
                lastPackets.pop_front();
            }
        }
        bool PacketArrival(ftl::timenano& captureTime, RtpPacketParser& packet)
        {
            if( !packet.IsValid() && !packet.Parse()) {
                bValidStream = false;
                return false;
            }
            pushPacket(packet);

            currRx = captureTime;
            currTx = packet.timestamp;

            codecType = (RTPCodec) packet.payloadType;
            packet.GetPayload(&lenPayload);
            SetMinMax(packet.sequenceNum, nMinSeqNum, nMaxSeqNum);

            nRecvPackts++;
            return true;
        }
        virtual bool IsValidStream() const
        {
            return bValidStream;
        }
        bool DetectStream() {
            bValidStream = false;
            if( lastPackets.size() < NLASTPACKETS ) return false;

            PacketList::iterator it = lastPackets.begin();
            PacketList::iterator prev = it++;
            nMinSeqNum = prev->sequenceNum;
            nMaxSeqNum = prev->sequenceNum;
            for(; it!=lastPackets.end(); ++it, ++prev) {
                if( it->packetType != prev->packetType 
                    || it->ssrc != prev->ssrc )
                    return false;
                INT64 dSeq = it->sequenceNum - prev->sequenceNum;
                INT64 dTime = it->timestamp - prev->timestamp;
                if( (dSeq ^ dTime) < 0 )  // different sign
                    return false;
                SetMinMax(it->sequenceNum, nMinSeqNum, nMaxSeqNum);
            }
            if( nMinSeqNum + NLASTPACKETS - 2 > nMaxSeqNum)  // tolerance 1
                return false;

            // update info
            bValidStream = true;
            {
                nCodeFrameSize = RTPCodec2CodecFrameSize(codecType, &lenPayload);
                RTPCodec2RTPMediaType(codecType, mediaType, streamType);
                nClockRate = RTPCodec2ClockRate(codecType);
                nRecvPackts = NLASTPACKETS;
            }
            return true;
        }
        float CalculatePacketLossRate()
        {
            return float(nRecvPackts)/(nMaxSeqNum-nMinSeqNum);
        }

        ftl::timenano CalculateOneWayDelay()
        {
            using namespace ftl;
            prevDelay = currDelay;

            // calculate current delay
            timenano  dCapture = currRx - prevRx;
            UINT32    dCountTx  = currTx - prevTx;

            timenano  dTimePacket = timenano.kSUBSEC/nClockRate*dCountTx;  // convert to nano-second

            currDelay = dCapture - dTimePacket;
            return currDelay;
        }

        //J(i) = J(i-1) + ( |D(i-1,i)| - J(i-1) )/16
        ftl::timenano CalculateJitter()
        {
            prevJitter = currJitter;
            if(currDelay<0) currDelay = - currDelay;
            currJitter = prevJitter + (currDelay-prevJitter);
            return currJitter;
        }

        bool CalculateMOS(float& mos, float& rfactor)
        {
            // todo
            return 0;
        }
    };

    RtpStream *CreateRtpStream()
    {
        return new RtpStreamImpl();
    }
    void DestroyRtpStream(RtpStream * s)
    {
        delete s;
    }
}  // namespace mqa
