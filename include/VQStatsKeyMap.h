#ifndef MQA_VQSTATSKEYMAP_H_
#define MQA_VQSTATSKEYMAP_H_

#include "mqa_flowkeys.h"
#include "mqa_flowentries.h"
#include "StatsFrameParser.h"

namespace mqa {


    struct VQStatsKeyMapVisitor
    {
        // exit visiting if return false. continue visiting if return true;
        virtual bool visit(VQStatsConnEntryTable&){ return true;}
        virtual bool visit(VQStatsTunnelEntryTable&, bool isGRE){return true;}

        virtual bool visit(VQStatsInterface&){return true;}

        virtual bool visit(VQStatsSubEntry&){return true;}
    };

    class VQStatsKeyMap
    {
    public:
        VQStatsKeyMap();
        ~VQStatsKeyMap();

        // return true if a new flow
        bool FindStreamSet(const StatsFrameParser& Parser,
            CMQmonInterface*& pInterface, VQStatsStreamSetPtr& pStreamSet, VQStatsSubEntry*& SubEntry);

        void PrintStats(std::ostream& os, const string& sPrefix, bool bDetailedHeaders);

        static VQStatsConnSubEntryKey CreateConnSubEntryKey(const StatsFrameParser& Parser, bool* pSrcToDest = NULL);
        static VQStatsConnEntry2Key CreateConnEntry2Key(const StatsFrameParser& Parser);
        static VQStatsConnEntry1Key CreateConnEntry1Key(const StatsFrameParser& Parser);
        static VQStatsTunnelSubEntryKey CreateTunnelSubEntryKey(const StatsFrameParser& Parser);
        static VQStatsTunnelEntryKey CreateTunnelEntryKey(const StatsFrameParser& Parser);

        //private:
        VQStatsConnEntryTable m_ConnEntryTable;
        VQStatsTunnelEntryTable m_GRETunnelEntryTable;
        VQStatsTunnelEntryTable m_GTPTunnelEntryTable;

        void accept(VQStatsKeyMapVisitor&);
    };

}
#endif //MQA_VQSTATSKEYMAP_H_