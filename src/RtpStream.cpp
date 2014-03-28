#include "mqa/RtpStream.h"
#include <list>
#include <algorithm>

#pragma warning (disable:4244)

#define SetMinMax(nSample, nMin, nMax) \
    if ((nSample < nMin)) nMin = nSample; \
    if (nSample > nMax) nMax = nSample;

namespace mqa {

    template<typename timesec_t>
    inline timesec_t ToTimeSec(double sec) {
        return timesec_t(long(sec), (sec-long(sec))*timesec_t::kSUBSEC);
    }

    template <typename PacketParser_T>
    struct PacketInfo:
        public PacketParser_T
    {
        PacketInfo(const PacketParser_T& p, const ftl::timenano& t)
            : PacketParser_T(p), capTime(t)
        {}

        PacketInfo(){}
        ftl::timenano capTime; // capture time;
    };
    template < typename ElemType >
    class BoundedQueue
        : public std::list<ElemType>
    {
    public:
        typedef ElemType value_type;
        typedef std::list<value_type> BackendQueue;

        BoundedQueue(size_t minsize, size_t maxsize) 
            : minSize_(minsize), maxSize_(maxsize) 
            , BackendQueue(minsize)
        {}
        // push back and drop old ones
        void push_back(const value_type& v) {
            BackendQueue::push_back(v);
            while(size() > maxSize_) {
                BackendQueue::pop_front();
            }
        }
        // if forcePush, same as push_back; otherwise, only push if not full.
        // return true if pushed.
        bool try_push_back(const value_type& v, bool forcePush) {
            if( size() < maxSize_ ) {
                BackendQueue::push_back(v);
                return true;
            }
            return false;
        }
        bool try_pop_front(value_type& v) {
            if( size() > minSize_ ) {
                v = front();
                BackendQueue::pop_front();
                return true;
            }
            return false;
        }
        void pop_front() {
            if( size() > minSize_ ) {
                BackendQueue::pop_front();
            }
        }
        size_t min_size() const { return minSize_;}
        size_t max_size() const { return maxSize_;}
    protected:
        const size_t minSize_;  // min size
        const size_t maxSize_;  // max size
        
    };
    typedef PacketInfo<RtpPacketParser> RtpPacketInfo;
    typedef PacketInfo<RtcpPacketParser> RtcpPacketInfo;
    typedef BoundedQueue<RtpPacketInfo >     PacketQueue;
    typedef BoundedQueue<RtcpPacketInfo >    RtcpPacketQueue;

    struct RtpPacketSorter
    {
        bool operator()(RtpPacketInfo& a, RtpPacketInfo& b) {
            return a.sequenceNum < b.sequenceNum;
        }
    };
    struct RtpStreamImpl
        :public RtpStream
    {
        enum {MAX_NOISE_PACKETS = 1};
        enum {MAX_DETECT_PACKETS = 4, NLAST_RTCP = 10};

        UINT8          bValidStream:1;

        UINT32        prevTx, currTx;
        ftl::timenano prevRx, currRx;

        ftl::timenano currDelay, prevDelay;
        ftl::timenano currJitter, prevJitter;
        double        currMOS, prevMOS;

        UINT64        nMinSeqNum, nMaxSeqNum;
        UINT32        nRecvPackts;
        UINT32        nCodecFrameSize, nClockRate;
        int           lenPayload;
        UINT32        userOnewayDelay;

        RTPMediaType  mediaType;
        RTPCodec      codecType;
        RTPStreamType streamType;

        // following are used to detect stream
        const unsigned int     NLASTPACKETS;
        PacketQueue    lastPackets;  // save the last several packets.

        const int     NLastRtcpPackets;
        RtcpPacketQueue lastRtcpPackets;

        // noise packets 
        int           nNoisePackets;

        UINT32          nRtcpPackets;
        UINT32          ssrc;
        ftl::timenano   captureDelay;

        RtpStreamImpl(int nDetectPackets = MAX_DETECT_PACKETS, int nNoisePackets = MAX_NOISE_PACKETS)
            : bValidStream(false)
            , nRecvPackts(0)
            , NLASTPACKETS(nDetectPackets)
            , currDelay(0), prevDelay(0)
            , currJitter(0), prevJitter(0)
            , currMOS(0), prevMOS(0)
            , nMinSeqNum(0), nMaxSeqNum(0)
            , nNoisePackets(nDetectPackets)
            , lastPackets(0, nDetectPackets)
            , nRtcpPackets(0)
            , NLastRtcpPackets(NLAST_RTCP)
            , lastRtcpPackets(0, NLastRtcpPackets)
            , codecType(0), mediaType(0),streamType(0) 
            , userOnewayDelay(0)
            , ssrc(0)
        {
        }
        void reset(int nDetectPackets, int nNoisePackets)
        {
            new (this) RtpStreamImpl(nDetectPackets, nNoisePackets);
        }
        // only check some fields to verify RTP packet alive.
        // for detailed check, should use DetectStream().
        static bool fastCheckRTP(const RtpPacketParser& prev, const RtpPacketParser& curr)
        {
            return prev.ver == curr.ver
            && prev.nHeaderLength == curr.nHeaderLength
            && prev.nCC == curr.nCC
            && prev.ssrc == curr.ssrc
            //&& prev.payloadType == curr.payloadType  // payload type may dynamicly change
            ;
        }
        void invalidateStream() {
            nMinSeqNum = RtpPacketParser::MAX_SEQ_NUM;
            nMaxSeqNum = RtpPacketParser::MIN_SEQ_NUM;
            //lastPackets.pop_front();  // only drop one packet. other packets may be valid RTP packets.
            bValidStream= false;
            nNoisePackets = 0;
        }
        // return true if it's noise packet and discard the packet;
        // return false indicates the stream has stopped, reinitialize stream.
        bool handleNoisePacket(RtpPacketParser& packet)
        {
            if( bValidStream ){  // only check nose packet when valid RTP stream.
                if( ++nNoisePackets > MAX_NOISE_PACKETS ) {
                    // the stream stopped, reinitialize.
                    invalidateStream();
                    return false;
                }else{
                    // it's noise packet, drop it.
                    return true;
                }
            }else{  // otherwise, it is still invalid RTP stream.
                return false;
            }
        }
        bool IndicateRtpPacket(const ftl::timenano& captureTime, const RtpPacketParser& apacket)
        {
            RtpPacketParser packet(apacket);
            if( !packet.IsValid() && !packet.Parse()) {
                return handleNoisePacket(packet);
            }
            if( lastPackets.size()>0 ) {
                const RtpPacketParser& prevPacket= lastPackets.back();
                bool bConsistent = fastCheckRTP(prevPacket, packet);
                if( bValidStream && !bConsistent ) {
                    return handleNoisePacket(packet);
                }
                if( bConsistent && prevPacket.sequenceNum >= packet.sequenceNum ) { // check duplicate
                    //nRecvPackts++; 
                    return true;
                }
            }
            nNoisePackets = 0;
            lastPackets.push_back(RtpPacketInfo(packet, captureTime));

            currRx = captureTime;
            currTx = packet.timestamp;

            packet.GetPayload(&lenPayload);
            SetMinMax(packet.sequenceNum, nMinSeqNum, nMaxSeqNum);

            nRecvPackts++;
            return true;
        }
        bool IsAssociated(const RtcpPacketParser& rtcp, const RtpPacketParser& rtp ) {
            if( rtcp.ssrc == rtp.ssrc ) 
                return true;
            if( rtcp.reports.size() > 0 ) {
                if( rtcp.reports[0]->ssrc == rtp.ssrc )
                    return true;
            }
            return false;
        }
        bool IndicateRtcpPacket(const ftl::timenano& captureTime, const RtcpPacketParser& apacket)
        {
            RtcpPacketParser packet(apacket);
            if( !packet.IsValid() && !packet.Parse()) {
                return false;
            }
            if(lastRtcpPackets.size()>0) {
                RtcpPacketInfo& prev = lastRtcpPackets.back();
                if( prev.NTPTimeLSW == apacket.NTPTimeLSW && prev.NTPTimeMSW == apacket.NTPTimeMSW 
                    && prev.sendersPacketCount == apacket.sendersPacketCount && prev.sendersByteCount == apacket.sendersByteCount )
                    return false;
            }
            if( packet.reports.size()>0 && lastPackets.size()>0) {
                //if( lastRtcpPackets.size() > 0 ) {
                //    if( packet == dynamic_cast<RtcpPacketParser&>(lastRtcpPackets.back()) ) {
                //        nRtcpPackets++;
                //        return true;
                //    }
                //}
                if( IsAssociated(packet, lastPackets.front() ) ) {
                    RtcpPacketInfo curr(packet, captureTime);
                    if( lastRtcpPackets.size()>0 ) 
                    {
                        // calculate delay
                        RtcpPacketInfo& prev = lastRtcpPackets.back();
                        if( curr.reports[0]->ssrc == ssrc && prev.ssrc == ssrc )
                        {
                            calcRtcpDelay(captureDelay, curr, prev);
                        }
                    }
                    lastRtcpPackets.push_back(curr);
                    nRtcpPackets++;
                    return true;
                }
            }
            return false;
        }
        virtual bool IsValidStream() const
        {
            return bValidStream;
        }
        bool DetectStream() {
            bValidStream = true;
            if( lastPackets.size() < NLASTPACKETS ) {
                invalidateStream();
                return false;
            }

            PacketQueue::iterator it = lastPackets.begin();
            PacketQueue::iterator prev = it++;
            nMinSeqNum = prev->sequenceNum;
            nMaxSeqNum = prev->sequenceNum;
            for(; it!=lastPackets.end(); ++it, ++prev) {
                if( !fastCheckRTP(*prev, *it) ) {
                    bValidStream = false;
                    break;
                }
                INT64 dSeq = it->sequenceNum - prev->sequenceNum;
                INT64 dTime = it->timestamp - prev->timestamp;
                if( (dSeq ^ dTime) < 0 ) { // different sign
                    bValidStream = false;
                    break;
                }
                SetMinMax(it->sequenceNum, nMinSeqNum, nMaxSeqNum);
            }
            if(  !bValidStream || nMinSeqNum + NLASTPACKETS - 2 > nMaxSeqNum)  {// tolerance 1
                invalidateStream();
                return false;
            }

            // update info
            bValidStream = true;
            prev--;
            {
                codecType = prev->payloadType;
                nCodecFrameSize = RTPCodec2CodecFrameSize(codecType, &lenPayload);
                RTPCodec2RTPMediaType(codecType, mediaType, streamType);
                nClockRate = RTPCodec2ClockRate(codecType);
                nRecvPackts = NLASTPACKETS;

                prevTx = prev->timestamp;
                prevRx = prev->capTime;
                ssrc = prev->ssrc;
            }
            return true;
        }
        inline RtcpPacketParser::StatsBlock *GetRtcpStatsReport() {
            if( nRtcpPackets && lastRtcpPackets.size()>0 ){
                for(RtcpPacketQueue::reverse_iterator it = lastRtcpPackets.rbegin();
                    it != lastRtcpPackets.rend(); it++ ) 
                {
                    if( it->reports[0]->ssrc == ssrc ) {
                        return (RtcpPacketParser::StatsBlock*)it->reports[0].get();
                    }
                }
            }
            return NULL;
        }
        inline float GetRtcpPacketLossRate(RtcpPacketParser::StatsBlock* pBlock, UINT32& nPackets) {
            nPackets = pBlock->highestSequenceNumber - nMinSeqNum + 1;
            return pBlock->fractionLost / 256.0F;
            //return  float(pBlock->cumulativeLost)/nPackets;
        }
        inline bool calcRtcpDelay(ftl::timenano& delay, const RtcpPacketInfo& currRtcp, const RtcpPacketInfo& prevRtcp) 
        {
            //if( currRtcp.reports[0]->ssrc == ssrc && prevRtcp.ssrc == ssrc )
            {
                ftl::timenano dCapture = currRtcp.capTime - prevRtcp.capTime;
                delay = dCapture - ToTimeSec<ftl::timenano>(currRtcp.reports[0]->dlxr/65536.0F);
                if(delay < 0 ) {
                    delay = 0;
                    return false;
                }
                return true;
            }
            return false;
        }
        inline bool GetRtcpOnewayDelay(ftl::timenano& delay) {
            if( captureDelay.as<int>() == 0 ) return false;
            delay = captureDelay;
            return true;
            RtcpPacketQueue::reverse_iterator itCurr = lastRtcpPackets.rbegin();
            RtcpPacketQueue::reverse_iterator itPrev = itCurr;
            itPrev++;
            for(;lastRtcpPackets.rend() != itPrev; itCurr++, itPrev++)
            {
                //if( itPrev->CompactNTPTime != 0 && itCurr->reports[0]->lxr == itPrev->CompactNTPTime ) 
                if( itCurr->reports[0]->ssrc == ssrc && itPrev->ssrc == ssrc )
                {
                    return calcRtcpDelay(delay, *itCurr, *itPrev);
                }
            }; 
            return false;
        }
        inline ftl::timenano TimeCount2Nano(UINT32 timestamp) {
            return float(timestamp)/nClockRate*ftl::timenano::kSUBSEC;
        }
        inline ftl::timenano GetRtcpJitter(RtcpPacketParser::StatsBlock* pBlock) {
            return TimeCount2Nano(pBlock->interarrivalJitter);
        }
        float CalculatePacketLossRate(UINT32& nPackets)
        {
            if( RtcpPacketParser::StatsBlock *pRtcpStats = GetRtcpStatsReport() ) {
                //--- use rtcp
                return GetRtcpPacketLossRate(pRtcpStats, nPackets);
            }
            nPackets = nRecvPackts;
            float lossrate = float(nRecvPackts-(nMaxSeqNum-nMinSeqNum+1))/(nMaxSeqNum-nMinSeqNum+1);
            if( lossrate<0 ) lossrate = 0;
            return lossrate;
        }

        bool CalculateOneWayDelay(ftl::timenano& delay)
        {
            if(userOnewayDelay>0) {
                delay.nsec = userOnewayDelay*1000000;
                return true;
            }
            if( lastRtcpPackets.size() > 1 ) {
                //--- use rtcp
                return GetRtcpOnewayDelay(delay);
            }
            return false;
            using namespace ftl;
            prevDelay = currDelay;

            // calculate current delay
            timenano  dCapture = currRx - prevRx;
            UINT32    dCountTx  = currTx - prevTx;
            prevRx = currRx;
            prevTx = currTx;

            timenano  dTimePacket = TimeCount2Nano(dCountTx);  // convert to nano-second

            currDelay = dCapture - dTimePacket;
            delay = currDelay<0?(-currDelay):currDelay;
            return true;
        }

        //J(i) = J(i-1) + ( |D(i-1,i)| - J(i-1) )/16
        ftl::timenano CalculateJitter()
        {
            if( RtcpPacketParser::StatsBlock *pRtcpStats = GetRtcpStatsReport() ) {
                //--- use rtcp
                return GetRtcpJitter(pRtcpStats);
            }
            prevJitter = currJitter;

            ftl::timenano D = currDelay > prevDelay? (currDelay-prevDelay) : (prevDelay-currDelay);
            currJitter = prevJitter + (D-prevJitter)*(1.0/16);
            return currJitter;
        }

        bool CalculateMOS(float& mos, float& rfactor, const ftl::timenano& onewayDelay, const ftl::timenano& jitter, float packetLossRate)
        {
            double MOS, RFactor;
            bool ret;
            double fJitter = jitter.as<double>()/1000000;
            double fDelay = onewayDelay.as<double>()/1000000;
            double lossRate = packetLossRate;
            ret = CalculateRFactor(codecType, nCodecFrameSize, fJitter>0?fJitter:-fJitter
                , fDelay>0?fDelay:-fDelay, 0.0, lossRate, &RFactor, &MOS);
            if( ret ) {
                mos = MOS;
                rfactor = RFactor;
            }
            return ret;
        }
        bool SetCodecType(INT16 codec)
        {
            codecType = (RTPCodec)codec;
            nCodecFrameSize = RTPCodec2CodecFrameSize(codecType, &lenPayload);
            RTPCodec2RTPMediaType(codecType, mediaType, streamType);
            nClockRate = RTPCodec2ClockRate(codecType);
            return true;
        }
        void SetUserOnewayDelay(UINT32 milli)
        {
            userOnewayDelay = milli;
        }
        virtual INT16 GetCodecType()
        {
            return codecType;
        }
        RTPMediaType GetMediaType()
        {
            return mediaType;
        }
        UINT32 GetRtcpPacketCount()
        {
            return nRtcpPackets;
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
