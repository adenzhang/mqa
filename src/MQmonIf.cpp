#include "MQmonIf.h"
#include <list>
#include "RtpStream.h"
#include "timesec.h"
#include "ftlcollections.h"
#include <assert.h>

using namespace ftl;

namespace mqa {
    typedef std::list<CMQmonStream*> StreamList;
    typedef std::list<CMQmonInterface*> InterfaceList;

    struct MQmonPriv
    {
        MQmonNotifyHandler handler;
        UINT8 nNotifyMask;
        int nLogLevel;
        MQmonProvider nProvider;

        StreamList streams;
        InterfaceList interfaces;
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
        }
        ~CMQmonStreamDt(){
            DestroyRtpStream(rtpStream);
        }

        virtual bool IndicatePacket(const UINT8* pPkt, UINT16 pPktLength, const CMQmonFrameInfo& PktInfo)
        {
            assert(0);
            return false;
        }
        virtual bool IndicateRtpPacket(const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec)
        {
            return rtpStream->PacketArrival(timenano(timeSec, timeNSec), RtpPacketParser((const char*)pPkt, pPktLength));
        }
        virtual bool IsValidStream()
        {
            return rtpStream->IsValidStream();
        }
        virtual bool DetectStream(){
            return rtpStream->DetectStream();
        }
        virtual bool GetMetrics(CMQmonMetrics& Metrics)
        {
            if(!rtpStream->IsValidStream()) return false;
            rtpStream->CalculateOneWayDelay();
            Metrics.Jitter = rtpStream->CalculateJitter();
            Metrics.fLossRate = rtpStream->CalculatePacketLossRate();
            float jitter, mos;
            if(! rtpStream->CalculateMOS(mos, jitter))
                return false;
            Metrics.Jitter = jitter;
            Metrics.MOS = mos;
            return true;
        }


    };
    //=================== CMQmonInterfaceDt =============================================
    class CMQmonInterfaceDt
        : public CMQmonInterface
    {
    public:
        MQmon *mon;
        Map<uintptr_t, CMQmonStreamDt*> streamMap;

        CMQmonInterfaceDt(MQmon* mon):mon(mon){}

        virtual bool IndicatePacket(const UINT8* pPkt, UINT16 pPktLength, const CMQmonFrameInfo& PktInfo){
            assert(0);
            return false;
        }
        virtual void removeFlow(uintptr_t flowId)
        {
            streamMap.RemoveKey(flowId);
        }
        bool IndicateRtpPacket(uintptr_t flowId, const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec)
        {
            CMQmonStreamDt *pStream=NULL;
            if(!streamMap.Lookup(flowId, pStream)){
                pStream = (CMQmonStreamDt *)mon->CreateStream();
                streamMap.SetAt(flowId, pStream);
            }
            if(!pStream->IndicateRtpPacket(pPkt, pPktLength, timeSec, timeNSec)) 
                return false;
            if(!pStream->IsValidStream()) {
                if( pStream->DetectStream() ) {
                    if(mon->priv->handler) {
                        MQmonNotifyInfo info;
                        info.bFlowId = true;
                        info.flowId = flowId;
                        mon->priv->handler(this, pStream, MQMON_NOTIFY_ACTIVATING, &info);
                    }
                }
            }
            // detect stream close. as it's time consuming. so usually donot detect it.
            return true;
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
        MQmon  _THIS;
        
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
        CMQmonStream* p = new CMQmonStreamDt(this, intf);
        priv->streams.push_back(p);
        return p;
    }


}  // namespace mqa