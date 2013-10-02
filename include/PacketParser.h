#ifndef MQA_PACKET_H_
#define MQA_PACKET_H_

namespace mqa {

    enum PACKET_TYPE {
        kUNKOWN_PACKET      = 0x00000000,

        // layer1
        kLAYER1_PACKET      = 0x0000000F,
        kETHERNET_PACKET    = 0x00000001,

        // layer2
        kLAYER2_PACKET      = 0x000000F0,
        kIP_PACKET          = 0x00000010,

        // layer3
        kLAYER3_PACKET      = 0x0000FF00,
        kUDP_PACKET         = 0x00000100,
        kTCP_PACKET         = 0x00000200,

        // layer4
        kLAYER4_PACKET      = 0x00FF0000,
        kRTP_PACKET         = 0x00010000,

        kLAYER5_PACKET      = 0xFF000000
    };
    struct PacketParser {
        const char   *packetBuffer;
        int           packetLength;
        PACKET_TYPE   packetType;
        PacketParser *lowerPacket;

        PacketParser(const char *buf, int len, PACKET_TYPE type = kUNKOWN_PACKET, PacketParser *lower=0)
            :packetBuffer(buf), packetLength(len), packetType(type), lowerPacket(lower) {}

        // return false if failed to parse current layer.
        virtual bool Parse(){return false;}
        virtual bool IsValid() {return false;}
        virtual const char *GetPayload(int* len){return 0;}

        virtual ~PacketParser(){}
    };


    struct EthernetPacketParser 
        : public PacketParser
    {
        EthernetPacketParser(const char *buf, int len, PacketParser *lower=0): PacketParser(buf, len, kETHERNET_PACKET, lower) {}
    };

    struct IpPacketParser 
        : public PacketParser
    {
        IpPacketParser(const char *buf, int len, PacketParser *lower=0): PacketParser(buf, len, kIP_PACKET, lower) {}

        unsigned char         ver;
        unsigned short        prot;
        unsigned int          destIP[4], srcIP[4];

        bool Parse() {
            ver = packetBuffer[0]>>4;
            // todo
            return true;
        }
    };

    struct UdpPacketParser 
        : public PacketParser
    {
        UdpPacketParser(const char *buf, int len, PacketParser *lower=0): PacketParser(buf, len, kUDP_PACKET, lower) {}
    };


    struct TcpPacketParser
        : public PacketParser
    {
        TcpPacketParser(const char *buf, int len, PacketParser *lower=0): PacketParser(buf, len, kTCP_PACKET, lower) {}
    };


}  // namespace mqa

#endif  // MQA_PACKET_H_
