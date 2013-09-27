#include "MQAlgo.h"

#define SetMinMax(nSample, nMin, nMax) \
    if ((nSample < nMin)) nMin = nSample; \
    if (nSample > nMax) nMax = nSample;

namespace mqa {

    struct RtpStreamAlgo
        :public StreamAlgo
    {
        RTPStreamInfo info;


        ftl::timenano prevTx, currTx;
        ftl::timenano prevRx, currRx;

        unsigned int  currDelay, prevDelay;
        UINT64        nMinSeqNum, nMaxSeqNum, nRecvPackts;
        unsigned int  nCodeFrameSize, codecType, nFreq;

        RTPMediaType  mediaType;

        //RtpPacket     packet;

        RtpStreamAlgo() 
            :nRecvPackts(0)
        {
        }
        void SetStreamInfo(RTPStreamInfo& info) 
        {
            this->info = info;
        }

        void PacketArrival(ftl::timenano& captureTime, RtpPacket& packet)
        {
            prevRx = currRx;
            prevTx = currRx;

            currRx = captureTime;
            currTx = packet.timestamp;
            //this->packet = packet;

            // if first arrival
            if(nRecvPackts) {
                codecType = packet.payloadType;
                nCodeFrameSize = RTPCodec2CodecFrameSize(codecType, packet.GetPayload());
                mediaType = RTPCodec2RTPMediaType(codecType);
                SetMinMax(packet.sequenceNum, nMinSeqNum, nMaxSeqNum);
                nFreq = RTPCodec2Frequency(codecType);
            }
            nRecvPackts++;
        }

        ftl::timenano CalculateOneWayDelay()
        {
            prevDelay = currDelay;
            // todo calculate current delay
            return 0;
        }

        float CalculateJitter()
        {
            // todo
            return 0;
        }

        float CalculateMOS()
        {
            // todo
            return 0;
        }
    };

    StreamAlgo *CreateStreamAlgo()
    {
        // todo
        return NULL;
    }
}  // namespace mqa
