#ifndef MQA_STATSPARSER_H_
#define MQA_STATSPARSER_H_

#include "mqa_global.h"
#include "mqa_shared.h"
#include "classifyconsts.h"

namespace mqa {

#ifndef RTSD_MAX_VLANMPLS_IDS
#define RTSD_MAX_VLANMPLS_IDS 8
#endif

    class StatsTransInfo
    {
    public:
        StatsTransInfo()
        {
            Reset();
        }

        void Reset()
        {
            nTransProto = 0;
            nSrcPort = nDestPort = 0;
            nTransOffset = 0;
            nTransLength = 0;
        }

        UINT8 nTransProto;
        UINT16 nSrcPort;
        UINT16 nDestPort;
        UINT16 nTransOffset;
        UINT8 nTransHeaderLength;
        UINT16 nTransLength; // Including transport header
    };

    class StatsIpInfo
    {
    public:
        StatsIpInfo()
        {
            Reset();
        }

        void Reset()
        {
            nFrameType = FRAME_TYPE_NONE;
            nIpType = LAYER_NONE;
            nIpFragment = FRAG_NONE;
            nIpOffset = 0;
        }

        UINT8 nFrameType;
        UINT8 nIpType;
        UINT8 nIpFragment;
        UINT16 nIpOffset;
        UINT16 nIpHeaderLength;
        UINT16 nIpLength;
    };

    class StatsIpTransInfo
    {
    public:
        StatsIpTransInfo()
        {
            Reset();
        }

        void Reset()
        {
            IpInfo.Reset();
            TransInfo.Reset();
        }

        StatsIpInfo IpInfo;
        StatsTransInfo TransInfo;
    };
    class MQA_SHARED StatsFrameParser
    {
    public:
        StatsFrameParser();
        ~StatsFrameParser();

        void Reset();
        bool ParseFrame(const UINT8* pFrame);  // RtsdBufFmtFrameHeader header
        bool ParseFrame(const UINT8* pFrame, UINT16 len, UINT8 limPort); // Ethernet header

        bool ParseVLANMPLSIds(UINT16& nOffset);
        bool ParseIpv4(UINT16& nOffset, bool bUpper);
        bool ParseIpv6(UINT16& nOffset, bool bUpper);
        bool ParseIp(UINT16& nOffset, bool bUpper);
        bool ParseTrans(UINT16& nOffset, bool bUpper);
        bool ParseGTP(UINT16& nOffset);
        bool ParseGRE(UINT16& nOffset);
        bool ParseUpper(UINT16& nOffset);
        bool ParseLower(UINT16& nOffset);

        inline bool IsIpv4(bool bUpper) const
        {
            return (bUpper ? (UpperInfo.IpInfo.nIpType == LAYER_IPV4) : (LowerInfo.IpInfo.nIpType == LAYER_IPV4));
        }
        inline const UINT8* IpPtr(bool bUpper) const
        {
            return (pData + (bUpper ? UpperInfo.IpInfo.nIpOffset : LowerInfo.IpInfo.nIpOffset));
        }
        inline UINT8 IpAddrOffset(bool bUpper) const
        {
            return ((bUpper ? UpperInfo.IpInfo.nIpOffset : LowerInfo.IpInfo.nIpOffset) + (IsIpv4(bUpper) ? 12 : 8));
        }
        inline UINT8 IpAddrLen(bool bUpper) const
        {
            return (IsIpv4(bUpper) ? 4 : 16);
        }
        inline const UINT8* SrcIp(bool bUpper) const
        {
            return (pData + IpAddrOffset(bUpper));
        }
        inline const UINT8* DestIp(bool bUpper) const
        {
            return (SrcIp(bUpper) + IpAddrLen(bUpper));
        }

        //---- get the real dest/source ip -------

        inline bool IsIpv4() const
        {
            return (nTunnelType==LAYER_NONE? (LowerInfo.IpInfo.nIpType == LAYER_IPV4) : (UpperInfo.IpInfo.nIpType == LAYER_IPV4));
        }
        inline const UINT8* IpPtr() const
        {
            return (pData + (nTunnelType==LAYER_NONE ? LowerInfo.IpInfo.nIpOffset : UpperInfo.IpInfo.nIpOffset));
        }
        inline UINT8 IpAddrOffset() const
        {
            return ((nTunnelType==LAYER_NONE ? LowerInfo.IpInfo.nIpOffset : UpperInfo.IpInfo.nIpOffset) + (IsIpv4() ? 12 : 8));
        }
        inline UINT8 IpAddrLen() const
        {
            return (IsIpv4() ? 4 : 16);
        }
        inline const UINT8* SrcIp() const
        {
            return (pData + IpAddrOffset());
        }
        inline const UINT8* DestIp() const
        {
            return SrcIp() + IpAddrLen();
        }
        inline StatsIpTransInfo& IpTransInfo() {
            return nTunnelType==LAYER_NONE? LowerInfo:UpperInfo;
        }
        inline const UINT8 *GetTransPayload(UINT32& length) {
            length = IpTransInfo().TransInfo.nTransLength - IpTransInfo().TransInfo.nTransHeaderLength;
            return pData + IpTransInfo().TransInfo.nTransOffset + IpTransInfo().TransInfo.nTransHeaderLength;
        }

        const UINT8* pData;
        UINT16 nDataLength;
        UINT16 nAppLayerOffset;

        UINT8 nLimPort;         // LIM port number of classified traffic
        UINT8 nTunnelType;      // IP and Tunnel type LAYER_GRE, LAYER_GTP or LAYER_NONE

        UINT8 nVLAN;
        UINT8 nMPLS;
        UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS];

        UINT32 nTEI;            // Tunnel identifier for GTP/GRE, or 0 non-tunnel

        StatsIpTransInfo LowerInfo;
        StatsIpTransInfo UpperInfo;
        timeval          nTimestamp;
    };
} // namespace mqa

#endif // MQA_STATSPARSER_H_
