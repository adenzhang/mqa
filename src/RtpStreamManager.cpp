#include "RtpStreamManager.h"
namespace mqa {

    struct RtpStreamManagerImpl
        :public RtpStreamManager
    {
        StreamEventHandler   *streamHandler;

        RtpStreamManagerImpl():streamHandler(NULL){}
        ~RtpStreamManagerImpl(){}

        void RtpStreamManagerImpl::init(StreamEventHandler *handler)
        {
            streamHandler = handler;
        }
        void RtpStreamManagerImpl::IndicateRtpPacket(ftl::timenano& timestamp, UINT8 *packet, int packetlen)
        {
            // todo
            return;
        }
    };

}  // namespace mqa
