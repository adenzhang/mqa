#ifndef MQA_CALCULATERFACTOR_H_
#define MQA_CALCULATERFACTOR_H_

#include "RtpTypes.h"

namespace mqa{
    bool CalculateRFactor(RTPCodec nCodecType,
        double    dCodecFrameSize,
        double    dRTPjitter,
        double    dRTCPDelay,
        double    dAfactor,
        double    dPacketLossRate,
        double*   Rfactor,
        double*   MOS);

}

#endif