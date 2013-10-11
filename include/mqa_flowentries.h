#ifndef MQA_MAQ_FLOWENTRIES_H_
#define MQA_MAQ_FLOWENTRIES_H_

#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>
#include <boost/shared_ptr.hpp>

#include "MQmonIf.h"
#include "ftl/ftlcollections.h"
#include "mqa_flowkeys.h"
#include "mqainterface.h"
#include "classifyconsts.h"

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#endif

namespace mqa {

    using namespace std;

    inline int SizeOf(RtsdBufFmtVQStatsType atype) {
        switch(atype) 
        {
        case RtsdBufFmtVQStatsEntryCountTypeMOS_T: return sizeof(RtsdBufFmtVQStatsEntryCountTypeMOS); 
        default: return 0; 
        }
    } 
    inline int GetLength(RtsdBufFmtVQStatsEntryCount* entryCount) {
        int n = 0;
        for(int i=0; i<entryCount->nTypeCount; ++i)
            n += SizeOf((RtsdBufFmtVQStatsType)entryCount->Block[n]);
        return n + sizeof(entryCount->nTypeCount);
    }

    class FlowKey;
    class VQStatsStream
    {
    public:
        VQStatsStream(MQmonStreamType type) : streamType(type),pMQmonStream(NULL), bActivate(false){};
        ~VQStatsStream()
        {
            // TODO: need set m_nLastTime of interface, so that
            // VQmonInterfaceStreamsCleanupInactive(m_hInterface, m_nLastTime, 0); will destroy those inactive streams.
            //if (bActivate)
            //    dprintf(__FUNCTION__" error: stream %p may still be in use.\n", this);
        }
        virtual bool retrieveResults()
        {
            if(pMQmonStream) {
                if( pMQmonStream->GetMetrics(metrics) ) {
                    result.mos = metrics.MOS;
                    result.jitter = metrics.Jitter;
                    result.streamType = streamType;
                    result.codecType =  pMQmonStream->m_codecType;
                    return true;
                }
            }
            return false;
        }

        CMQmonStream  *pMQmonStream;
        CMQmonMetrics  metrics;
        bool           bActivate;
        StreamResult   result;
        MQmonStreamType streamType;
        FlowKey        *flowKey;
        //    RtsdBufFmtVQStatsMOS nMOS;
    };
    typedef boost::shared_ptr<VQStatsStream> VQStatsStreamPtr;

    class VQStatsStreamSet
    {
    public:
        VQStatsStreamSet():Streams(3) {
            Streams[0].reset(new VQStatsStream(MQMON_STREAM_AUDIO));
            Streams[1].reset(new VQStatsStream(MQMON_STREAM_VIDEO));
            Streams[2].reset(new VQStatsStream(MQMON_STREAM_VOICE));
        };
        ~VQStatsStreamSet() {};

        virtual VQStatsStreamPtr GetStream(MQmonStreamType nType)
        {
            switch (nType)
            {
            case MQMON_STREAM_AUDIO:
                return Streams[0];
            case MQMON_STREAM_VIDEO:
                return Streams[1];
            case MQMON_STREAM_VOICE:
                return Streams[2];
            default:
                return VQStatsStreamPtr();
            }
        }
        virtual VQStatsStreamPtr GetActiveStream()
        {
            for(int i=0; i<3; ++i) {
                if(Streams[i]->bActivate) {
                    return Streams[i];
                }
            }
            return VQStatsStreamPtr();
        }
        virtual void retrieveResults()
        {
            for(size_t i=0;i<Streams.size(); ++i) {
                if( Streams[i]->bActivate )
                    Streams[i]->retrieveResults();
            }
        }
        virtual void dump(RtsdBufFmtVQStatsEntryCount& e)
        {
            e.nTypeCount = 1;
            RtsdBufFmtVQStatsEntryCountTypeMOS * t = (RtsdBufFmtVQStatsEntryCountTypeMOS*)e.Block;
            t->VQStatsType = RtsdBufFmtVQStatsEntryCountTypeMOS_T;

            if(GetStream(MQMON_STREAM_AUDIO)->pMQmonStream) t->MOS_Audio = GetStream(MQMON_STREAM_AUDIO)->result.mos +5;
            if(GetStream(MQMON_STREAM_VIDEO)->pMQmonStream) t->MOS_Video = GetStream(MQMON_STREAM_VIDEO)->result.mos +5;
            if(GetStream(MQMON_STREAM_VOICE)->pMQmonStream) t->MOS_Voice = GetStream(MQMON_STREAM_VOICE)->result.mos +5;

            assert(GetLength(&e) <= RtsdBufFmtVQStatsEntryCount_BlockMaxSize);
        }

        inline bool HasActivate() const
        {
            for (int i = 0; i < 3; ++i)
                if (Streams[i]->bActivate)
                    return true;
            return false;
        }

        friend std::ostream& operator<<(std::ostream& os, const VQStatsStreamSet& StreamSet)
        {
            if (StreamSet.Streams[0]->pMQmonStream)
                os << "Audio_MOS(" << StreamSet.Streams[0]->result.mos << ")";
            if (StreamSet.Streams[1]->pMQmonStream)
                os << "Video_MOS(" << StreamSet.Streams[1]->result.mos << ")";
            if (StreamSet.Streams[2]->pMQmonStream)
                os << "Voice_MOS(" << StreamSet.Streams[2]->result.mos << ")";
            return os;
        }
        std::vector<VQStatsStreamPtr> Streams; // [0] audio; [1] video; [2] voice
    };
    typedef boost::shared_ptr<VQStatsStreamSet> VQStatsStreamSetPtr;


    class VQStatsInterface
    {
    public:
        VQStatsInterface(UINT8 nType_) : nType(nType_) {
            pMQmonInterface = MQmon::Instance()->CreateInterface();
            pMQmonInterface->m_pUserdata = this;
        }
        virtual ~VQStatsInterface() {
            if (pMQmonInterface)
            {
                delete pMQmonInterface;
                pMQmonInterface = NULL;
            }
        }

        UINT8 nType;
        CMQmonInterface *pMQmonInterface;
        void *userData;
    };

    ////////////////////////////// Conn Entry //////////////////////////////

    class VQStatsSubEntry //VQStatsConnSubEntry
    {
    public:
        //    VQStatsSubEntry():threadIdx(-1) {}
        VQStatsSubEntry(const VQStatsTunnelKey& k):flowKey(k), flowKey1(k), threadIdx(-1) 
        {
            BiFlowKeys[0] = &flowKey;
            BiFlowKeys[1] = &flowKey1;
        }
        VQStatsSubEntry(const VQStatsConnKey& k):flowKey(k), flowKey1(k), threadIdx(-1) {
            std::swap(flowKey1.pConnKey->ip, flowKey.pConnKey->ip);
            std::swap(flowKey1.pConnKey->port, flowKey.pConnKey->port);
        }

        ~VQStatsSubEntry() {}

        inline bool HasData(int k) const {
            if(k<0 || k>1) return false;
            return BiDirects[k] &&BiDirects[k]->HasActivate();
        }
        inline bool HasData() const
        {
            return (BiDirects[0] &&BiDirects[0]->HasActivate()) || (BiDirects[1] && BiDirects[1]->HasActivate());
        }

        int                 threadIdx;
        FlowKey           flowKey;
        FlowKey           flowKey1;
        FlowKey             *BiFlowKeys[2];
        VQStatsStreamSetPtr BiDirects[2]; // [0] src->dst; [1] dst->src
#ifdef VQSTATS_DEBUG
        VQStatsConnSubEntryKey Key;
#endif
    };
    typedef VQStatsSubEntry VQStatsConnSubEntry;

    typedef ftl::Map<VQStatsConnSubEntryKey, VQStatsConnSubEntry*, ftl::ElementTraits<VQStatsConnSubEntryKey>> VQStatsConnSubEntryMap;

    class VQStatsConnEntry2
    {
    public:
        VQStatsConnEntry2() {}
        ~VQStatsConnEntry2()
        {
            for (ftl::POSITION Pos = ConnSubEntryMap.GetStartPosition(); Pos;)
                delete ConnSubEntryMap.GetNextValue(Pos);
            ConnSubEntryMap.RemoveAll();
        }

        VQStatsConnSubEntryMap ConnSubEntryMap;
#ifdef VQSTATS_DEBUG
        VQStatsConnEntry2Key Key;
#endif
    };

    typedef ftl::Map<VQStatsConnEntry2Key, VQStatsConnEntry2*, ftl::ElementTraits<VQStatsConnEntry2Key> > VQStatsConnEntry2Map;


    class VQStatsConnEntry1 : public VQStatsInterface
    {
    public:
        VQStatsConnEntry1(UINT8 nType=LAYER_NONE)
            : VQStatsInterface(nType)
        {}
        ~VQStatsConnEntry1()
        {
            for (ftl::POSITION Pos = ConnEntry2Map.GetStartPosition(); Pos;)
                delete ConnEntry2Map.GetNextValue(Pos);
            ConnEntry2Map.RemoveAll();
        }

        VQStatsConnEntry2Map ConnEntry2Map;
#ifdef VQSTATS_DEBUG
        VQStatsConnEntry1Key Key;
#endif
    };

    typedef ftl::Map<VQStatsConnEntry1Key, VQStatsConnEntry1*, ftl::ElementTraits<VQStatsConnEntry1Key> > VQStatsConnEntry1Map;

    class VQStatsConnEntryTable
    {
    public:
        VQStatsConnEntryTable() {}
        ~VQStatsConnEntryTable()
        {
            for (ftl::POSITION Pos = ConnEntry1Map.GetStartPosition(); Pos;)
                delete ConnEntry1Map.GetNextValue(Pos);
            ConnEntry1Map.RemoveAll();
        }
        VQStatsSubEntry* findflow(const VQStatsConnKey& flow, bool addIfNotExist=false) {
            VQStatsConnEntry1 *entry1 = NULL;
            if( !ConnEntry1Map.Lookup(flow.vlan, entry1) ) {
                if(addIfNotExist){
                    entry1 = new VQStatsConnEntry1();
                    ConnEntry1Map.SetAt(flow.vlan, entry1);
                }else
                    return NULL;
            }
            if(!entry1) return NULL;
            VQStatsConnEntry2 *entry2 = NULL;
            if(!entry1->ConnEntry2Map.Lookup(flow.ip, entry2)) {
                if(addIfNotExist){
                    entry2 = new VQStatsConnEntry2();
                    entry1->ConnEntry2Map.SetAt(flow.ip, entry2);
                }else
                    return NULL;
            }
            VQStatsConnSubEntry *subEntry = NULL;
            if(!entry2->ConnSubEntryMap.Lookup(flow.port, subEntry)){
                if(addIfNotExist){
                    subEntry = new VQStatsConnSubEntry(flow);
                    entry2->ConnSubEntryMap.SetAt(flow.port, subEntry);
                }else
                    return NULL;
            }
            return subEntry;
        }

        void PrintStats(std::ostream& os, const string& sPrefix, bool bDetailedHeaders)
        {
            if (bDetailedHeaders)
                os << "==== # " << "Conn" << " connections = " << ConnEntry1Map.GetCount() << ", "
                "hash table size = " << ConnEntry1Map.GetHashTableSize() << " ====" << endl;
            for (ftl::POSITION Pos = ConnEntry1Map.GetStartPosition(); Pos;)
            {
                const VQStatsConnEntry1* pEntry1 = ConnEntry1Map.GetValueAt(Pos);
                const VQStatsConnEntry1Key& Entry1Key = ConnEntry1Map.GetNextKey(Pos);

                for (ftl::POSITION Pos1 = pEntry1->ConnEntry2Map.GetStartPosition(); Pos1;)
                {
                    const VQStatsConnEntry2* pEntry2 = pEntry1->ConnEntry2Map.GetValueAt(Pos1);
                    const VQStatsConnEntry2Key& Entry2Key = pEntry1->ConnEntry2Map.GetNextKey(Pos1);

                    stringstream ss;
                    ss << sPrefix << "Conn" << ", " << Entry2Key.ipSrc << ", " << Entry2Key.ipDest << ", , ";

                    for (ftl::POSITION Pos2 = pEntry2->ConnSubEntryMap.GetStartPosition(); Pos2;)
                    {
                        const VQStatsConnSubEntry* pSubEntry = pEntry2->ConnSubEntryMap.GetValueAt(Pos2);
                        const VQStatsConnSubEntryKey SubEntryKey = pEntry2->ConnSubEntryMap.GetNextKey(Pos2);

                        os << ss.str() << ", , " << (int)SubEntryKey.uProtocol << ", "
                            << htons(SubEntryKey.uSrcPort) << ", " << htons(SubEntryKey.uDestPort) << ",";
                        if (pSubEntry->BiDirects[0]->HasActivate())
                            os << " S->D: " << pSubEntry->BiDirects[0];
                        if (pSubEntry->BiDirects[1]->HasActivate())
                            os << " D->S: " << pSubEntry->BiDirects[1];
                        os << endl;
                    }
                }
            }
        }

        VQStatsConnEntry1Map ConnEntry1Map;
    };

    ////////////////////////////// Tunnel Entry //////////////////////////////

    typedef VQStatsSubEntry VQStatsTunnelSubEntry;

    typedef ftl::Map<VQStatsTunnelSubEntryKey, VQStatsTunnelSubEntry*, ftl::ElementTraits<VQStatsTunnelSubEntryKey> > VQStatsTunnelSubEntryMap;

    class VQStatsTunnelEntry : public VQStatsInterface
    {
    public:
        VQStatsTunnelEntry(UINT8 nType)
            : VQStatsInterface(nType)
        {}
        ~VQStatsTunnelEntry()
        {
            for (ftl::POSITION Pos = TunnelSubEntryMap.GetStartPosition(); Pos;)
                delete TunnelSubEntryMap.GetNextValue(Pos);
            TunnelSubEntryMap.RemoveAll();
        }

        VQStatsTunnelSubEntryMap TunnelSubEntryMap;
#ifdef VQSTATS_DEBUG
        VQStatsTunnelEntryKey Key;
#endif
    };

    typedef ftl::Map<VQStatsTunnelEntryKey, VQStatsTunnelEntry*, ftl::ElementTraits<VQStatsTunnelEntryKey>> VQStatsTunnelEntryMap;

    class VQStatsTunnelEntryTable
    {
    public:
        VQStatsTunnelEntryTable(UINT8 nType_) : nType(nType_) {};
        ~VQStatsTunnelEntryTable()
        {
            for (ftl::POSITION Pos = TunnelEntryMap.GetStartPosition(); Pos;)
                delete TunnelEntryMap.GetNextValue(Pos);
            TunnelEntryMap.RemoveAll();
        }

        VQStatsSubEntry* findflow(const VQStatsTunnelKey& flow, bool addIfNotExist=false, UINT8 nType=LAYER_GRE) {
            VQStatsTunnelEntry *entry = NULL;
            if( !TunnelEntryMap.Lookup(flow.lowerAddr, entry) ) {
                if(addIfNotExist){
                    entry = new VQStatsTunnelEntry(nType);
                    TunnelEntryMap.SetAt(flow.lowerAddr, entry);
                }else
                    return NULL;
            }
            VQStatsTunnelSubEntry *subEntry = NULL;
            if(!entry->TunnelSubEntryMap.Lookup(flow.upperAddr, subEntry)){
                if(addIfNotExist){
                    subEntry = new VQStatsTunnelSubEntry(flow);
                    entry->TunnelSubEntryMap.SetAt(flow.upperAddr, subEntry);
                }else
                    return NULL;
            }
            return subEntry;
        }

        void PrintStats(std::ostream& os, const string& sPrefix, bool bDetailedHeaders)
        {
            string sName = ((nType == LAYER_GRE) ? "GRE" : "GTP");
            if (bDetailedHeaders)
                os << "==== # " << sName << " tunnels = " << TunnelEntryMap.GetCount() << ", "
                "hash table size = " << TunnelEntryMap.GetHashTableSize() << " ====" << endl;
            for (ftl::POSITION Pos = TunnelEntryMap.GetStartPosition(); Pos;)
            {
                const VQStatsTunnelEntryKey& EntryKey = TunnelEntryMap.GetNextKey(Pos);
                const VQStatsTunnelEntry* pEntry = TunnelEntryMap.GetValueAt(Pos);

                stringstream ss;
                ss << sPrefix << sName << ", " << EntryKey.ipSrc << ", "
                    << EntryKey.ipDest << ", " << EntryKey.TEI << ", ";

                for (ftl::POSITION Pos1 = pEntry->TunnelSubEntryMap.GetStartPosition(); Pos1;)
                {
                    const VQStatsTunnelSubEntry* pSubEntry = pEntry->TunnelSubEntryMap.GetValueAt(Pos1);
                    const VQStatsTunnelSubEntryKey SubEntryKey = pEntry->TunnelSubEntryMap.GetNextKey(Pos1);

                    os << ss.str() << SubEntryKey.ipSrc << ", " << SubEntryKey.ipDest << ", " << (int)SubEntryKey.uProtocol
                        << ", " << htons(SubEntryKey.uSrcPort) << ", " << htons(SubEntryKey.uDestPort) << ", "
                        << pSubEntry->BiDirects[0] << endl;
                }
            }
        }

        UINT8 nType;
        VQStatsTunnelEntryMap TunnelEntryMap;
    };

} // namespace mqa

#endif // MQA_MAQ_FLOWENTRIES_H_
