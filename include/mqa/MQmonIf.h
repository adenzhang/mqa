#ifndef MQMONPROXY_H_
#define MQMONPROXY_H_


#ifdef WIN32
#include <WinSock2.h>
#define MQMON_DLLEXPORT __declspec (dllexport)
#else  // linux
#define MQMON_DLLEXPORT
#include <sys/time.h>
#include "linuxbasetsd.h"
#include "stdint.h"
#endif

namespace mqa {
    typedef struct timeval MQmonTimestamp;
    //struct MQmonTimestamp{
    //    long tv_sec;
    //    long tv_usec;
    //} ;
    typedef float MQmonMOS;

    typedef enum
    {
        MQMON_PROTO_UNKNOW = 0,
        MQMON_PROTO_TCP = 0x06,
        MQMON_PROTO_UDP = 0x11
    } MQmonProtocol;

    typedef enum
    {
        MQMON_LOG_NONE = 0,
        MQMON_LOG_ERROR,
        MQMON_LOG_INFO,
        MQMON_LOG_DEBUG
    } MQmonLogLevel;

    typedef enum
    {
        MQMON_TELCHEMY,
        MQMON_DRIVETEST,
        MQMON_UNKNOWN
    } MQmonProvider;

    typedef enum
    {
        MQMON_STREAM_UNKOWN     = 0,
        MQMON_STREAM_AUDIO      = 1 << 1,
        MQMON_STREAM_VIDEO      = 1 << 2,
        MQMON_STREAM_VOICE      = 1 << 3
    } MQmonStreamType;

    typedef enum
    {
        MQMON_NOTIFY_RTP              = 0,
        MQMON_NOTIFY_ACTIVATING       = 1,
        MQMON_NOTIFY_DEACTIVATING     = 1 << 1,
        MQMON_NOTIFY_NORTP            = 1 << 2,
        MQMON_NOTIFY_ALL              = MQMON_NOTIFY_ACTIVATING | MQMON_NOTIFY_DEACTIVATING
    } MQmonNotifyType;

    typedef struct _MQmonNotifyInfo_S
    {
        MQmonStreamType nType;
        union
        {
            struct
            {
                bool bIpv4;
                UINT8 nTransProto;
                UINT64 nIpSrc[2];
                UINT64 nIpDst[2];
                UINT16 nSrcPort;
                UINT16 nDstPort;
            } activating;
            uintptr_t flowId;
        };
        bool bFlowId;
    } MQmonNotifyInfo;

    class MQMON_DLLEXPORT CMQmonFrameInfo
    {
    public:
        MQmonTimestamp nTimestamp;
        bool bIpv4;
        UINT16 nIpAddrOffset;
        UINT8 nTOS;
        UINT8 nTTL;
        UINT8 nTransType;
        UINT16 nTransOffset;
        UINT16 nTransLength;
    };

    class MQMON_DLLEXPORT CMQmonMetrics
    {
    public:
        CMQmonMetrics()
            : MOS(0)
            , Jitter(0)
            , RFactor(0)
            , fLossRate(0)
            , nPackets(0)
            , nDelay(0)
        {
        }

        MQmonMOS MOS;
        float    Jitter;
        float    RFactor;
        float    fLossRate;
        UINT32   nPackets;
        INT32    nDelay;
    };

    class MQmon;
    class CMQmonStream;
    class MQMON_DLLEXPORT CMQmonInterface
    {
        friend class MQmon;
    protected:
        virtual ~CMQmonInterface() {}
        CMQmonInterface(MQmon* mon=NULL) : m_pMon(mon), m_pUserdata(NULL) {}
    public:
        virtual bool IndicatePacket(const UINT8* pPkt, UINT16 pPktLength, const CMQmonFrameInfo& PktInfo) = 0;

        // remove and destroy stream
        // return true if successful; false when no flow has been found.
        virtual bool removeFlow(uintptr_t flowId){return false;}

        // out new stream or NULL
        virtual MQmonNotifyType IndicateRtpPacket(uintptr_t flowId, const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec, CMQmonStream**){return MQMON_NOTIFY_RTP;}

        virtual bool GetMetrics(CMQmonMetrics& Metrics) = 0;

        virtual CMQmonStream *FindStream(uintptr_t flowId) {return NULL;}

        // remove and destroy stream
        virtual bool removeFlow(CMQmonStream *strm){return false;}

        virtual void clearFlows(){}

        MQmon* m_pMon;
        void*  m_pUserdata;
    };

    class MQMON_DLLEXPORT CMQmonStream
    {
    protected:
        friend class MQmon;
        friend class CMQmonInterface;
        virtual ~CMQmonStream()
        {}
        CMQmonStream(CMQmonInterface* Interface=NULL)
            : m_Interface(Interface)
            , m_pUserdata(NULL)
        {}
    public:

        virtual bool IndicatePacket(const UINT8* pPkt, UINT16 pPktLength, const CMQmonFrameInfo& PktInfo) = 0;
        virtual bool IndicateRtpPacket(const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec){return false;}
        virtual bool IndicateRtcpPacket(const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec){return false;}
        virtual bool IsValidStream(){return false;}
        virtual bool DetectStream(){return false;}
        virtual bool GetMetrics(CMQmonMetrics& Metrics) = 0;

        // Reset internal objects and stream will become invalid.
        // Users should call DetectStream to detect stream.
        virtual void Reset() { return ;}

        virtual bool SetCodecType(INT16 codec) {
            m_codecType = codec;
            return true;
        }
        CMQmonInterface* m_Interface;
        MQmonStreamType  m_StreamType;
        INT16            m_codecType;
        void*            m_pUserdata;
    };

    typedef bool (*MQmonNotifyHandler)(CMQmonInterface* pInterface, CMQmonStream* pStream, MQmonNotifyType nType, MQmonNotifyInfo* pInfo);

    enum RtpDetectionParam {kDefaultDetectPackets = 4, kDefaultNoisePackets = 1};

    class MQmonPriv;
    class MQMON_DLLEXPORT MQmon 
    {
        MQmon();
        ~MQmon();
        MQmon(const MQmon&){}
        void operator=(const MQmon&){}
    public:

        MQmonPriv *priv;
        static MQmon* Instance();

        virtual bool Init(MQmonNotifyHandler pNotifyHandler = NULL,
            UINT8 nNotifyMask = MQMON_NOTIFY_ALL,
            int nLogLevel = MQMON_LOG_ERROR,
            MQmonProvider nProvider = MQMON_TELCHEMY);
        virtual void Destroy();

        virtual CMQmonInterface* CreateInterface();
        // when destroy an interface, all associated streams will be destroyed.
        virtual void DestroyInterface(CMQmonInterface* );
        virtual CMQmonStream* CreateStream(CMQmonInterface *intf);
        virtual void DestroyStream(CMQmonStream* );

        // only given number of consecutive packets has been valid RTP packets, the RTP stream will be detected.
        // if not specified, RtpDetectionParam::kDefaultDetectPackets will be used by default.
        virtual void SetRtpDetectionPacketsNum(int num);

        // when not larger than given number of consecutive packets has been invalid RTP packets, these packets will be regards as noise packets and be discarded.
        // if not specified, RtpDetectionParam::kDefaultNoisePackets will be used by default.
        virtual void SetRtpNoisePacketsNum(int num);

    };

}  // namespace mqa
#endif
