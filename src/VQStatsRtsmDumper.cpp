#include "VQStatsRtsmDumper.h"

#include "MqaAssert.h"

namespace mqa {

    void VQStatsRtsmDumper::VQStatsOutputAndResetBlock(UINT8* pRTSMBuffer, UINT32& nOffset)
    {
        RtsdBufFmtOtherHeader *pOtherHeader = (RtsdBufFmtOtherHeader*)(pRTSMBuffer);
        pOtherHeader->length = nOffset - sizeof(RtsdBufFmtOtherHeader);
        if (m_pfnOutputStatsBlock)
            m_pfnOutputStatsBlock(m_uUserParam, pRTSMBuffer, nOffset);

        // No need to modify RTSM Statistics header and VQStats header
        nOffset = sizeof(RtsdBufFmtOtherHeader) + sizeof(RtsdBufFmtVQStatsHeader);
    }

    static void VQStatsDumpEntryHeader(RtsdBufFmtVQStatsEntryHeader& Header, bool bTunnel,
        UINT8 nStatsLayer, UINT8 nStatsDir, bool bIpv4)
    {
        Header.bTunnel = (bTunnel ? 1 : 0);
        Header.uStatsLayer = nStatsLayer;
        Header.uStatsDir = nStatsDir;
        Header.bIPVersion = (bIpv4 ? 0 : 1);
    }

    static void VQStatsDumpStreamSet(RtsdBufFmtVQStatsEntryCount* EntryCount, VQStatsStreamSetPtr StreamSet)
    {
        StreamSet->retrieveResults();
        StreamSet->dump(*EntryCount);
    }

    template <class EntryType>
    static void VQStatsDumpConnEntryTmpl(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsConnEntry1Key& Entry1Key, const VQStatsConnEntry2Key& Entry2Key)
    {
        EntryType* pStatsEntry = (EntryType*)(pRTSMBuffer + nOffset);
        memset(pStatsEntry, 0, sizeof(*pStatsEntry));
        VQStatsDumpEntryHeader(pStatsEntry->header, 0, 0, 0, Entry2Key.ipSrc.IsIpv4());
        pStatsEntry->nLimPortNum = Entry1Key.nLimPort;
        Entry2Key.ipSrc.Dump(pStatsEntry->ipLowerSrc);
        Entry2Key.ipDest.Dump(pStatsEntry->ipLowerDest);
        pStatsEntry->nVLANMPLSIds = min(Entry1Key.nVLANMPLSIds, RTSD_MAX_VLANMPLS_IDS);
        for (int i = 0; i < Entry1Key.nVLANMPLSIds; ++i)
            pStatsEntry->aVLANMPLSIds[i] = Entry1Key.aVLANMPLSIds[i];
        nOffset += FIELD_OFFSET(EntryType, aVLANMPLSIds) + sizeof(pStatsEntry->aVLANMPLSIds[0]) * pStatsEntry->nVLANMPLSIds;
        stringstream ss;
        ss << Entry2Key.ipSrc << " --> " << Entry2Key.ipDest;
        dprintf("DumpConnEntry:LimPort=%d,IP:%s", Entry1Key.nLimPort, ss.str().c_str());
    }

    static void VQStatsDumpConnEntry(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsConnEntry1Key& Entry1Key, const VQStatsConnEntry2Key& Entry2Key)
    {
        if (Entry2Key.ipSrc.IsIpv4())
            return VQStatsDumpConnEntryTmpl<RtsdBufFmtVQStatsIPv4ConnEntry>(pRTSMBuffer, nRTSMLength, nOffset, Entry1Key, Entry2Key);
        else
            return VQStatsDumpConnEntryTmpl<RtsdBufFmtVQStatsIPv6ConnEntry>(pRTSMBuffer, nRTSMLength, nOffset, Entry1Key, Entry2Key);
    }

    void VQStatsDumpConnSubEntry(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsConnSubEntryKey& SubEntryKey, const VQStatsConnSubEntry& SubEntry)
    {
        UINT8 nStatsDir = 0;
        if (SubEntry.BiDirects[0] && SubEntry.BiDirects[0]->HasActivate())
            nStatsDir |= 1;
        if (SubEntry.BiDirects[1] && SubEntry.BiDirects[1]->HasActivate())
            nStatsDir |= 2;
        if (nStatsDir == 0)
            return;

        RtsdBufFmtVQStatsIPv4ConnSubEntry* pStatsEntry = (RtsdBufFmtVQStatsIPv4ConnSubEntry*)(pRTSMBuffer + nOffset);
        memset(pStatsEntry, 0, sizeof(*pStatsEntry));
        VQStatsDumpEntryHeader(pStatsEntry->header, 0, 1, nStatsDir, 0);
        pStatsEntry->ipProtocol = SubEntryKey.uProtocol;
        pStatsEntry->srcPort = SubEntryKey.uSrcPort;
        pStatsEntry->destPort = SubEntryKey.uDestPort;
        nOffset += FIELD_OFFSET(RtsdBufFmtVQStatsIPv4ConnSubEntry, tStatsCount);
        UINT8 *pStatsCount = (UINT8*)pStatsEntry->tStatsCount;
        for (int i = 0; i < 2; ++i) {
            if (SubEntry.BiDirects[i] && SubEntry.BiDirects[i]->HasActivate())
            {
                VQStatsDumpStreamSet((RtsdBufFmtVQStatsEntryCount*)pStatsCount, SubEntry.BiDirects[i]);
                int n = GetLength((RtsdBufFmtVQStatsEntryCount*)pStatsCount);
                nOffset += n;
                pStatsCount += n;
            }
        }
        dprintf("DumpConnSubEntry:Prot=%d,Port:%d-->%d, Dir=%d", SubEntryKey.uProtocol, ntohs(SubEntryKey.uSrcPort), ntohs(SubEntryKey.uDestPort), nStatsDir);
    }

    void VQStatsRtsmDumper::DumpConn(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsConnEntryTable& EntryTable)
    {
        for (ftl::POSITION Pos = EntryTable.ConnEntry1Map.GetStartPosition(); Pos;)
        {
            const VQStatsConnEntry1* pEntry1 = EntryTable.ConnEntry1Map.GetValueAt(Pos);
            const VQStatsConnEntry1Key& Entry1Key = EntryTable.ConnEntry1Map.GetNextKey(Pos);

            for (ftl::POSITION Pos1 = pEntry1->ConnEntry2Map.GetStartPosition(); Pos1;)
            {
                const VQStatsConnEntry2* pEntry2 = pEntry1->ConnEntry2Map.GetValueAt(Pos1);
                const VQStatsConnEntry2Key& Entry2Key = pEntry1->ConnEntry2Map.GetNextKey(Pos1);

                if (pEntry2->ConnSubEntryMap.IsEmpty())
                    continue;

                bool bNeedDumpLayer1 = true;
                for (ftl::POSITION Pos2 = pEntry2->ConnSubEntryMap.GetStartPosition(); Pos2;)
                {
                    const VQStatsConnSubEntry* pSubEntry = pEntry2->ConnSubEntryMap.GetValueAt(Pos2);
                    const VQStatsConnSubEntryKey SubEntryKey = pEntry2->ConnSubEntryMap.GetNextKey(Pos2);

                    if (!pSubEntry || !pSubEntry->HasData())
                        continue;

                    UINT32 nNeedSpace = VQSTATS_CONN_LAYER2_MAX_LENGTH + (bNeedDumpLayer1 ? VQSTATS_CONN_LAYER1_MAX_LENGTH : 0);
                    if (nOffset + nNeedSpace > nRTSMLength)
                    {
                        dprintf("VQStatsRtsmDumper::DumpConn VQStatsOutputAndResetBlock");
                        VQStatsOutputAndResetBlock(pRTSMBuffer, nOffset);
                        bNeedDumpLayer1 = true;
                    }

                    if (bNeedDumpLayer1)
                    {
                        // Dump entry
                        VQStatsDumpConnEntry(pRTSMBuffer, nRTSMLength, nOffset, Entry1Key, Entry2Key);
                        bNeedDumpLayer1 = false;
                    }

                    // Dump sub entry
                    VQStatsDumpConnSubEntry(pRTSMBuffer, nRTSMLength, nOffset, SubEntryKey, *pSubEntry);
                }
            }
        }
    }

    template <class EntryType>
    static void VQStatsDumpTunnelEntryTmpl(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsTunnelEntryKey& EntryKey, UINT8 nType)
    {
        EntryType* pStatsEntry = (EntryType*)(pRTSMBuffer + nOffset);
        memset(pStatsEntry, 0, sizeof(*pStatsEntry));
        VQStatsDumpEntryHeader(pStatsEntry->header, 1, 0, 0, EntryKey.ipSrc.IsIpv4());
        pStatsEntry->bTunnelType = ((nType == LAYER_GRE) ? 1 : 0);
        pStatsEntry->nLimPortNum = EntryKey.nLimPort;
        EntryKey.ipSrc.Dump(pStatsEntry->ipLowerSrc);
        EntryKey.ipDest.Dump(pStatsEntry->ipLowerDest);
        pStatsEntry->TEI = EntryKey.TEI;
        pStatsEntry->nVLANMPLSIds = min(EntryKey.nVLANMPLSIds, RTSD_MAX_VLANMPLS_IDS);
        for (int i = 0; i < EntryKey.nVLANMPLSIds; ++i)
            pStatsEntry->aVLANMPLSIds[i] = EntryKey.aVLANMPLSIds[i];
        nOffset += FIELD_OFFSET(EntryType, aVLANMPLSIds) + sizeof(pStatsEntry->aVLANMPLSIds[0]) * pStatsEntry->nVLANMPLSIds;
    }

    static void VQStatsDumpTunnelEntry(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsTunnelEntryKey& EntryKey, UINT8 nType)
    {
        if (EntryKey.ipSrc.IsIpv4())
            return VQStatsDumpTunnelEntryTmpl<RtsdBufFmtVQStatsIPv4TunnelEntry>(pRTSMBuffer, nRTSMLength, nOffset, EntryKey, nType);
        else
            return VQStatsDumpTunnelEntryTmpl<RtsdBufFmtVQStatsIPv6TunnelEntry>(pRTSMBuffer, nRTSMLength, nOffset, EntryKey, nType);
    }


    template <class EntryType>
    static void VQStatsDumpTunnelSubEntryTmpl(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsTunnelSubEntryKey& EntryKey, const VQStatsTunnelSubEntry& Entry)
    {
        EntryType* pStatsEntry = (EntryType*)(pRTSMBuffer + nOffset);
        memset(pStatsEntry, 0, sizeof(*pStatsEntry));
        VQStatsDumpEntryHeader(pStatsEntry->header, 1, 1, 1, EntryKey.ipSrc.IsIpv4());
        EntryKey.ipSrc.Dump(pStatsEntry->ipUpperSrc);
        EntryKey.ipDest.Dump(pStatsEntry->ipUpperDest);
        pStatsEntry->ipUpperProtocol = EntryKey.uProtocol;
        pStatsEntry->srcPort = EntryKey.uSrcPort;
        pStatsEntry->destPort = EntryKey.uDestPort;
        nOffset += FIELD_OFFSET(EntryType, tStatsCount);
        VQStatsDumpStreamSet(&pStatsEntry->tStatsCount, Entry.BiDirects[0]);
        nOffset += GetLength(&pStatsEntry->tStatsCount);
    }

    static void VQStatsDumpTunnelSubEntry(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsTunnelSubEntryKey& EntryKey, const VQStatsTunnelSubEntry& Entry)
    {
        if (EntryKey.ipSrc.IsIpv4())
            return VQStatsDumpTunnelSubEntryTmpl<RtsdBufFmtVQStatsIPv4TunnelSubEntry>(pRTSMBuffer, nRTSMLength, nOffset, EntryKey, Entry);
        else
            return VQStatsDumpTunnelSubEntryTmpl<RtsdBufFmtVQStatsIPv6TunnelSubEntry>(pRTSMBuffer, nRTSMLength, nOffset, EntryKey, Entry);
    }

    void VQStatsRtsmDumper::DumpTunnel(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsTunnelEntryTable& EntryTable)
    {
        for (ftl::POSITION Pos = EntryTable.TunnelEntryMap.GetStartPosition(); Pos;)
        {
            const VQStatsTunnelEntryKey& EntryKey = EntryTable.TunnelEntryMap.GetNextKey(Pos);
            const VQStatsTunnelEntry* pEntry = EntryTable.TunnelEntryMap.GetValueAt(Pos);

            if (pEntry->TunnelSubEntryMap.IsEmpty())
                continue;

            bool bNeedDumpLayer1 = true;
            for (ftl::POSITION Pos1 = pEntry->TunnelSubEntryMap.GetStartPosition(); Pos1;)
            {
                const VQStatsTunnelSubEntry* pSubEntry = pEntry->TunnelSubEntryMap.GetValueAt(Pos1);
                const VQStatsTunnelSubEntryKey SubEntryKey = pEntry->TunnelSubEntryMap.GetNextKey(Pos1);

                if (!pSubEntry || !pSubEntry->HasData())
                    continue;

                UINT32 nNeedSpace = VQSTATS_TUNNEL_LAYER2_MAX_LENGTH + (bNeedDumpLayer1 ? VQSTATS_TUNNEL_LAYER1_MAX_LENGTH : 0);
                if (nOffset + nNeedSpace > nRTSMLength)
                {
                    VQStatsOutputAndResetBlock(pRTSMBuffer, nOffset);
                    bNeedDumpLayer1 = true;
                }

                if (bNeedDumpLayer1)
                {
                    // Dump entry
                    VQStatsDumpTunnelEntry(pRTSMBuffer, nRTSMLength, nOffset, EntryKey, EntryTable.nType);
                    bNeedDumpLayer1 = false;
                }

                // Dump sub entry
                VQStatsDumpTunnelSubEntry(pRTSMBuffer, nRTSMLength, nOffset, SubEntryKey, *pSubEntry);
            }
        }
    }

    void VQStatsRtsmDumper::setPrevTime(ftl::timenano& t)
    {
        m_PrevTime = t;
    }
    bool VQStatsRtsmDumper::processTime(ftl::timenano& t)
    {
        setPrevTime(t);

        if(m_Interval > 0)
        {
            if(m_SampleEndTime == 0)  // haven't sampled once.
            {
                m_SampleEndTime = m_PrevTime + m_Interval;
            }
            else
            {
                if( m_PrevTime > m_SampleEndTime)
                {
                    m_SampleEndTime = m_PrevTime + m_Interval;
                    return true;
                }
            }
        }
        return false;
    }
    void VQStatsRtsmDumper::OutputStats()
    {
        if (!m_pKeyMap)
            return;

        // Allocate buffer
        UINT32 nRTSMLength = MAX_RTSM_VQSTATS_PACKET_SIZE;
        UINT8* pRTSMBuffer = new UINT8[nRTSMLength];
        UINT32 nOffset = 0;

        // Setup RTSM Statistics header
        RtsdBufFmtOtherHeader *pOtherHeader = (RtsdBufFmtOtherHeader*)(pRTSMBuffer + nOffset);
        pOtherHeader->recordType = BUFFMT_TYPE_EMPTY;
        pOtherHeader->subType = RTSD_OTHER_SUBTYPE_VQ_STATISTICS;
        pOtherHeader->length = 0;   // updated later
        pOtherHeader->timeSecs = m_PrevTime.sec; //m_nPreviousTimeSec;
        pOtherHeader->timeNSecs = m_PrevTime.nsec; //m_nPreviousTimeNSec;
        nOffset += sizeof(RtsdBufFmtOtherHeader);

        // Setup VQ Stats header
        RtsdBufFmtVQStatsHeader *pVQStatsHeader = (RtsdBufFmtVQStatsHeader*)(pRTSMBuffer + nOffset);
        memset(pVQStatsHeader, 0, sizeof(RtsdBufFmtVQStatsHeader));
        pVQStatsHeader->versionMajor = RTSD_VQ_STATS_VERSION_MAJOR;
        pVQStatsHeader->versionMinor = RTSD_VQ_STATS_VERSION_MINOR;
        ftl::timenano starttime = m_SampleEndTime - m_Interval;
        pVQStatsHeader->nSampleStartTimeSecs = starttime.sec; //m_uSampleEndSecs - m_nSampleInterval;  // This is not accurate (for fileplay in loop) as it assume time stable timing
        pVQStatsHeader->nSampleStartTimeNSecs = starttime.nsec; //m_uSampleEndNSecs;
        pVQStatsHeader->nSampleEndTimeSecs = m_SampleEndTime.sec; //m_uSampleEndSecs;
        pVQStatsHeader->nSampleEndTimeNSecs = m_SampleEndTime.nsec; //m_uSampleEndNSecs;
        nOffset += sizeof(RtsdBufFmtVQStatsHeader);

        dprintf("--- VQStatsRtsmDumper::OutputStats time: %d.%d", pVQStatsHeader->nSampleStartTimeSecs, pVQStatsHeader->nSampleStartTimeNSecs);

        // todo: ignore nTunneledEntries, nTunneledEntrySize, nConnEntries, nConnEntrySize

        // GTP Tunnel Entries
        DumpTunnel(pRTSMBuffer, nRTSMLength, nOffset, m_pKeyMap->m_GTPTunnelEntryTable);

        // GRE Tunnel Entries
        DumpTunnel(pRTSMBuffer, nRTSMLength, nOffset, m_pKeyMap->m_GRETunnelEntryTable);

        // Conn Entries
        DumpConn(pRTSMBuffer, nRTSMLength, nOffset, m_pKeyMap->m_ConnEntryTable);

        // write block if we added anything more than initial headers
        if(nOffset > sizeof(RtsdBufFmtOtherHeader) + sizeof(RtsdBufFmtVQStatsHeader))
        {
            pOtherHeader->length = nOffset - sizeof(RtsdBufFmtOtherHeader);
            if(m_pfnOutputStatsBlock)
                m_pfnOutputStatsBlock(m_uUserParam, pRTSMBuffer, nOffset);
            dprintf("+++ VQStatsRtsmDumper::OutputStats|pOtherHeader->length=%d", pOtherHeader->length);
        }

        delete [] pRTSMBuffer;
    }

}  // namespace mqa