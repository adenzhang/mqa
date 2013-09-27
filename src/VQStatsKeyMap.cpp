#include "VQStatsKeyMap.h"
#include "MqaAssert.h"

//=========================== VQStatsMap ===================================
namespace mqa {

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

    void VQStatsKeyMap::accept(VQStatsKeyMapVisitor& v)
    {
        //--- visit conn table
        do{

            VQStatsConnEntryTable& EntryTable = m_ConnEntryTable;
            if( !v.visit(EntryTable) ) return;
            for (ftl::POSITION Pos = EntryTable.ConnEntry1Map.GetStartPosition(); Pos;)
            {
                VQStatsConnEntry1* pEntry1 = EntryTable.ConnEntry1Map.GetValueAt(Pos);
                const VQStatsConnEntry1Key& Entry1Key = EntryTable.ConnEntry1Map.GetNextKey(Pos);

                VQStatsInterface* pStatsInterface = dynamic_cast<VQStatsInterface*> (pEntry1);
                ASSERTMSG1(pStatsInterface, "ConnEntry1Map dynamic_cast<VQStatsInterface*> (pEntry1)");
                if( !v.visit(*pStatsInterface) ) return;

                for (ftl::POSITION Pos1 = pEntry1->ConnEntry2Map.GetStartPosition(); Pos1;)
                {
                    const VQStatsConnEntry2* pEntry2 = pEntry1->ConnEntry2Map.GetValueAt(Pos1);
                    const VQStatsConnEntry2Key& Entry2Key = pEntry1->ConnEntry2Map.GetNextKey(Pos1);

                    if (pEntry2->ConnSubEntryMap.IsEmpty())
                        continue;

                    for (ftl::POSITION Pos2 = pEntry2->ConnSubEntryMap.GetStartPosition(); Pos2;)
                    {
                        VQStatsConnSubEntry* pSubEntry = pEntry2->ConnSubEntryMap.GetValueAt(Pos2);
                        const VQStatsConnSubEntryKey SubEntryKey = pEntry2->ConnSubEntryMap.GetNextKey(Pos2);
                        if( !v.visit(*pSubEntry) ) return;
                    }
                }
            }
        }while(false);

        //--- visit tunnel tables
        VQStatsTunnelEntryTable* pTunnelTables[2] = {&m_GRETunnelEntryTable, &m_GTPTunnelEntryTable};
        for(int i=0; i<2; ++i)
        {
            VQStatsTunnelEntryTable& EntryTable = *pTunnelTables[i];
            if( !v.visit(EntryTable, i==1?true:false) ) return;

            for (ftl::POSITION Pos = EntryTable.TunnelEntryMap.GetStartPosition(); Pos;)
            {
                const VQStatsTunnelEntryKey& EntryKey = EntryTable.TunnelEntryMap.GetNextKey(Pos);
                VQStatsTunnelEntry* pEntry = EntryTable.TunnelEntryMap.GetValueAt(Pos);

                VQStatsInterface* pStatsInterface = dynamic_cast<VQStatsInterface*> (pEntry);
                ASSERTMSG1(pStatsInterface, "TunnelEntryMap dynamic_cast<VQStatsInterface*> (pEntry)");
                if( !v.visit(*pStatsInterface) ) return;

                if (pEntry->TunnelSubEntryMap.IsEmpty())
                    continue;

                for (ftl::POSITION Pos1 = pEntry->TunnelSubEntryMap.GetStartPosition(); Pos1;)
                {
                    VQStatsTunnelSubEntry* pSubEntry = pEntry->TunnelSubEntryMap.GetValueAt(Pos1);
                    const VQStatsTunnelSubEntryKey SubEntryKey = pEntry->TunnelSubEntryMap.GetNextKey(Pos1);
                    if( !v.visit(*pSubEntry) ) return;
                }
            }
        }
    }

} // namespace mqa