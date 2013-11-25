#include "mqa/RtpTypes.h"
#include "mqa/MqaAssert.h"

#define ASSERT ASSERTMSG0
#define TRACE dprintf

namespace mqa {

    RTPPayloadCodecInfo c_RTPPayloadCodecs[] =
    {
        //  Codec                 Codec Name                  Clock Rate
        {RTPCODEC_PCMU,          "PCM mu-law",                   8000},
        {RTPCODEC_1016,          "CelP 1016",                    8000},
        {RTPCODEC_G721,          "G.721",                        8000},
        {RTPCODEC_GSM,           "GSM",                          8000},
        {RTPCODEC_G723,          "G.723",                        8000},
        {RTPCODEC_DVI4_8000,     "DVI4-8khz",                    8000},
        {RTPCODEC_DVI4_16000,    "DVI4-16khz",                  16000},
        {RTPCODEC_LPC,           "LPC",                          8000},
        {RTPCODEC_PCMA,          "PCM a-law",                    8000},
        {RTPCODEC_G722,          "G.722",                       16000},
        {RTPCODEC_L16_2CH,       "L16-2 channel",               44100},
        {RTPCODEC_L16_1CH,       "L16-1 channel",               44100},
        {RTPCODEC_QCELP,         "QcelP",                        8000},
        {RTPCODEC_CN,            "CN",                           8000},
        {RTPCODEC_MPA,           "MPA",                         90000},
        {RTPCODEC_G728,          "G.728",                        8000},
        {RTPCODEC_DVI4_11025,    "DVI4-11.025khz",              11025},
        {RTPCODEC_DVI4_22050,    "DVI4-22.050khz",              22050},
        {RTPCODEC_G729,          "G.729",                        8000},
        {RTPCODEC_CELLB,         "CelB",                        90000},
        {RTPCODEC_JPEG,          "JPEG",                        90000},
        {RTPCODEC_NV,            "NV",                          90000},
        {RTPCODEC_H261,          "H.261",                       90000},
        {RTPCODEC_MPV,           "MPV",                         90000},
        {RTPCODEC_MP2T,          "MP2T",                        90000},
        {RTPCODEC_H263,          "H.263",                       90000},
        {RTPCODEC_AMRNB,         "AMR-NB",                       8000},
        {RTPCODEC_AMRWB,         "AMR-WB",                      16000},
        {RTPCODEC_EFR,           "GSM-EFR",                      8000},
        {RTPCODEC_EVRC,          "EVRC",                         8000},
        {RTPCODEC_OTHERAUDIO,    "Other Audio",                  8000},
        {RTPCODEC_OTHERVIDEO,    "Other Video",                 90000},
        {RTPCODEC_OTHERAV,       "Other AV",                    90000},
        {RTPCODEC_RTAUDIO_NB,    "RTAudio-NB",                   8000},
        {RTPCODEC_RTAUDIO_WB,    "RTAudio-WB",                  16000}
    };
    inline int GetAvailableCodecCount(){
        return sizeof(c_RTPPayloadCodecs) / sizeof(RTPPayloadCodecInfo);
    }
    MQA_API RTPPayloadCodecInfo *GetCodecInfo(RTPCodec codec)
    {
        int N = GetAvailableCodecCount();
        for(int i =0;i <N; ++i) {
            if( codec == c_RTPPayloadCodecs[i].eCodec ) {
                return &c_RTPPayloadCodecs[i];
            }
        }
        return NULL;

    }

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
        case RTPCODEC_AMRNB:       // AMR-NB
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
        case RTPCODEC_RTAUDIO_NB:
        case RTPCODEC_RTAUDIO_WB:
            streamType = RTPTYPE_AUDIO;
            mediaType  = RTPMEDIA_AUDIO;
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

            case RTPCODEC_RTAUDIO_NB:
            case RTPCODEC_RTAUDIO_WB:
                dCodecFrameSize = 10;
                break;
             default:
                 dCodecFrameSize = 20;
                 break;
        }
        return dCodecFrameSize;
    }
    UINT32 RTPCodec2ClockRate(const RTPCodec codec)
    {
        int N = GetAvailableCodecCount();
        for(int i =0;i <N; ++i) {
            if( codec == c_RTPPayloadCodecs[i].eCodec ) {
                return c_RTPPayloadCodecs->nClockRate;
            }
        }
        return 8000; // default
    }
    /*
    void RTPCalculateExpectedBitRate(RTPCodec codecType, UINT32 nPayloadSize, UINT8& nSampleSize, UINT32& nCodecBitRate, UINT32& nCodecPacketSize)
    {
        // framesize = timestamp_delta/frequency
        // bitrate = payloadSize*8/framesize;
        //
        // codecPackeSize = framesize = f * payloadsize
        //
        switch (codecType)
        {
        case RTPCODEC_PCMU:   // PCMU
        case RTPCODEC_PCMA:   // PCMA
            {
                // Each G.711 audio sample is 80 bytes
                //if ((nPayloadSize % 80) != 0)
                //{
                //   ASSERT(false);
                //   return;
                //}

                nSampleSize      = 80;
                nCodecBitRate    = 64000;
                nCodecPacketSize = 125 * nPayloadSize;
                break;
            }

        case RTPCODEC_GSM:   // GSM
            {
                // Each GSM audio sample is 33 bytes
                if ((nPayloadSize % 33) != 0)
                {
                    ASSERT(false);
                    return;
                }

                nSampleSize      = 33;
                nCodecBitRate    = 13200;
                nCodecPacketSize = 606 * nPayloadSize;
                break;
            }

        case RTPCODEC_G723:   // G.723
            {
                // Each G.723 audio sample is 20 or 24 bytes depending on bit rate
                if (((nPayloadSize % 20) != 0) && ((nPayloadSize % 24) != 0))
                {
                    ASSERT(false);
                    return;
                }

                if ((nPayloadSize % 20) == 0)
                {
                    nSampleSize      = 20;
                    nCodecBitRate    = 5333;
                    nCodecPacketSize = 1500 * nPayloadSize;
                }
                else
                {
                    nSampleSize      = 24;
                    nCodecBitRate    = 6400;
                    nCodecPacketSize = 1250 * nPayloadSize;
                }

                break;
            }

        case RTPCODEC_DVI4_8000:   // DVI4-8khz
            {
                // Each DVI4-8khz audio sample is 80 bytes
                if ((nPayloadSize % 80) != 0)
                {
                    ASSERT(false);
                    return;
                }

                nSampleSize      = 80;
                nCodecBitRate    = 32000;
                nCodecPacketSize = 250 * nPayloadSize;
                break;
            }

        case RTPCODEC_DVI4_16000:   // DVI4-16khz
            {
                // Each DVI4-16khz audio sample is 160 bytes
                if ((nPayloadSize % 160) != 0)
                {
                    ASSERT(false);
                    return;
                }

                nSampleSize      = 160;
                nCodecBitRate    = 64000;
                nCodecPacketSize = 125 * nPayloadSize;
                break;
            }

        case RTPCODEC_LPC:   // LPC
            {
                // Each LPC audio sample is 14 bytes
                if ((nPayloadSize % 14) != 0)
                {
                    ASSERT(false);
                    return;
                }

                nSampleSize      = 14;
                nCodecBitRate    = 5600;
                nCodecPacketSize = 1429 * nPayloadSize;
                break;
            }

        case RTPCODEC_G728:  // G.728
            {
                // Each G.728 audio sample is 10 bytes
                if ((nPayloadSize % 10) != 0)
                {
                    ASSERT(false);
                    return;
                }

                nSampleSize      = 10;
                nCodecBitRate    = 16000;
                nCodecPacketSize = 500 * nPayloadSize;
                break;
            }

        case RTPCODEC_G729:  // G.729
            {
                // Each G.729 audio sample is 6 or 10 bytes depending on bit rate
                if (((nPayloadSize % 6) != 0) && ((nPayloadSize % 10) != 0))
                {
                    TRACE("RTP: unexpected G.729 payload size %u!!\n", nPayloadSize);
                    return;
                }

                if ((nPayloadSize % 6) == 0)
                {
                    nSampleSize      = 6;
                    nCodecBitRate    = 6000;
                    nCodecPacketSize = 1667 * nPayloadSize;
                }
                else
                {
                    nSampleSize      = 10;
                    nCodecBitRate    = 8000;
                    nCodecPacketSize = 1000 * nPayloadSize;
                }
                break;
            }
        default:
            break;
        }

        //if (pStream->nSampleSize != 0)
        //{
        //    pStream->nSamplesPerPacket   = (UINT8)(nPayloadSize / pStream->nSampleSize);
        //    pStream->nPacketsPerSecond   = (UINT16)(pStream->nCodecBitRate / (nPayloadSize * 8));
        //    pStream->nExpectedThroughput = pStream->nPacketsPerSecond * nTotalPacketBits;
        //}
    }
    */

}  // namespace mqa
