#ifndef MQA_MAQINTERFACE_H_
#define MQA_MAQINTERFACE_H_

#include <list>
#include "mqa_shared.h"

#include "mqa_global.h"

#include "ftlcollections.h"

namespace mqa {

    enum STREAMTYPE_T
    {
        STREAM_UNKOWN     = 0,
        STREAM_AUDIO      = 1 << 1,
        STREAM_VIDEO      = 1 << 2,
        STREAM_VOICE      = 1 << 3
    } ;


    typedef void *FlowKeyPtr;

    struct StreamResult {
        float     mos;
        float     jitter;
        char      streamType; //STREAMTYPE_T
        char      codecType;
    };  // StreamResult

    typedef std::list<std::pair<FlowKeyPtr, StreamResult> >StreamResultSet;


    // analyzer engine type, to be specified when getting Analyzer instance.
    enum ENGINE_TYPE {ENGINE_TELCHM, ENGINE_DRIVETEST};

    enum LOGLEVEL_TYPE {LOG_ERROR, LOG_INFO, LOG_DEBUG};

    struct StreamEventHandler
    {
        enum EVENT_T { EVT_STREAM_ACTIVATED = 0x01, EVT_STREAM_DISACTIVATED = 0x02 };

        virtual bool handle(EVENT_T event, FlowKeyPtr&) = 0;
    };
    struct ResultEventHandler
    {
        virtual bool handle(StreamResultSet *result) = 0;
    };

    // An analyzer is an analysis instance which handles all the data feeding into it.
    // It is not safe when accessed by multiple threads.
    struct Analyzer {


        //--------- configure instance -----------

        virtual bool setLogLevel(LOGLEVEL_TYPE loglevel) = 0;

        // Analyzer users should implement EventHanlder and set to Analyzer.
        // when some event happens, the handler will be called to notify users.
        virtual void setStreamEventhandler(StreamEventHandler *handler) = 0;

        // Result handler can be set with an time interval
        // users can passively receive results
        virtual void setResultEventHandler(ResultEventHandler *handler, unsigned int millisec) = 0;

        //--------- start to work -----------

        virtual bool start() = 0;

        virtual bool feedPacket(char *ethernetHeader, size_t len, unsigned int timesec, unsigned int timeusec, int limID = -1) = 0;

        // Or users may actively retrieve results
        virtual bool retrieveResults(StreamResultSet& results) = 0;

        virtual bool stopFlow(FlowKeyPtr& flow) = 0;

        virtual void stop() = 0;

        // destroy the object
        virtual void release() = 0;

        //--------- Instance attributes -----------

        virtual bool isSingleton() = 0;

    };  // Analyzer

    // if analyzer is singleton, return the old one.
    MQA_API Analyzer *Mqa_CreateAnalyzer(ENGINE_TYPE engine = ENGINE_TELCHM);
} // namespace mqa

#endif //MQA_MAQINTERFACE_H_
