
#include "mqainterface.h"
#include "AnalyzerImpl.h"

#include "MqaAssert.h"


namespace mqa {

//////////////////////////////////  AnalyzerImpl ////////////////////////////////////////////////////

    bool AnalyzerImpl::VQStatsNotifyHandler(CMQmonInterface* pInterface, CMQmonStream* pStream, MQmonNotifyType nType, MQmonNotifyInfo* pInfo)
    {
        if (!pInterface || !pStream)
        {
            dprintf(__FUNCTION__" error: interface or stream argument is null.\n");
            return false;
        }

        VQStatsInterface* pStatsInterface = (VQStatsInterface*)pInterface->m_pUserdata;
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
                FlowKey            *pFlowKey = pInfo->bFlowId?(FlowKey*)pInfo->flowId:NULL;
                if(pInfo->bFlowId) {
                    if( pStatsInterface->userData ) {
                        AnalyzerImpl    *analyzer = (AnalyzerImpl *)pStatsInterface->userData;
                        VQStatsSubEntry *SubEntry = analyzer->_vqStatsMap.findFlow(pFlowKey);
                        if(SubEntry) {
                            int nIdx = pFlowKey == SubEntry->BiFlowKeys[0]?0:1;
                            SubEntry->BiDirects[nIdx].reset(new VQStatsStreamSet);
                            pStreamSet = SubEntry->BiDirects[nIdx];
                        }else{
                            dprintf(__FUNCTION__" error: Unable to find VQStatsSubEntry.\n");
                            return false;
                        }
                    }else{
                        dprintf(__FUNCTION__" error: invalid AnalyzerImpl Ptr.\n");
                        return false;
                    }
                }else{
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
                        }else{
                            dprintf("exception! VQStatsConnSubEntry must be created already!");
                        }
                        pStreamSet = pConnSubEntry->BiDirects[nIdx];
                        pFlowKey = pConnSubEntry->BiFlowKeys[nIdx];
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
                        }else{
                            dprintf("exception! VQStatsTunnelSubEntry must be created already!");
                        }
                        pStreamSet = pTunnelSubEntry->BiDirects[0];
                        pFlowKey = pTunnelSubEntry->BiFlowKeys[0];
                    }
                    else
                    {
                        dprintf(__FUNCTION__" error: invalid interface.\n");
                        return false;
                    }
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
                pStatsStream->flowKey = pFlowKey;
                pStream->m_pUserdata = pStatsStream.get();

                // notify analyzer's handler
                if( pStatsInterface->userData && pStatsStream->flowKey){
                    AnalyzerImpl *analyzer = (AnalyzerImpl *)pStatsInterface->userData;
                    if( analyzer->_streamHandler ) {
                        analyzer->_streamHandler->handle(StreamEventHandler::EVT_STREAM_ACTIVATING, pStatsStream->flowKey);
                    }
                }

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
                // notify analyzer's handler
                if( pStatsInterface->userData && pStatsStream->flowKey){
                    AnalyzerImpl *analyzer = (AnalyzerImpl *)pStatsInterface->userData;
                    if( analyzer->_streamHandler ) {
                        analyzer->_streamHandler->handle(StreamEventHandler::EVT_STREAM_DEACTIVATING, pStatsStream->flowKey);
                    }
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
    LONG AnalyzerImpl::g_nInstances = 0;
    void AnalyzerImpl::InitVQStats(LOGLEVEL_TYPE loglevel)
    {
        if (InterlockedIncrement(&g_nInstances) == 1)
            //MQmonInit(VQStatsNotifyHandler);
            MQmon::Instance()->Init(AnalyzerImpl::VQStatsNotifyHandler, MQMON_NOTIFY_ALL, loglevel);
    }
    void AnalyzerImpl::FiniVQStats(void)
    {
        if (InterlockedDecrement(&g_nInstances) == 0)
            //MQmonFini();
            MQmon::Instance()->Destroy();
    }

    AnalyzerImpl::AnalyzerImpl(ENGINE_TYPE engine)
        : _streamHandler(NULL), _resultHandler(NULL), _outputHandler(NULL), _engineType(engine)
    {}

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
    void AnalyzerImpl::setResultEventHandler(ResultEventHandler *handler, timeval& timeout)
    {
        _resultHandler = handler;
        _resultInterval = timeout;
    }
    void AnalyzerImpl::setResultOutputCallback(UINT32 nUserParm, PFN_OutputBlockCallback pOutput, timeval& timeout)
    {
        _outputParam = nUserParm;
        _outputHandler = pOutput;
        _resultInterval = timeout;
        _rtsmDumper.init(&_vqStatsMap, pOutput, nUserParm, ftl::timenano(timeout));
    }
    //--------- start to work -----------

    bool AnalyzerImpl::start()
    {
        InitVQStats(_logLevel);
        return true;
    }
    void AnalyzerImpl::processTime(timeval& timestamp)
    {
        if( _rtsmDumper.processTime(ftl::timenano(timestamp)) ) // timeout to output
        {
            if( _resultHandler ) {
                StreamResultSet results;
                retrieveResults(results);
                _resultHandler->handle(&results);
            }
            if( _outputHandler ) {
                outputResults(timestamp);
            }
        }
    }

    bool AnalyzerImpl::feedPacket(char *ethernetHeader, size_t len, timeval& timestamp, int limID)
    {
        bool                bMultiThread = false;

        // check timeout t0 output
        processTime(timestamp);

        StatsFrameParser    Parser; 
        if( !Parser.ParseFrame((UINT8*)ethernetHeader, len, limID) )
            return false;

        // Find interface and stream
        CMQmonInterface*    pInterface = NULL;
        VQStatsStreamSetPtr pStreamSet;
        bool                oldFlow;
        VQStatsSubEntry*    SubEntry;
        int                 flowIdx;
        oldFlow = _vqStatsMap.FindStreamSet(Parser, pInterface, pStreamSet, SubEntry,flowIdx);
        if(!oldFlow)  // new flow
            ((VQStatsInterface*)pInterface->m_pUserdata)->userData = this;

        uintptr_t flowId=0;
        if(_engineType == ENGINE_DRIVETEST) {
            flowId = (uintptr_t) &SubEntry->BiFlowKeys[flowIdx];
        }

        // Set ip header value, and feed
        CMQmonFrameInfo Info;
        SetMQmonFrameInfo(timestamp, Parser, Info);

        bool   succ = true;
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
                    if(bMultiThread){
                        succ = m_TaskEngine->sendTask(pStream, Info, Parser.nDataLength, (void*)Parser.pData, &SubEntry->threadIdx, !oldFlow);
                    }else 
#endif
                    {
                        if(_engineType == ENGINE_DRIVETEST) {
                            pStream->IndicateRtpPacket(Parser.pData+Parser.nAppLayerOffset, Parser.nDataLength-Parser.nAppLayerOffset, timestamp.tv_sec, timestamp.tv_usec*1000);
                        }else
                        {
                            succ = pStream->IndicatePacket(Parser.pData, Parser.nDataLength, Info);
                        }
                    }
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
            if(bMultiThread){
                succ = m_TaskEngine->sendTask(pInterface, Info, Parser.nDataLength, (void*)Parser.pData, &SubEntry->threadIdx, !oldFlow);
            }else
#endif
            {
                if(_engineType == ENGINE_DRIVETEST) {
                    succ = pInterface->IndicateRtpPacket(flowId, Parser.pData+Parser.nAppLayerOffset, Parser.nDataLength-Parser.nAppLayerOffset, timestamp.tv_sec, timestamp.tv_usec*1000, NULL);
                }else{
                    succ = pInterface->IndicatePacket(Parser.pData, Parser.nDataLength, Info);
                }
            }
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
        results.clear();

        RetrieveResultsVisitor v(results);
        _vqStatsMap.accept(v);
        return true;
    }
    void AnalyzerImpl::outputResults(timeval& timestamp)
    {
        _rtsmDumper.OutputStats();
    }
    bool AnalyzerImpl::stopFlow(FlowKeyPtr& flow)
    {
        // currently unable to stop a flow manually.
        return false;
    }

    void AnalyzerImpl::stop()
    {
        FiniVQStats();
    }

    // destroy the object
    void AnalyzerImpl::release()
    {
        stop();
    }

    //--------- Instance attributes -----------

    bool AnalyzerImpl::isSingleton()
    {
        return false;
    }

    Analyzer *Mqa_CreateAnalyzer(ENGINE_TYPE engine)
    {
        if( engine == ENGINE_TELCHM || engine == ENGINE_DRIVETEST ) {
            return new AnalyzerImpl(engine);
        }
        return NULL;
    }

} // namespace mqa
