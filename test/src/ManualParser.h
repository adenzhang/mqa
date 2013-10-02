#pragma once

#ifndef _MANUAL_PARSER_H_
#define _MANUAL_PARSER_H_

#include "stdint.h"
#include <deque>

#ifdef _WIN32
#include <WinSock2.h>
#endif

typedef enum
{
    CU_PROTOCOL_TYPE_TCP = 0x06,
    CU_PROTOCOL_TYPE_UDP = 0x11,
    CU_PROTOCOL_TYPE_SCTP = 0x84,
    CU_PROTOCOL_TYPE_ICMP_IPV4 = 0x01,
    CU_PROTOCOL_TYPE_ICMP_IPV6 = 0x3A,
    CU_PROTOCOL_TYPE_IPV4 = 0x04,
    CU_PROTOCOL_TYPE_IPV6 = 0x29,

} CU_PROTOCOL_TYPE;

typedef struct _MQmonIpAddr_s
{
    bool bIpv6;
    union
    {
        uint8_t Ipv4[4];
        uint8_t Ipv6[16];
    };
} MQmonIpAddr;

typedef struct _MQmonFrameInfoIp_s
{
    MQmonIpAddr aSrcAddr;
    MQmonIpAddr aDstAddr;
    uint8_t nTOS;
    uint8_t nTTL;
    uint16_t nProtocolLength;
    uint8_t* pProtocolFrame;
} MQmonFrameInfoIp;

typedef struct timeval MQmonTimestamp;
class CMQmonFrameInfo
{
public:
    CMQmonFrameInfo()
        : m_pFrame(NULL)
        , m_nFrameLength(0)
        , nFlags(false)
        , nProtocolLower(0)
        , nProtocolUpper(0)
        , nIpLowerOffset(0)
        , nIpUpperOffset(0)
        , nProtocolLowerOffset(0)
        , nProtocolUpperOffset(0)
    {
        nTimestamp.tv_sec = nTimestamp.tv_usec = 0;
    }

    enum
    {
        IS_TUNNEL = 1,
        IS_IP_LOWER_IPV6 = 1 << 1,
        IS_IP_UPPER_IPV6 = 1 << 2,
        IS_IP_LOWER_FRAG = 1 << 3,
        IS_IP_UPPER_FRAG = 1 << 4,
        IS_IP_INFO_EXTRACT = 1 << 5
    };

    uint16_t nFlags;
    uint8_t nProtocolLower;
    uint8_t nProtocolUpper;
    uint16_t nIpLowerOffset;
    uint16_t nIpUpperOffset;
    uint16_t nProtocolLowerOffset;
    uint16_t nProtocolUpperOffset;
    uint16_t nAppLayerOffset;
    MQmonTimestamp nTimestamp;

    MQmonFrameInfoIp tIpInfo;

    inline uint8_t* Frame() const { return m_pFrame; }
    inline uint16_t FrameLength() const { return m_nFrameLength; }

    inline void SetTunnel(bool bTunnel) { if (bTunnel) nFlags |= IS_TUNNEL; else nFlags &= ~IS_TUNNEL; }
    inline void SetIpLowerIpv6(bool bIpLowerIpv6) { if (bIpLowerIpv6) nFlags |= IS_IP_LOWER_IPV6; else nFlags &= ~IS_IP_LOWER_IPV6; }
    inline void SetIpUpperIpv6(bool bIpUpperIpv6) { if (bIpUpperIpv6) nFlags |= IS_IP_UPPER_IPV6; else nFlags &= ~IS_IP_UPPER_IPV6; }
    inline void SetIpLowerFrag(bool bIpLowerFrag) { if (bIpLowerFrag) nFlags |= IS_IP_LOWER_FRAG; else nFlags &= ~IS_IP_LOWER_FRAG; }
    inline void SetIpUpperFrag(bool bIpUpperFrag) { if (bIpUpperFrag) nFlags |= IS_IP_UPPER_FRAG; else nFlags &= ~IS_IP_UPPER_FRAG; }
    inline void SetIpInfoExtract(bool bIpInfoExtract) { if (bIpInfoExtract) nFlags |= IS_IP_INFO_EXTRACT; else nFlags &= ~IS_IP_INFO_EXTRACT; }

    inline bool IsTunnel(void) const { return (nFlags & IS_TUNNEL) != 0; }
    inline bool IsIpLowerIpv6(void) const { return (nFlags & IS_IP_LOWER_IPV6) != 0; }
    inline bool IsIpUpperIpv6(void) const { return (nFlags & IS_IP_UPPER_IPV6) != 0; }
    inline bool IsIpLowerFrag(void) const { return (nFlags & IS_IP_LOWER_FRAG) != 0; }
    inline bool IsIpUpperFrag(void) const { return (nFlags & IS_IP_UPPER_FRAG) != 0; }
    inline bool IsIpInfoExtract(void) const { return (nFlags & IS_IP_INFO_EXTRACT) != 0; }

    uint16_t IpLowerLength() const;
    uint16_t IpUpperLength() const;

    uint16_t SrcAddrLowerOffset() const;
    uint16_t SrcAddrUpperOffset() const;
    uint16_t DstAddrLowerOffset() const;
    uint16_t DstAddrUpperOffset() const;

    inline uint16_t SrcPortLowerOffset() const { return nProtocolLowerOffset; }
    inline uint16_t SrcPortUpperOffset() const { return nProtocolUpperOffset; }
    inline uint16_t DstPortLowerOffset() const { return (nProtocolLowerOffset + 2); }
    inline uint16_t DstPortUpperOffset() const { return (nProtocolUpperOffset + 2); }

    inline uint16_t SrcPortLower(uint8_t* pBuffer = NULL) const { return ntohs(*(uint16_t*)((pBuffer ? pBuffer : m_pFrame) + SrcPortLowerOffset())); }
    inline uint16_t SrcPortUpper(uint8_t* pBuffer = NULL) const { return ntohs(*(uint16_t*)((pBuffer ? pBuffer : m_pFrame) + SrcPortUpperOffset())); }
    inline uint16_t DstPortLower(uint8_t* pBuffer = NULL) const { return ntohs(*(uint16_t*)((pBuffer ? pBuffer : m_pFrame) + DstPortLowerOffset())); }
    inline uint16_t DstPortUpper(uint8_t* pBuffer = NULL) const { return ntohs(*(uint16_t*)((pBuffer ? pBuffer : m_pFrame) + DstPortUpperOffset())); }

    inline uint8_t* IpInnerPtr(uint8_t* pBuffer = NULL) const { return ((pBuffer ? pBuffer : m_pFrame) + (IsTunnel() ? nIpUpperOffset : nIpLowerOffset)); }
    inline uint16_t IpInnerLength() const { return (IsTunnel() ? IpUpperLength() : IpLowerLength()); }
    inline uint8_t* SrcAddrInnerPtr(uint8_t* pBuffer = NULL) const { return ((pBuffer ? pBuffer : m_pFrame) + (IsTunnel() ? SrcAddrUpperOffset() : SrcAddrLowerOffset())); }
    inline uint8_t* DstAddrInnerPtr(uint8_t* pBuffer = NULL) const { return ((pBuffer ? pBuffer : m_pFrame) + (IsTunnel() ? DstAddrUpperOffset() : DstAddrLowerOffset())); }
    inline uint8_t* SrcPortInnerPtr(uint8_t* pBuffer = NULL) const { return ((pBuffer ? pBuffer : m_pFrame) + (IsTunnel() ? SrcPortUpperOffset() : SrcPortLowerOffset())); }
    inline uint8_t* DstPortInnerPtr(uint8_t* pBuffer = NULL) const { return ((pBuffer ? pBuffer : m_pFrame) + (IsTunnel() ? DstPortUpperOffset() : DstPortLowerOffset())); }


    bool Extract(uint8_t* pFrame, uint16_t nFrameLength, bool bExtractIpInfo);

private:
    uint8_t* m_pFrame;
    uint16_t m_nFrameLength;

    bool ExtractIPUpperFrameData(uint8_t* pFrame, uint16_t nFrameLength, uint32_t& nBitOffset);
    bool ExtractIPInfo(uint8_t* pFrame, uint16_t nFrameLength, uint32_t& nBitOffset,
        uint16_t& nIpOffset, bool& bIpv6, bool& bFrag, uint8_t& nProtocol, uint16_t& nProtocolOffset);
    bool Parse(uint8_t* pFrame, uint16_t nFrameLength);
};

typedef std::deque<CMQmonFrameInfo*> CMQmonFrameList;

#endif //_MANUAL_PARSER_H_