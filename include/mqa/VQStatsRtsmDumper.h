#ifndef MQA_VQSTATSRTSMDUMPER_H_
#define MQA_VQSTATSRTSMDUMPER_H_

#include "VQStatsKeyMap.h"
#include "mqainterface.h"

namespace mqa {

#define VQSTATS_CONN_LAYER1_MAX_LENGTH      (sizeof(RtsdBufFmtVQStatsIPv6ConnEntry) )
#define VQSTATS_CONN_LAYER2_MAX_LENGTH      (sizeof(RtsdBufFmtVQStatsEntryHeader) + sizeof(VQStatsConnSubEntryKey) + 2*sizeof(RtsdBufFmtVQStatsEntryCount))
#define VQSTATS_TUNNEL_LAYER1_MAX_LENGTH    (sizeof(RtsdBufFmtVQStatsIPv6TunnelEntry))
#define VQSTATS_TUNNEL_LAYER2_MAX_LENGTH    (sizeof(RtsdBufFmtVQStatsEntryHeader) + sizeof(VQStatsTunnelSubEntryKey) + sizeof(RtsdBufFmtVQStatsEntryCount))

    struct VQStatsRtsmDumper
    {
        void init(VQStatsKeyMap *keymap, Analyzer::PFN_OutputBlockCallback pFunc, INT32 param, const ftl::timenano& interval)
        {
            m_pKeyMap = keymap;
            m_pfnOutputStatsBlock = pFunc;
            m_uUserParam = param;
            m_Interval = interval; 
        }

        void OutputStats();

        void DumpTunnel(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsTunnelEntryTable& EntryTable);
        void DumpConn(UINT8* pRTSMBuffer, UINT32 nRTSMLength, UINT32& nOffset, const VQStatsConnEntryTable& EntryTable);

        void VQStatsOutputAndResetBlock(UINT8* pRTSMBuffer, UINT32& nOffset);

        // return true if it's time to output
        bool processTime(const ftl::timenano& timestamp);

    protected:

        VQStatsKeyMap *m_pKeyMap;


        ftl::timenano        m_PrevTime, m_SampleEndTime, m_Interval;

        Analyzer::PFN_OutputBlockCallback m_pfnOutputStatsBlock;
        INT32          m_uUserParam;

        void setPrevTime(const ftl::timenano &timestamp);
    };

}  // namespace mqa

#endif //MQA_VQSTATSRTSMDUMPER_H_
