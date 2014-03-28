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
            mon->priv->streams.remove((CMQmonStream* )this);
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
        virtual bool IndicateRtcpPacket(const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec)
        {
            return rtpStream->IndicateRtcpPacket(timenano(timeSec, timeNSec), RtcpPacketParser((const char*)pPkt, pPktLength));
        }
        virtual bool IsValidStream()
        {
            return rtpStream->IsValidStream();
        }
        virtual bool DetectStream(){
            bool ret = rtpStream->DetectStream();
            if(ret) {
                m_codecType = rtpStream->GetCodecType();
                m_StreamType = (MQmonStreamType)rtpStream->GetMediaType();
            }
            return ret;
        }
        virtual void Reset() {
            rtpStream->reset(mon->priv->nDetectPackets, mon->priv->nNoisePackets);
        }
        virtual bool GetMetrics(CMQmonMetrics& Metrics)
        {
            if(!rtpStream->IsValidStream()) return false;
            ftl::timenano jitter = rtpStream->CalculateJitter();
            UINT32 nPackets;
            Metrics.Jitter = ftl::timemilli(jitter).as<float>();
            Metrics.fLossRate = rtpStream->CalculatePacketLossRate(nPackets);
            Metrics.nPackets = nPackets;
            ftl::timenano delay ;
            //rtpStream->CalculateOneWayDelay(delay);
            if( !rtpStream->CalculateOneWayDelay(delay) )
                return false;
            Metrics.nDelay = ftl::timemilli(delay).as<INT32>(); 
            float rfactor, mos;
            if(! rtpStream->CalculateMOS(mos, rfactor, delay, jitter, Metrics.fLossRate))
                return false;
            Metrics.RFactor = rfactor;
            Metrics.MOS = mos;
            return true;
        }

        virtual bool SetCodecType(INT16 codec)  {
            rtpStream->SetCodecType(codec);
            m_codecType = codec;
            m_StreamType = (MQmonStreamType)rtpStream->GetMediaType();
            return true;
        }
        virtual void SetUserOnewayDelay(UINT32 milli) {
            rtpStream->SetUserOnewayDelay(milli);
        }
    };
    //=================== CMQmonInterfaceDt =============================================
    class CMQmonInterfaceDt
        : public CMQmonInterface
    {
    public:
        typedef boost::unordered_map<uintptr_t, CMQmonStreamDt*> StreamMap;
        typedef StreamMap::iterator                              StreamMapIter;
        boost::unordered_map<uintptr_t, CMQmonStreamDt*>         streamMap;

        CMQmonInterfaceDt(MQmon* mon)
            : CMQmonInterface(mon)
        {}
        virtual ~CMQmonInterfaceDt() {
            clearFlows();
        }

        virtual bool IndicatePacket(const UINT8* pPkt, UINT16 pPktLength, const CMQmonFrameInfo& PktInfo){
            assert(0);
            return false;
        }
        bool destroyFlow(StreamMapIter it) {
            if( it != streamMap.end() ) {
                it->second->removeFromContainer();
                CMQmonStreamDt *p = (CMQmonStreamDt*)it->second;
                delete p;
                streamMap.erase(it);
                return true;
            }
            return false;
        }
        StreamMapIter FindStreamIter(uintptr_t flowId) {
            return streamMap.find(flowId);
        }
        StreamMapIter FindStreamIter(CMQmonStream *strm) {
            for(StreamMapIter it= streamMap.begin(); it!= streamMap.end(); ++it) {
                if( it->second == strm ) 
                    return it;
            }
            return streamMap.end();
        }
        CMQmonStream *FindStream(uintptr_t flowId) {
            StreamMapIter it = streamMap.find(flowId);
            return it == streamMap.end()?NULL:it->second;
        }
        virtual bool removeFlow(uintptr_t flowId) {
            return destroyFlow(FindStreamIter(flowId));
        }
        virtual bool removeFlow(CMQmonStream *strm){
            return destroyFlow(FindStreamIter(strm));
        }
        virtual void clearFlows() {
            while( !streamMap.empty() ) {
                destroyFlow(streamMap.begin());;
            }
        }

         MQmonNotifyType IndicateRtpPacket(uintptr_t flowId, const UINT8* pPkt, UINT16 pPktLength, UINT32 timeSec, UINT32 timeNSec, CMQmonStream** ppStream)
        {
            CMQmonStreamDt *pStream=NULL;
            StreamMapIter   it;
            if( streamMap.end() == (it=streamMap.find(flowId)) ){
                pStream = (CMQmonStreamDt *)m_pMon->CreateStream(this);
                streamMap[flowId] = pStream;
            }else{
                pStream = it->second;
            }
            if(!pStream->IndicateRtpPacket(pPkt, pPktLength, timeSec, timeNSec)) 
                return MQMON_NOTIFY_NORTP;
            if(!pStream->IsValidStream()) {
                if( pStream->DetectStream() ) {
                    if(m_pMon->priv->handler) {
                        MQmonNotifyInfo info;
                        info.bFlowId = true;
                        info.flowId = flowId;
                        m_pMon->priv->handler(this, pStream, MQMON_NOTIFY_ACTIVATING, &info);
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
        if(priv)
            delete priv;
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
        for(InterfaceList::iterator it=priv->interfaces.begin(); it!= priv->interfaces.end(); ++it) {
            delete *it;
        }
        for(StreamList::iterator it=priv->streams.begin(); it!= priv->streams.end(); ++it) {
            delete *it;
        }
        priv->streams.clear();
        priv->interfaces.clear();

        //delete priv;
        //delete this;
    }

    CMQmonInterface* MQmon::CreateInterface()
    {
        CMQmonInterface* p = new CMQmonInterfaceDt(this);
        priv->interfaces.push_back(p);
        return p;
    }
    void MQmon::DestroyInterface(CMQmonInterface* intf) {
        priv->interfaces.remove(intf);
        delete intf;
    }

    CMQmonStream* MQmon::CreateStream(CMQmonInterface *intf)
    {
        CMQmonStreamDt* p = new CMQmonStreamDt(this, intf);
        p->putToContainer();
        return (CMQmonStream*)p;
    }
    void MQmon::DestroyStream(CMQmonStream* strm) {
        for(InterfaceList::iterator it=priv->interfaces.begin(); it!= priv->interfaces.end(); ++it) {
            if( (*it)->removeFlow(strm) )
                return;
        }
        delete strm;
    }

    void MQmon::SetRtpNoisePacketsNum(int num) {
        priv->nNoisePackets = num;
    }
    void MQmon::SetRtpDetectionPacketsNum(int num) {
        priv->nDetectPackets = num;
    }

}  // namespace mqa
