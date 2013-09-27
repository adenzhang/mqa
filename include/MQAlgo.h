#ifndef MQA_MQALGO_H_
#define MQA_MQALGO_H_

#include "mqa_shared.h"
#include "timesec.h"

#include "mqa_rtptypes.h"

namespace mqa {

    enum PACKET_TYPE {
        kUNKOWN_PACKET      = 0x00000000,

        // layer1
        kLAYER1_PACKET      = 0x0000000F,
        kETHERNET_PACKET    = 0x00000001,

        // layer2
        kLAYER1_PACKET      = 0x000000F0,
        kIP_PACKET          = 0x00000010,

        // layer3
        kLAYER3_PACKET      = 0x0000FF00,
        kUDP_PACKET         = 0x00000100,
        kTCP_PACKET         = 0x00000200,

        // layer4
        kLAYER4_PACKET      = 0x00FF0000,
        kRTP_PACKET         = 0x00010000,

        kLAYER4_PACKET      = 0xFF000000
    };
    struct Packet {
        char         *buf;
        size_t        len;
        PACKET_TYPE   packetType;

        Packet(char *buf, size_t len, PACKET_TYPE type = kUNKOWN_PACKET):buf(buf), len(len), packetType(type) {}

        // return false if failed to parse current layer.
        virtual bool Parse(){return false;}
        virtual Packet *GetPayload(){return NULL;}
    };

    struct EthernetPacket 
        : public Packet
    {
        EthernetPacket(char *buf, size_t len): Packet(buf, len, kETHERNET_PACKET) {}
    };

    struct IpPacket 
        : public Packet
    {
        IpPacket(char *buf, size_t len): Packet(buf, len, kIP_PACKET) {}

        unsigned char         ver;
        unsigned short        prot;
        unsigned int          destIP[4], srcIP[4];

        bool Parse() {
            ver = buf[0]>>4;
            // todo
            return true;
        }
    };

    struct UdpPacket 
        : public Packet
    {
        UdpPacket(char *buf, size_t len): Packet(buf, len, kUDP_PACKET) {}
    };


    struct TcpPacket 
        : public Packet
    {
        TcpPacket(char *buf, size_t len): Packet(buf, len, kTCP_PACKET) {}
    };

    struct RtpPacket 
        : public Packet
    {
        RtpPacket(char *buf, size_t len): Packet(buf, len, kRTP_PACKET) {}

        unsigned char   payloadType;
        unsigned short  sequenceNum;
        unsigned int    timestamp;
    };

    struct RTPStreamInfo
    {
        unsigned int freq;
        unsigned int codecType;
    };

    struct StreamAlgo
    {
        virtual ~StreamAlgo(){}

        virtual void SetStreamInfo(RTPStreamInfo& info) = 0;

        virtual void PacketArrival(ftl::timenano& captureTime, RtpPacket& packet) = 0;

        // calculate results after setting above parameters
        virtual ftl::timenano CalculateOneWayDelay() = 0;

        virtual float CalculateJitter() = 0;

        virtual float CalculateMOS() = 0;
    };

    MQA_API StreamAlgo *CreateStreamAlgo();

}  // namespace mqa

#endif // MQA_MQALGO_H_