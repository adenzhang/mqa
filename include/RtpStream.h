#ifndef MQA_MQALGO_H_
#define MQA_MQALGO_H_

#include "mqa_shared.h"
#include "timesec.h"

#include "RtpTypes.h"
#include "RtpPacketParser.h"


namespace mqa {

    struct RtpStream
    {
        virtual ~RtpStream(){}

        // return false and set invalid if invalid RTP header. 
        virtual bool IndicateRtpPacket(const ftl::timenano& captureTime, const RtpPacketParser& packet) = 0;

        // check whether stream has been detected.
        virtual bool IsValidStream() const = 0;

        // check whether it is a valid RTP stream.
        // all internal variables will be reset if RTP stream is detected.
        virtual bool DetectStream() = 0;

        // step 1: calculate one way delay
        // calculate results after setting above parameters
        // return time in second
        virtual ftl::timenano CalculateOneWayDelay() = 0;

        // step 2: calculate loss rate
        virtual float CalculatePacketLossRate(UINT32& nPackets) = 0;

        // step 3: calculate jitter
        virtual ftl::timenano CalculateJitter() = 0;

        // step 4: calculate MOS & jitter
        virtual bool CalculateMOS(float& mos, float& rfactor) = 0;

    };

    MQA_API RtpStream *CreateRtpStream();
    MQA_API void DestroyRtpStream(RtpStream *);

}  // namespace mqa

#endif // MQA_MQALGO_H_
