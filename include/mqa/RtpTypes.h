#ifndef MQA_RTPTYPES_H_
#define MQA_RTPTYPES_H_

#include "mqa_global.h"
#include "mqa_shared.h"

namespace mqa {

    enum {MAX_RTP_PAYLOAD_TYPE  = 128 };

    enum tagRTPMediaType
    {
        RTPMEDIA_UNKNOWN = 0,
        RTPMEDIA_AUDIO,
        RTPMEDIA_VIDEO,
        RTPMEDIA_AV
    } ;
    typedef int RTPMediaType;

    typedef enum tagRTPStreamType
    {
        RTPTYPE_NONE = 0,
        RTPTYPE_VOIP,
        RTPTYPE_AUDIO = 2,
        RTPTYPE_IPTV,
        RTPTYPE_VIDEO = 4,
        RTPTYPE_ISMA,
        RTPTYPE_ICC_RUDP // used for reference, do not display
    } ;
    typedef int RTPStreamType;

#define DEF_SUBCODEC(codec, num) \
	codec##_##num = (codec|(num<<RTPCODEC_BITS))

#define DEF_SUBCODECN(codec, name, num) \
	codec##_##name = (codec|(num<<RTPCODEC_BITS))

    enum RTPCodec_
    {
        RTPCODEC_MASK        = 0x00FF,
        RTPSUBCODEC_MASK     = 0xFF00,
        RTPCODEC_BITS        = 8,
        RTPSUBCODEC_BITS     = 8,

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
        RTPCODEC_AMRNB       = 128,
        RTPCODEC_AMRWB       = 129,
        RTPCODEC_EFR         = 130,
        RTPCODEC_EVRC        = 131,

        //----- other dynamic ---
        RTPCODEC_G726_A      = 132,
        RTPCODEC_G726_U      = 133,
        RTPCODEC_G711_PLC    = 134,
        RTPCODEC_RTAUDIO_NB  = 135,    // {Freq:8K,  BitRate:8.8bps, PackeSize:22bytes, Lookahead:10ms, FrameSize:20ms}
        RTPCODEC_RTAUDIO_WB  = 136,    // {Freq:16K, BitRate:18bps,  PackeSize:45bytes, Lookahead:10ms, FrameSize:20ms}

        //////////////////////////////////////////////////
        // Others
        RTPCODEC_OTHERAUDIO  = 252,
        RTPCODEC_OTHERVIDEO  = 253,
        RTPCODEC_OTHERAV     = 254,


        //////////////////////////////////////////////////
        // Unspecific
        RTPCODEC_UNSPECIFIC  = 255,


        //---- codec subtypes -------------------

        DEF_SUBCODEC(RTPCODEC_G726_A, 16),
        DEF_SUBCODEC(RTPCODEC_G726_U, 16),
        DEF_SUBCODEC(RTPCODEC_G726_A, 24),
        DEF_SUBCODEC(RTPCODEC_G726_U, 24),
        DEF_SUBCODEC(RTPCODEC_G726_A, 32),
        DEF_SUBCODEC(RTPCODEC_G726_U, 32),
        DEF_SUBCODEC(RTPCODEC_G726_A, 40),
        DEF_SUBCODEC(RTPCODEC_G726_U, 40),

        DEF_SUBCODECN(RTPCODEC_AMRNB, 0475, 1),
        DEF_SUBCODECN(RTPCODEC_AMRNB, 0515, 2),
        DEF_SUBCODECN(RTPCODEC_AMRNB, 0590, 3),
        DEF_SUBCODECN(RTPCODEC_AMRNB, 0670, 4),
        DEF_SUBCODECN(RTPCODEC_AMRNB, 0740, 5),
        DEF_SUBCODECN(RTPCODEC_AMRNB, 0795, 6),
        DEF_SUBCODECN(RTPCODEC_AMRNB, 1020, 7),
        DEF_SUBCODECN(RTPCODEC_AMRNB, 1220, 8),

        DEF_SUBCODECN(RTPCODEC_AMRWB, 0660, 1),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 0885, 2),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 1265, 3),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 1425, 4),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 1585, 5),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 1825, 6),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 1985, 7),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 2305, 8),
        DEF_SUBCODECN(RTPCODEC_AMRWB, 2385, 9),

        RTPCODEC_UNKOWN      = 0xFFFF
    };
    typedef int RTPCodec;

    typedef struct CodecListStruct {
        RTPCodec_   CodecCode;
        char        CodecName[16];
    } CodecListType;

    typedef struct
    {
        RTPCodec eCodec;
        char     szName[32];
        UINT32   nClockRate;
    } RTPPayloadCodecInfo;

    struct RtpEModelFactor
    {
        RTPCodec codec;
        double fLookahead;  // milli second
        double Ie;          // equipment impairment factor
        double Bpl;         // Packet-loss robustness factor
    };
    MQA_API RtpEModelFactor *GetEModelFactor(RTPCodec codec);
    MQA_API RTPPayloadCodecInfo *GetCodecInfo(RTPCodec codec);

    MQA_API bool RTPCodec2RTPMediaType(RTPCodec codec, RTPMediaType& mediaType, RTPStreamType& streamType) ;
    MQA_API int RTPCodec2CodecFrameSize(RTPCodec codec, int* packetSize=0);
    MQA_API UINT32 RTPCodec2ClockRate(RTPCodec codec);
    MQA_API bool CalculateRFactor(RTPCodec nCodecType,
            double    dCodecFrameSize,
            double    dRTPjitter,
            double    dRTCPDelay,
            double    dAfactor,
            double    dPacketLossRate,
            double*   Rfactor,
            double*   MOS);


}  // namespace mqa

#endif // MQA_RTPTYPES_H_
