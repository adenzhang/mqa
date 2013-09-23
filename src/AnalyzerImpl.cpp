
#include "mqainterface.h"
#include "AnalyzerImpl.h"

static void dprintf(const char *s, ...)
{
    va_list arglist;
    char szDebug[256];

    va_start(arglist, s);
    vsprintf(szDebug, s, arglist);
    va_end(arglist);
    OutputDebugStringA(szDebug);
}
static void dprintf(const wchar_t *s, ...)
{
    va_list arglist;
    wchar_t szDebug[256];

    va_start(arglist, s);
    wvsprintf(szDebug, s, arglist);
    va_end(arglist);
    OutputDebugStringW(szDebug);
}
#define MY_ASSERT
#ifdef MY_ASSERT

#define ASSERTMSG0(assertv) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__);  assert(__assert_value__);}\
} while(false)

#define ASSERTMSG1(assertv, msg) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg);  assert(__assert_value__);}\
} while(false)

#define ASSERTMSG2(assertv, msg, m1) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1);  assert(__assert_value__);}\
} while(false)

#define ASSERTMSG3(assertv, msg, m1, m2) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1, m2);  assert(__assert_value__);}\
} while(false)
//--------- assert ret

#define ASSERTRET(assertv) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__);  return;}\
} while(false)

#define ASSERTRET0(assertv, ret) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__);  return ret;}\
} while(false)

#define ASSERTRET1(assertv,ret, msg) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg);  return ret;}\
} while(false)

#define ASSERTRET2(assertv,ret, msg, m1) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1);  return ret;}\
} while(false)

#define ASSERTRET3(assertv, ret, msg, m1, m2) do { \
    bool __assert_value__= (assertv); \
    if(!__assert_value__) {dprintf(__FUNCTION__, msg, m1, m2); return ret;}\
} while(false)

#else  // define MY_ASSERT empty

#define ASSERTMSG0(assertv) do{}while(true)
#define ASSERTMSG1(assertv) do{}while(true)
#define ASSERTMSG2(assertv) do{}while(true)
#define ASSERTMSG3(assertv) do{}while(true)

#define ASSERTRET(assertv) do{}while(true)
#define ASSERTRET0(assertv) do{}while(true)
#define ASSERTRET1(assertv) do{}while(true)
#define ASSERTRET2(assertv) do{}while(true)
#define ASSERTRET3(assertv) do{}while(true)

#endif

namespace mqa {

    //=========================== VQStatsMap ===================================
    VQStatsKeyMap::VQStatsKeyMap()
        : m_ConnEntryTable()
        , m_GRETunnelEntryTable(LAYER_GRE)
        , m_GTPTunnelEntryTable(LAYER_GTP)
    {
    }

    VQStatsKeyMap::~VQStatsKeyMap()
    {
    }

    VQStatsConnSubEntryKey VQStatsKeyMap::CreateConnSubEntryKey(const StatsFrameParser& Parser, bool* pSrcToDest)
    {
        bool bSrcToDest;
        if (!pSrcToDest)
            bSrcToDest = *pSrcToDest;
        else
        {
            StatsIpAddr IpSrc(Parser.IsIpv4(false), Parser.SrcIp(false));
            StatsIpAddr IpDest(Parser.IsIpv4(false), Parser.DestIp(false));
            bSrcToDest = !(IpSrc > IpDest);
        }
        if (bSrcToDest)
            return VQStatsConnSubEntryKey(
            Parser.LowerInfo.TransInfo.nSrcPort,
            Parser.LowerInfo.TransInfo.nDestPort,
            Parser.LowerInfo.TransInfo.nTransProto
            );
        else
            return VQStatsConnSubEntryKey(
            Parser.LowerInfo.TransInfo.nDestPort,
            Parser.LowerInfo.TransInfo.nSrcPort,
            Parser.LowerInfo.TransInfo.nTransProto
            );
    }

    VQStatsConnEntry2Key VQStatsKeyMap::CreateConnEntry2Key(const StatsFrameParser& Parser)
    {
        return VQStatsConnEntry2Key(
            Parser.IsIpv4(false),
            Parser.SrcIp(false),
            Parser.DestIp(false)
            );
    }

    VQStatsConnEntry1Key VQStatsKeyMap::CreateConnEntry1Key(const StatsFrameParser& Parser)
    {
        return VQStatsConnEntry1Key(
            Parser.nVLAN + Parser.nMPLS,
            Parser.aVLANMPLSIds,
            Parser.nLimPort
            );
    }

    VQStatsTunnelSubEntryKey VQStatsKeyMap::CreateTunnelSubEntryKey(const StatsFrameParser& Parser)
    {
        return VQStatsTunnelSubEntryKey(
            Parser.IsIpv4(true),
            Parser.SrcIp(true),
            Parser.DestIp(true),
            Parser.UpperInfo.TransInfo.nSrcPort,
            Parser.UpperInfo.TransInfo.nDestPort,
            Parser.UpperInfo.TransInfo.nTransProto
            );
    }

    VQStatsTunnelEntryKey VQStatsKeyMap::CreateTunnelEntryKey(const StatsFrameParser& Parser)
    {
        return VQStatsTunnelEntryKey(
            Parser.IsIpv4(false),
            Parser.SrcIp(false),
            Parser.DestIp(false),
            Parser.nTEI,
            Parser.nVLAN + Parser.nMPLS,
            Parser.aVLANMPLSIds,
            Parser.nLimPort
            );
    }


    bool VQStatsKeyMap::FindStreamSet(const StatsFrameParser& Parser,
        CMQmonInterface*& pInterface, VQStatsStreamSetPtr& pStreamSet, VQStatsSubEntry*& SubEntry)
    {
        bool ret = true;  // whether stream exists already.
        // If tunnel
        if(Parser.nTunnelType == LAYER_NONE) 
        {
            // Entry Layer
            // Find conn entry1
            VQStatsConnEntry1Key Entry1Key = CreateConnEntry1Key(Parser);
            VQStatsConnEntry1* pConnEntry1 = NULL;
            if (!m_ConnEntryTable.ConnEntry1Map.Lookup(Entry1Key, pConnEntry1))
            {
                ret = false;
                pConnEntry1 = new VQStatsConnEntry1(LAYER_NONE);
                m_ConnEntryTable.ConnEntry1Map.SetAt(Entry1Key, pConnEntry1);
#ifdef VQSTATS_DEBUG
                pConnEntry1->Key = Entry1Key;
#endif
            }
            pInterface = pConnEntry1->pMQmonInterface;

            VQStatsConnEntry2Key Entry2Key = CreateConnEntry2Key(Parser);
            VQStatsConnEntry2* pConnEntry2 = NULL;
            if (!pConnEntry1->ConnEntry2Map.Lookup(Entry2Key, pConnEntry2))
            {
                pConnEntry2 = new VQStatsConnEntry2();
                pConnEntry1->ConnEntry2Map.SetAt(Entry2Key, pConnEntry2);
                ret = false;
            }
            // Check direction
            StatsIpAddr IpSrc(Parser.IsIpv4(false), Parser.SrcIp(false));
            bool bSrcToDest = (IpSrc == Entry2Key.ipSrc);

            // Sub entry layer
            // Find conn sub entry
            VQStatsConnSubEntryKey SubEntryKey = CreateConnSubEntryKey(Parser, &bSrcToDest);
            VQStatsConnSubEntry* pConnSubEntry = NULL;
            if (pConnEntry2->ConnSubEntryMap.Lookup(SubEntryKey, pConnSubEntry))
            {
                UINT8 nIdx = (bSrcToDest ? 0 : 1);
                pStreamSet = pConnSubEntry->BiDirects[nIdx];
            }else{
                SubEntry = pConnSubEntry = new VQStatsConnSubEntry(VQStatsConnKey(Entry2Key, SubEntryKey, Entry1Key));
                pConnEntry2->ConnSubEntryMap.SetAt(SubEntryKey, pConnSubEntry);
                ret = false;
            }
        }else
        {
            // GRE or GTP
            VQStatsTunnelEntryTable* pTunnelEntryTable = (Parser.nTunnelType == LAYER_GTP)
                ? &m_GTPTunnelEntryTable : &m_GRETunnelEntryTable;

            // Entry Layer
            // Find or create tunnel entry
            VQStatsTunnelEntryKey EntryKey = CreateTunnelEntryKey(Parser);
            VQStatsTunnelEntry* pTunnelEntry = NULL;
            if (!pTunnelEntryTable->TunnelEntryMap.Lookup(EntryKey, pTunnelEntry))
            {
                ret = false;
                pTunnelEntry = new VQStatsTunnelEntry(Parser.nTunnelType);
                pTunnelEntryTable->TunnelEntryMap.SetAt(EntryKey, pTunnelEntry);
#ifdef VQSTATS_DEBUG
                pTunnelEntry->Key = EntryKey;
#endif
            }
            pInterface = pTunnelEntry->pMQmonInterface;

            // Sub entry layer
            // Find tunnel sub entry
            VQStatsTunnelSubEntryKey SubEntryKey = CreateTunnelSubEntryKey(Parser);
            VQStatsTunnelSubEntry* pTunnelSubEntry = NULL;
            if (pTunnelEntry->TunnelSubEntryMap.Lookup(SubEntryKey, pTunnelSubEntry))
                pStreamSet = pTunnelSubEntry->BiDirects[0];
            else{
                SubEntry = pTunnelSubEntry = new VQStatsTunnelSubEntry(VQStatsTunnelKey(EntryKey, SubEntryKey));
                pTunnelEntry->TunnelSubEntryMap.SetAt(SubEntryKey, pTunnelSubEntry);
                ret = false;
            }
        }


        return true;
    }

    void VQStatsKeyMap::PrintStats(std::ostream& os, const string& sPrefix, bool bDetailedHeaders)
    {
        m_GRETunnelEntryTable.PrintStats(os, sPrefix, bDetailedHeaders);
        m_GTPTunnelEntryTable.PrintStats(os, sPrefix, bDetailedHeaders);
        m_ConnEntryTable.PrintStats(os, sPrefix, bDetailedHeaders);
    }

//////////////////////////////////  AnalyzerImpl ////////////////////////////////////////////////////


    static bool VQStatsNotifyHandler(CMQmonInterface* pInterface, CMQmonStream* pStream, MQmonNotifyType nType, MQmonNotifyInfo* pInfo)
    {
        if (!pInterface || !pStream)
        {
            dprintf(__FUNCTION__" error: interface or stream argument is null.\n");
            return false;
        }

        switch (nType)
        {
        case MQMON_NOTIFY_ACTIVATING:
            {
                if (!pInfo)
                {
                    dprintf(__FUNCTION__" error: info argument is null.\n");
                    return false;
                }
                // Find and create stream set
                VQStatsStreamSetPtr pStreamSet;
                VQStatsInterface* pStatsInterface = (VQStatsInterface*)pInterface->m_pUserdata;
                if (pStatsInterface->nType == LAYER_NONE) // conn
                {
                    VQStatsConnEntry1* pEntry1 = (VQStatsConnEntry1*)pInterface->m_pUserdata;
                    // Entry Layer
                    // Find or create conn entry2
                    VQStatsConnEntry2Key Entry2Key(pInfo->activating.bIpv4, pInfo->activating.nIpSrc, pInfo->activating.nIpDst);
                    VQStatsConnEntry2* pConnEntry2 = NULL;
                    if (!pEntry1->ConnEntry2Map.Lookup(Entry2Key, pConnEntry2))
                    {
                        pConnEntry2 = new VQStatsConnEntry2();
                        pEntry1->ConnEntry2Map.SetAt(Entry2Key, pConnEntry2);
#ifdef VQSTATS_DEBUG
                        pConnEntry2->Key = Entry2Key;
#endif
                    }

                    // Check direction
                    StatsIpAddr IpSrc(pInfo->activating.bIpv4, pInfo->activating.nIpSrc);
                    bool bSrcToDest = (IpSrc == Entry2Key.ipSrc);
                    const UINT16& nSrcPort = (bSrcToDest ? pInfo->activating.nSrcPort : pInfo->activating.nDstPort);
                    const UINT16& nDstPort = (bSrcToDest ? pInfo->activating.nDstPort : pInfo->activating.nSrcPort);

                    // Sub entry layer
                    // Find or create tunnel sub entry
                    VQStatsConnSubEntryKey SubEntryKey(nSrcPort, nDstPort, pInfo->activating.nTransProto);
                    VQStatsConnSubEntry* pConnSubEntry = NULL;
                    UINT8 nIdx = (bSrcToDest ? 0 : 1);
                    if (pConnEntry2->ConnSubEntryMap.Lookup(SubEntryKey, pConnSubEntry))
                    {
                        // create a new flow
                        //pConnSubEntry = new VQStatsConnSubEntry();
                        //pConnEntry2->ConnSubEntryMap.SetAt(SubEntryKey, pConnSubEntry);
                        ASSERTMSG1(pConnSubEntry->BiDirects[nIdx], "Stream exists already!");
                        pConnSubEntry->BiDirects[nIdx].reset(new VQStatsStreamSet);
#ifdef VQSTATS_DEBUG
                        pConnSubEntry->Key = SubEntryKey;
#endif
                    }else{
                        dprintf("exception! VQStatsConnSubEntry must be created already!");
                    }
                    pStreamSet = pConnSubEntry->BiDirects[nIdx];
                }
                else if (pStatsInterface->nType == LAYER_GRE || pStatsInterface->nType == LAYER_GTP) // tunnel
                {
                    VQStatsTunnelEntry* pTunnelEntry = (VQStatsTunnelEntry*)pInterface->m_pUserdata;
                    // Sub entry layer
                    // Find or create tunnel sub entry
                    VQStatsTunnelSubEntryKey SubEntryKey(pInfo->activating.bIpv4, pInfo->activating.nIpSrc, pInfo->activating.nIpDst,
                        pInfo->activating.nSrcPort, pInfo->activating.nDstPort, pInfo->activating.nTransProto);
                    VQStatsTunnelSubEntry* pTunnelSubEntry = NULL;
                    if (pTunnelEntry->TunnelSubEntryMap.Lookup(SubEntryKey, pTunnelSubEntry))
                    {
                        ASSERTMSG1(pTunnelSubEntry->BiDirects[0], "Tunneled Stream exists already!");
                        pTunnelSubEntry->BiDirects[0].reset(new VQStatsStreamSet);
#ifdef VQSTATS_DEBUG
                        //pTunnelSubEntry->Key = SubEntryKey;
#endif
                    }else{
                        dprintf("exception! VQStatsTunnelSubEntry must be created already!");
                    }
                    pStreamSet = pTunnelSubEntry->BiDirects[0];
                }
                else
                {
                    dprintf(__FUNCTION__" error: invalid interface.\n");
                    return false;
                }
                if (!pStreamSet)
                {
                    dprintf(__FUNCTION__" error: cannot find stream set.\n");
                    return false;
                }

                // Set stream
                VQStatsStreamPtr pStatsStream = pStreamSet->GetStream(pInfo->nType);
                if (!pStatsStream)
                {
                    dprintf(__FUNCTION__" error: can not find stream.\n");
                    return false;
                }
                pStatsStream->bActivate = true;
                pStatsStream->pMQmonStream = pStream;
                pStream->m_pUserdata = pStatsStream.get();
                break;
            }
        case MQMON_NOTIFY_DEACTIVATING:
            {
                VQStatsStream* pStatsStream = (VQStatsStream*)pStream->m_pUserdata;
                if (!pStatsStream)
                {
                    dprintf(__FUNCTION__" error: cannot find stream to deactivate.\n");
                    return false;
                }
                pStatsStream->bActivate = false;
                pStatsStream->pMQmonStream = NULL; // will be auto deleted in MQmon.
                break;
            }
        default:
            {
                dprintf(__FUNCTION__" error: invalid notify type.\n");
                return false;
            }
        }
        return true;
    }
    static void SetMQmonFrameInfo(const timeval& timestamp, const StatsFrameParser& Parser, CMQmonFrameInfo& Info)
    {
        Info.nTimestamp = timestamp;
        bool bUseUpper = (Parser.nTunnelType != LAYER_NONE);
        Info.bIpv4 = Parser.IsIpv4(bUseUpper);
        Info.nIpAddrOffset = Parser.IpAddrOffset(bUseUpper);
        if (Info.bIpv4)
        {
            Info.nTOS = *(Parser.IpPtr(bUseUpper) + 1);
            Info.nTTL = *(Parser.IpPtr(bUseUpper) + 8);
        }
        else
        {
            const UINT8* pIpPtr = Parser.IpPtr(bUseUpper);
            Info.nTOS = ((*pIpPtr) << 4) || ((*(pIpPtr + 1)) >> 4);
            Info.nTTL = *(pIpPtr + 7);
        }
        const StatsTransInfo& TransInfo = (bUseUpper ? Parser.UpperInfo.TransInfo : Parser.LowerInfo.TransInfo);
        Info.nTransType = TransInfo.nTransProto;
        Info.nTransOffset = TransInfo.nTransOffset;
        Info.nTransLength = TransInfo.nTransLength;
    }
    static LONG g_nInstances = 0;
    static void InitVQStats(LOGLEVEL_TYPE loglevel)
    {
        if (InterlockedIncrement(&g_nInstances) == 1)
            //MQmonInit(VQStatsNotifyHandler);
            MQmon::Instance()->Init(VQStatsNotifyHandler, MQMON_NOTIFY_ALL, loglevel);
    }
    static void FiniVQStats(void)
    {
        if (InterlockedDecrement(&g_nInstances) == 0)
            //MQmonFini();
            MQmon::Instance()->Destroy();
    }

    //--------- configure instance -----------

    bool AnalyzerImpl::setLogLevel(LOGLEVEL_TYPE loglevel)
    {
        _logLevel = loglevel;
        return true;
    }

    // Analyzer users should implement EventHanlder and set to Analyzer.
    // when some event happens, the handler will be called to notify users.
    void AnalyzerImpl::setStreamEventhandler(StreamEventHandler *handler)
    {
        _streamHandler = handler;
    }

    // Result handler can be set with an time interval
    // users can passively receive results
    void AnalyzerImpl::setResultEventHandler(ResultEventHandler *handler, unsigned int millisec)
    {
        _resultHandler = handler;
        _resultInterval = millisec;
    }

    //--------- start to work -----------

    bool AnalyzerImpl::start()
    {
        InitVQStats(_logLevel);
        return true;
    }

    bool AnalyzerImpl::feedPacket(char *ethernetHeader, size_t len, unsigned int timesec, unsigned int timeusec, int limID)
    {
        bool                bMultiThread = false;
        StatsFrameParser Parser; 
        if( !Parser.ParseFrame((UINT8*)ethernetHeader, len, limID) )
            return false;

        // Find interface and stream
        CMQmonInterface*    pInterface = NULL;
        VQStatsStreamSetPtr pStreamSet;
        bool                oldFlow;
        VQStatsSubEntry*    SubEntry;
        timeval             timestamp;
        timestamp.tv_sec = timesec;
        timestamp.tv_usec = timesec;
        oldFlow = _vqStatsMap.FindStreamSet(Parser, pInterface, pStreamSet, SubEntry);
        if(!oldFlow)  // new flow
            ((VQStatsInterface*)pInterface->m_pUserdata)->userData = this;

        // Set ip header value, and feed
        CMQmonFrameInfo Info;
        SetMQmonFrameInfo(timestamp, Parser, Info);

        bool   succ = false;
        // Indicate stream
        bool bIndicated = false;
        if (pStreamSet)
        {
            // If have streams, indicate to every streams
            for (int i = 0; i < 3; ++i)
            {
                CMQmonStream* pStream = pStreamSet->Streams[i]->pMQmonStream;
                if (pStream)
                {
                    bIndicated = true;
#ifdef VQSTATS_MULTITHREAD
                    if(bMultiThread)
                        succ = m_TaskEngine->sendTask(pStream, Info, Parser.nDataLength, (void*)Parser.pData, &SubEntry->threadIdx, !oldFlow);
                    else 
#endif
                        succ = !pStream->IndicatePacket(Parser.pData, Parser.nDataLength, Info);
                    if(!succ)
                    {
                        // dprintf("Stream IndicatePacket error.\n");
                        return false;
                    }
                    if(bIndicated)  // @az only indicate to one stream
                        break;
                }
            }
        }
        if (!bIndicated)
        {
            // No stream yet, indicate to interface
#ifdef VQSTATS_MULTITHREAD
            if(bMultiThread)
                succ = m_TaskEngine->sendTask(pInterface, Info, Parser.nDataLength, (void*)Parser.pData, &SubEntry->threadIdx, !oldFlow);
            else 
#endif
                succ = pInterface->IndicatePacket(Parser.pData, Parser.nDataLength, Info);
            if(!succ)
            {
                //dprintf("Interface IndicatePacket error.\n");
                return false;
            }
        }
        return true;
    }

    // Or users may actively retrieve results
    bool AnalyzerImpl::retrieveResults(StreamResultSet& results)
    {
        // todo
        return true;
    }
    bool AnalyzerImpl::stopFlow(FlowKeyPtr& flow)
    {
        return true;
    }

    void AnalyzerImpl::stop()
    {
        // todo
    }

    // destroy the object
    void AnalyzerImpl::release()
    {
        // todo
    }

    //--------- Instance attributes -----------

    bool AnalyzerImpl::isSingleton()
    {
        // todo
        return false;
    }



    Analyzer *Mqa_CreateAnalyzer(ENGINE_TYPE engine)
    {
        if( engine == ENGINE_TELCHM ) {
            return new AnalyzerImpl();
        }
        // todo
        return NULL;
    }

} // namespace mqa
