#include "mqa_rtptypes.h"

namespace mqa {

    bool RTPCodec2RTPMediaType(const RTPCodec codec, RTPMediaType& mediaType, RTPStreamType& streamType) 
    {
        switch (codec)
        {
        case RTPCODEC_PCMU:   // PCMU
        case RTPCODEC_G723:   // G.723
        case RTPCODEC_PCMA:   // PCMA
        case RTPCODEC_G722:   // G.722
        case RTPCODEC_G729:   // G.729
        case RTPCODEC_GSM:         // GSM
        case RTPCODEC_DVI4_8000:   // DVI4-8khz
        case RTPCODEC_LPC:         // LPC
        case RTPCODEC_QCELP:       // QcelP
        case RTPCODEC_CN:          // CN
        case RTPCODEC_G728:        // G.728
        case RTPCODEC_DVI4_16000:  // DVI4-16khz
        case RTPCODEC_L16_2CH:     // L16-2 channel
        case RTPCODEC_L16_1CH:     // L16-1 channel
        case RTPCODEC_MPA:         // MPA
        case RTPCODEC_DVI4_11025:  // DVI4-11.025khz
        case RTPCODEC_DVI4_22050:  // DVI4-22.050khz
        case RTPCODEC_AMR:         // AMR
        case RTPCODEC_AMRWB:       // AMR-WB
        case RTPCODEC_EFR:         // EFR
        case RTPCODEC_EVRC:        // EVRC
        case RTPCODEC_OTHERAUDIO:  // Unknown Audio
            streamType = RTPTYPE_AUDIO;
            mediaType  = RTPMEDIA_AUDIO;
            break;

        case RTPCODEC_CELLB:       // CelB
        case RTPCODEC_JPEG:        // JPEG
        case RTPCODEC_NV:          // NV
        case RTPCODEC_H261:        // H.261
        case RTPCODEC_MPV:         // MPV
        case RTPCODEC_H263:        // H.263
        case RTPCODEC_OTHERVIDEO:  // Unknown Video
            streamType = RTPTYPE_VIDEO;
            mediaType  = RTPMEDIA_VIDEO;
            break;

        case RTPCODEC_MP2T:        // MP2T
        case RTPCODEC_OTHERAV:     // Unknown Audio+Video
            streamType = RTPTYPE_IPTV;
            mediaType  = RTPMEDIA_AV;
            break;

        default:
            return false;
            break;
        }
        return true;
    }
    int RTPCodec2CodecFrameSize(const RTPCodec codec, int* packetSize)
    {
        int dCodecFrameSize = 0;
        switch(codec) {
             case RTPCODEC_PCMU:     // PCMU
             case RTPCODEC_PCMA:     // PCMA
                 dCodecFrameSize = ((float)*packetSize) / 1000.0F;
                 dCodecFrameSize = 30;
                 break;
             case RTPCODEC_G723:     // G.723
                 dCodecFrameSize = 30;
                 break;

             case RTPCODEC_G729:    // G.729
                 dCodecFrameSize = 10;
                 break;

             default:
                 dCodecFrameSize = 20;
                 break;
        }
        return dCodecFrameSize;
    }
    int RTPCodec2Frequency(const RTPCodec codec)
    {
        int freq = 0;
        switch(codec) {
            case RTPCODEC_PCMU:
            case RTPCODEC_1016:
            case RTPCODEC_G721:
            case RTPCODEC_GSM:
            case RTPCODEC_G723:
            case RTPCODEC_DVI4_8000:
            case RTPCODEC_LPC:
            case RTPCODEC_PCMA:
            case RTPCODEC_G722:
            case RTPCODEC_QCELP:
            case RTPCODEC_CN:
            case RTPCODEC_G728:
            case RTPCODEC_G729:
                freq = 8000;
                break;
            case RTPCODEC_CELLB:
            case RTPCODEC_JPEG:
            case RTPCODEC_NV:
            case RTPCODEC_H261:
            case RTPCODEC_MPV:
            case RTPCODEC_MP2T:
            case RTPCODEC_H263:
            case RTPCODEC_MPA:
                freq = 90000;
                break;
            case RTPCODEC_DVI4_11025:
                freq = 11024;
                break;
            case RTPCODEC_DVI4_16000:
                freq = 16000;
                break;
            case RTPCODEC_DVI4_22050:
                freq = 22050;
                break;
            case RTPCODEC_L16_1CH:
            case RTPCODEC_L16_2CH:
                freq = 44100;
                break;
            default:
                freq = 8000;
                break;
        }

        return freq;
    }
}  // namespace mqa
