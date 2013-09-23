#ifndef MQA_MAQ_ANALYZERIMPL_H_
#define MQA_MAQ_ANALYZERIMPL_H_

#include "RtsdSupport.h"
#include "StatsFrameParser.h"
#include "mqmon.h"
#include "mqa_flowkeys.h"
#include "mqa_flowentries.h"

#include <WinSock2.h>

namespace mqa {

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

    };

    class AnalyzerImpl
        :public Analyzer
    {
    public:

        //--------- configure instance -----------

        virtual bool setLogLevel(LOGLEVEL_TYPE loglevel);

        // Analyzer users should implement EventHanlder and set to Analyzer.
        // when some event happens, the handler will be called to notify users.
        virtual void setStreamEventhandler(StreamEventHandler *handler);

        // Result handler can be set with an time interval
        // users can passively receive results
        virtual void setResultEventHandler(ResultEventHandler *handler, unsigned int millisec);

        //--------- start to work -----------

        virtual bool start();

        virtual bool feedPacket(char *ethernetHeader, size_t len, unsigned int timesec, unsigned int timeusec, int limID = -1);

        // Or users may actively retrieve results
        virtual bool retrieveResults(StreamResultSet& results);

        virtual bool stopFlow(FlowKeyPtr& flow);

        virtual void stop();

        // destroy the object
        virtual void release();

        //--------- Instance attributes -----------

        virtual bool isSingleton();


    protected:
        LOGLEVEL_TYPE         _logLevel;
        ENGINE_TYPE           _engineType;
        ResultEventHandler*   _resultHandler;
        StreamEventHandler*   _streamHandler;
        UINT64                _resultInterval;
        VQStatsKeyMap         _vqStatsMap;
    };

} // namespace xtreme::mqa

#endif // MQA_MAQ_ANALYZERIMPL_H_
