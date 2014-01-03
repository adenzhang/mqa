#include "mqa/MQmonIf.h"
#include <list>
#include "mqa/RtpStream.h"
#include "ftl/timesec.h"
#include <assert.h>
#include <boost/unordered_map.hpp>

using namespace ftl;

namespace mqa {
    typedef std::list<CMQmonStream*> StreamList;
    typedef std::list<CMQmonInterface*> InterfaceList;


    struct MQmonPriv
    {
        MQmonNotifyHandler           handler;
        UINT8                        nNotifyMask;
        int                          nLogLevel;
        MQmonProvider                nProvider;

        StreamList                   streams;
        InterfaceList                interfaces;
        int                          nDetectPackets;
        int                          nNoisePackets;

        MQmonPriv()
            : handler(NULL)
            , nNotifyMask(0)
            , nLogLevel(0)
            , nProvider(MQMON_DRIVETEST)
            , nDetectPackets(kDefaultDetectPackets)
            , nNoisePackets(kDefaultNoisePackets)
        {}
    };
    //=================== CMQmonStreamDt =============================================
    class CMQmonStreamDt
        : public CMQmonStream
    {
    public:
        MQmon *mon;
        RtpStream *rtpStream;

        CMQmonStreamDt(MQmon* mon, CMQmonInterface* inter):CMQmonStream(inter), mon(mon), rtpStream(CreateRtpStream())
        {
            rtpStream->reset(mon->priv->nDetectPackets, mon->priv->nNoisePackets);
        }
        ~CMQmonStreamDt(){
            DestroyRtpStream(rtpStream);
        }
        void putToContainer() {
            if(mon)
                mon->priv->streams.push_back(this);
        }
        void removeFromContainer() {
            if(!mon) return;
            // remove from MQmon
            for(StreamList::iterator it = mon->priv->streams.begin(); it != mon->priv->streams.end(); ++it ) {
                if( *it == (CMQmonStream* )this ) {
                    mon->priv->streams.erase(it);
                }
            }
        }

        virtual bool IndicatePacket(const UINT8* pPkt, UINT16 pPktLength, const CMQmonFrameInfo& PktInfo)
        {
            assert(0);
            return false;
        }
        virtual bool IndicateRtpPacket(const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec)
        {
            return rtpStream->IndicateRtpPacket(timenano(timeSec, timeNSec), RtpPacketParser((const char*)pPkt, pPktLength));
        }
        virtual bool IsValidStream()
        {
            return rtpStream->IsValidStream();
        }
        virtual bool DetectStream(){
            bool ret = rtpStream->DetectStream();
            m_codecType = rtpStream->GetCodecType();
            m_StreamType = (MQmonStreamType)rtpStream->GetMediaType();
            return ret;
        }
        virtual void Reset() {
            rtpStream->reset(mon->priv->nDetectPackets, mon->priv->nNoisePackets);
        }
        virtual bool GetMetrics(CMQmonMetrics& Metrics)
        {
            if(!rtpStream->IsValidStream()) return false;
            ftl::timenano delay = rtpStream->CalculateOneWayDelay();
            Metrics.nDelay = delay.as<int>(); 
            Metrics.nDelay /= 1000000; // to milli-sec
            UINT32 nPackets;
            Metrics.Jitter = rtpStream->CalculateJitter().as<float>();
            Metrics.Jitter /= 1000000; // to milli-sec
            Metrics.fLossRate = rtpStream->CalculatePacketLossRate(nPackets);
            Metrics.nPackets = nPackets;
            float rfactor, mos;
            if(! rtpStream->CalculateMOS(mos, rfactor))
                return false;
            Metrics.RFactor = rfactor;
            Metrics.MOS = mos;
            return true;
        }

        virtual bool SetCodecType(INT16 codec)  {
            rtpStream->SetCodecType(codec);
            m_codecType = codec;
            return true;
        }


    };
    //=================== CMQmonInterfaceDt =============================================
    class CMQmonInterfaceDt
        : public CMQmonInterface
    {
    public:
        MQmon *mon;
        typedef boost::unordered_map<uintptr_t, CMQmonStreamDt*> StreamMap;
        typedef StreamMap::iterator                              StreamMapIter;
        boost::unordered_map<uintptr_t, CMQmonStreamDt*>         streamMap;

        CMQmonInterfaceDt(MQmon* mon)
            : mon(mon)
        {}

        virtual bool IndicatePacket(const UINT8* pPkt, UINT16 pPktLength, const CMQmonFrameInfo& PktInfo){
            assert(0);
            return false;
        }
        virtual void removeFlow(uintptr_t flowId)
        {
            StreamMapIter it = streamMap.find(flowId);
            if( it != streamMap.end() ) {
                it->second->removeFromContainer();
                CMQmonStream *p = it->second;
                delete p;
            }
            streamMap.erase(flowId);
        }
         MQmonNotifyType IndicateRtpPacket(uintptr_t flowId, const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec, CMQmonStream** ppStream)
        {
            CMQmonStreamDt *pStream=NULL;
            StreamMapIter   it;
            if( streamMap.end() == (it=streamMap.find(flowId)) ){
                pStream = (CMQmonStreamDt *)mon->CreateStream();
                streamMap[flowId] = pStream;
            }else{
                pStream = it->second;
            }
            if(!pStream->IndicateRtpPacket(pPkt, pPktLength, timeSec, timeNSec)) 
                return MQMON_NOTIFY_NORTP;
            if(!pStream->IsValidStream()) {
                if( pStream->DetectStream() ) {
                    if(mon->priv->handler) {
                        MQmonNotifyInfo info;
                        info.bFlowId = true;
                        info.flowId = flowId;
                        mon->priv->handler(this, pStream, MQMON_NOTIFY_ACTIVATING, &info);
                    }
                    if(ppStream) *ppStream = pStream;
                    return MQMON_NOTIFY_ACTIVATING;
                }
            }
            // detect stream close. as it's time consuming. so usually donot detect it.
            return MQMON_NOTIFY_RTP;
        }

        virtual bool GetMetrics(CMQmonMetrics& Metrics){
            return false;
        }

    };

    //=================== MQmon =============================================
    MQmon::MQmon()
        : priv(new MQmonPriv())
    {
    }
    MQmon::~MQmon()
    {
    }
    MQmon* MQmon::Instance()
    {
        static MQmon  _THIS;
        
        return &_THIS;;
    }

    bool MQmon::Init(MQmonNotifyHandler pNotifyHandler,
        UINT8 nNotifyMask,
        int nLogLevel,
        MQmonProvider nProvider)
    {
        priv->handler = pNotifyHandler;
        priv->nNotifyMask = nNotifyMask;
        priv->nLogLevel = nLogLevel;
        priv->nProvider = nProvider;
        return true;
    }
    void MQmon::Destroy()
    {
        for(StreamList::iterator it=priv->streams.begin(); it!= priv->streams.end(); ++it) {
            delete *it;
        }
        for(InterfaceList::iterator it=priv->interfaces.begin(); it!= priv->interfaces.end(); ++it) {
            delete *it;
        }
        priv->streams.clear();
        priv->interfaces.clear();

        delete priv;
        delete this;
    }

    CMQmonInterface* MQmon::CreateInterface()
    {
        CMQmonInterface* p = new CMQmonInterfaceDt(this);
        priv->interfaces.push_back(p);
        return p;
    }

    CMQmonStream* MQmon::CreateStream(CMQmonInterface *intf)
    {
        CMQmonStreamDt* p = new CMQmonStreamDt(this, intf);
        p->putToContainer();
        return (CMQmonStream*)p;
    }

    void MQmon::SetRtpNoisePacketsNum(int num) {
        priv->nNoisePackets = num;
    }
    void MQmon::SetRtpDetectionPacketsNum(int num) {
        priv->nDetectPackets = num;
    }

}  // namespace mqa
