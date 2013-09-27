#ifndef MQA_MAQ_ANALYZERIMPL_H_
#define MQA_MAQ_ANALYZERIMPL_H_

#include "RtsdSupport.h"
#include "StatsFrameParser.h"
#include "mqmon.h"
#include "mqa_flowkeys.h"
#include "mqa_flowentries.h"

#include "VQStatsRtsmDumper.h"
#include "VQStatsKeyMap.h"

namespace mqa {

    struct RetrieveResultsVisitor
        : public VQStatsKeyMapVisitor
    {
        StreamResultSet& results;
        RetrieveResultsVisitor(StreamResultSet& results):results(results){}

        virtual bool visit(VQStatsSubEntry& e)
        {
            for(int i=0; i<2; ++i)
                if( e.HasData(i) && e.BiDirects[i]->GetActiveStream()->retrieveResults() )
                    results.push_back(std::make_pair(&e.BiFlowKeys[i], e.BiDirects[i]->GetActiveStream()->result));
            return true;
        }
    };
    class AnalyzerImpl
        :public Analyzer
    {
    public:
        AnalyzerImpl();

        //--------- configure instance -----------

        virtual bool setLogLevel(LOGLEVEL_TYPE loglevel);

        // Analyzer users should implement EventHanlder and set to Analyzer.
        // when some event happens, the handler will be called to notify users.
        virtual void setStreamEventhandler(StreamEventHandler *handler);

        // Result handler can be set with an time interval
        // users can passively receive results
        virtual void setResultEventHandler(ResultEventHandler *handler, timeval& interval);

        virtual void setResultOutputCallback(UINT32 nUserParm, PFN_OutputBlockCallback pOutput, timeval& interval);
        //--------- start to work -----------

        virtual bool start();

        virtual bool feedPacket(char *ethernetHeader, size_t len, timeval& timestamp, int limID = -1);

        // check timeout to output
        virtual void processTime(timeval& timestamp);

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
        PFN_OutputBlockCallback _outputHandler;
        UINT32                _outputParam;
        timeval               _resultInterval; 
        VQStatsKeyMap         _vqStatsMap;

        //-----
        timeval               _lastResultTime;

        VQStatsRtsmDumper     _rtsmDumper;

        virtual void outputResults(timeval& t);

    protected:
        static bool VQStatsNotifyHandler(CMQmonInterface* pInterface, CMQmonStream* pStream, MQmonNotifyType nType, MQmonNotifyInfo* pInfo);
        static void InitVQStats(LOGLEVEL_TYPE loglevel);
        static void FiniVQStats(void);
        static LONG g_nInstances;
    };

} // namespace xtreme::mqa

#endif // MQA_MAQ_ANALYZERIMPL_H_
