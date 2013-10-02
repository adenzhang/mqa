#ifndef MQA_RPTSTREAMMANAGER_H_
#define MQA_RPTSTREAMMANAGER_H_

#include "mqa_shared.h"
#include "timesec.h"
#include "RtpStream.h"

namespace mqa {
    
    class RtpStreamManager
    {
    public:
        typedef RtpStream* RtpStreamPtr;
        struct StreamEventHandler
        {
            enum EVENT_T { EVT_STREAM_ACTIVATING = 0x01, EVT_STREAM_DEACTIVATING = 0x02 };

            virtual bool handle(EVENT_T event, RtpStreamPtr) = 0;
        };

        virtual void init(StreamEventHandler *handler) = 0;
        
        virtual void IndicateRtpPacket(ftl::timenano& timestamp, UINT8 *packet, int packetlen) = 0;

        virtual ~RtpStreamManager();
    };

    MQA_API RtpStreamManager *CreateRtpStreamManager();
}  // namespace mqa

#endif