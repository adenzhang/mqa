#ifndef MQA_RTPTYPES_H_
#define MQA_RTPTYPES_H_

#include "mqa_shared.h"

namespace mqa {

    typedef enum tagRTPMediaType
    {
        RTPMEDIA_UNKNOWN = 0,
        RTPMEDIA_AUDIO,
        RTPMEDIA_VIDEO,
        RTPMEDIA_AV
    } RTPMediaType;

    typedef enum tagRTPStreamType
    {
        RTPTYPE_NONE = 0,
        RTPTYPE_VOIP,
        RTPTYPE_IPTV,
        RTPTYPE_AUDIO,
        RTPTYPE_ISMA,
        RTPTYPE_VIDEO,
        RTPTYPE_ICC_RUDP // used for reference, do not display
    } RTPStreamType;

    typedef enum
    {
        //////////////////////////////////////////////////
        // RFC Reserved
        RTPCODEC_PCMU        = 0,
        RTPCODEC_1016        = 1,
        RTPCODEC_G721        = 2,
        RTPCODEC_GSM         = 3,
        RTPCODEC_G723        = 4,
        RTPCODEC_DVI4_8000   = 5,
        RTPCODEC_DVI4_16000  = 6,
        RTPCODEC_LPC         = 7,
        RTPCODEC_PCMA        = 8,
        RTPCODEC_G722        = 9,
        RTPCODEC_L16_2CH     = 10,
        RTPCODEC_L16_1CH     = 11,
        RTPCODEC_QCELP       = 12,
        RTPCODEC_CN          = 13,
        RTPCODEC_MPA         = 14,
        RTPCODEC_G728        = 15,
        RTPCODEC_DVI4_11025  = 16,
        RTPCODEC_DVI4_22050  = 17,
        RTPCODEC_G729        = 18,
        RTPCODEC_CELLB       = 25,
        RTPCODEC_JPEG        = 26,
        RTPCODEC_NV          = 28,
        RTPCODEC_H261        = 31,
        RTPCODEC_MPV         = 32,
        RTPCODEC_MP2T        = 33,
        RTPCODEC_H263        = 34,

        //////////////////////////////////////////////////
        // Standard Codec
        RTPCODEC_AMR         = 128,
        RTPCODEC_AMRWB       = 129,
        RTPCODEC_EFR         = 130,
        RTPCODEC_EVRC        = 131,

        //////////////////////////////////////////////////
        // Others
        RTPCODEC_OTHERAUDIO  = 252,
        RTPCODEC_OTHERVIDEO  = 253,
        RTPCODEC_OTHERAV     = 254,

        //////////////////////////////////////////////////
        // Unspecific
        RTPCODEC_UNSPECIFIC  = 255
    } RTPCodec;

    MQA_API bool RTPCodec2RTPMediaType(const RTPCodec codec, RTPMediaType& mediaType, RTPStreamType& streamType) ;
    MQA_API int RTPCodec2CodecFrameSize(const RTPCodec codec, int* packetSize=0);
    MQA_API int RTPCodec2Frequency(const RTPCodec codec);

}  // namespace mqa

#endif // MQA_RTPTYPES_H_