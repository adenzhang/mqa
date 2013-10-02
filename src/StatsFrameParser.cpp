
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#include "StatsFrameParser.h"

#include "RtsdSupport.h"

namespace mqa {
StatsFrameParser::StatsFrameParser()
{
    Reset();
}

StatsFrameParser::~StatsFrameParser()
{
}

void StatsFrameParser::Reset()
{
    pData = NULL;
    nDataLength = 0;
    nLimPort = 0;
    nTunnelType = LAYER_NONE;
    nTEI = 0;
    nVLAN = nMPLS = 0; // aVLANMPLSIds is not cleared as it uses nVLAN+nMPLS as valid indexes
    LowerInfo.Reset();
    UpperInfo.Reset();
}

bool StatsFrameParser::ParseVLANMPLSIds(UINT16& nOffset)
{
    bool bButtomMplsStack = false;
    UINT16 uType;

    while (nOffset < nDataLength)
    {
        uType = *((UINT16*)(pData + nOffset));

        switch (uType)
        {
        case ETHER_PROTO_VLAN_SWAP:
            // 802.1Q VLAN: 0x8100 reversed
            {
                if (nVLAN + nMPLS < RTSD_MAX_VLANMPLS_IDS)
                {
                    aVLANMPLSIds[nVLAN+nMPLS] = (((pData[nOffset + 2] & 0x0F) << 8) + pData[nOffset + 3]) | RTSD_VLAN_FLAG;
                    ++nVLAN;
                }
                nOffset += 4;
                break;
            }
        case ETHER_PROTO_MPLS_UNICAST_SWAP:
        case ETHER_PROTO_MPLS_MULTICAST_SWAP:
            // MPLS: 0x8847 and 0x8848 reserved
            {
                nOffset += 2;
                bButtomMplsStack = true;
                do {
                    if (nVLAN + nMPLS < RTSD_MAX_VLANMPLS_IDS)
                    {
                        aVLANMPLSIds[nVLAN+nMPLS] = ((pData[nOffset] << 12) + (pData[nOffset + 1] << 4)
                            + ((pData[nOffset + 2] & 0xf0) >> 4)) | RTSD_MPLS_FLAG;
                        ++nMPLS;
                    }
                    bButtomMplsStack = pData[nOffset + 2] & 0x01;
                    nOffset += 4;                
                } while (!bButtomMplsStack);

                /* we have two types of MPLS frames:
                1. MAC address -> EtherType (MPLS) -> MPLS headers -> IP header 
                2. MAC address -> EtherType (MPLS) -> MPLS headers -> MAC address -> EtherType (0x0800) -> IP header 
                So after reaching bottom of the MPLS stack,
                - Type 1 is identified by the first octet of the IP header 0x0045.
                - Type 2 is identified otherwise and next EtherType needs to be evaluated.
                */
                UINT8 nIpType = pData[nOffset] & 0xF0;
                if (nIpType == ETHER_PROTO_IPV4_HEADER_VER
                    || nIpType == ETHER_PROTO_IPV6_HEADER_VER)
                {
                    // type 1 MPLS here, break out
                    return true;
                }
                else
                {
                    // type 2 MPLS with embedded MAC address advance past MAC address and check EtherType in next loop.
                    nOffset += 12; 
                    break;
                }
            }
        case ETHER_PROTO_IPV4_SWAP:
        case ETHER_PROTO_IPV6_SWAP:
            {
                nOffset += 2;
                return true;
            }
        default:
            // break out if it's neither VLAN nor MPLS nor IPv4/IPv6 (like ARP 0x0806)
            {
                LowerInfo.IpInfo.nFrameType = FRAME_TYPE_NON_IP;
                return false; 
            }
        }
    }

    return true;
}

bool StatsFrameParser::ParseIpv4(UINT16& nOffset, bool bUpper)
{
    StatsIpTransInfo& Info = (bUpper ? UpperInfo : LowerInfo);
    Info.IpInfo.nIpOffset = nOffset;
    Info.IpInfo.nFrameType = FRAME_TYPE_IPV4_OTHER;
    Info.IpInfo.nIpType = LAYER_IPV4;

    // Check fragment
    // TODO: Currently ignore fragment ip packets, need handle them in the future
    UINT16 FragAndFlags = ntohs(*((UINT16*)(pData + nOffset + 6)));
    UINT16 FragMF = FragAndFlags & FRAGMENT_MF_BITMASK;
    UINT16 FragOffset = FragAndFlags & FRAGMENT_OFFSET_BITMASK;
    if (FragMF != 0 && FragOffset != 0)
    {
        if (FragOffset == 0)
            Info.IpInfo.nIpFragment = FRAG_FIRST;
        else if (FragMF == 0)
            Info.IpInfo.nIpFragment = FRAG_LAST;
        else
            Info.IpInfo.nIpFragment = FRAG_NORMAL;
        return false;
    }

    // Get IP header length
    UINT8 nIpHeaderLength = (pData[nOffset] & 0x0F) << 2;
    UINT16 nIpLength = ntohs(*((UINT16*)(pData + nOffset + 2))); // Including IP header

    // Get Transport related info
    Info.TransInfo.nTransOffset = nOffset + nIpHeaderLength;
    Info.TransInfo.nTransProto = pData[nOffset + 9];
    Info.TransInfo.nTransLength = nIpLength - nIpHeaderLength;

    nOffset += nIpHeaderLength;

    return true;
}

bool StatsFrameParser::ParseIpv6(UINT16& nOffset, bool bUpper)
{
    StatsIpTransInfo& Info = (bUpper ? UpperInfo : LowerInfo);
    Info.IpInfo.nIpOffset = nOffset;
    Info.IpInfo.nFrameType = FRAME_TYPE_IPV6_OTHER;
    Info.IpInfo.nIpType = LAYER_IPV6;

    // Traverse through all extension headers, if exist.

    // IPv6 Header:
    // Bit   0 -   3 -> Version
    // Bit   4 -  11 -> Traffic Class
    // Bit  12 -  31 -> Flow Label
    // Bit  32 -  47 -> Payload Length
    // Bit  48 -  55 -> Next Header (equivalent to IPv4 protocol field)
    // Bit  56 -  63 -> Hop Limit
    // Bit  64 - 191 -> Source Address
    // Bit 192 - 319 -> Destination Address

    // Get the next header ID
    UINT8 uProtocol = pData[nOffset + 6];
    // Get to the beginning of next header
    const UINT8* pCurr = pData + nOffset + IPV6_HDR_LEN;

    // Traverse through all extension headers, if exist.
    for (bool bIsIPv6Ext = true; (false != bIsIPv6Ext); )
    {
        switch (uProtocol)
        {
        case IPv6_HDR_HOP_BY_HOP:
        case IPV6_HDR_ROUTING:
        case IPV6_HDR_DESTINATION:
            // RFC 2460
            //
            // Hop-by-Hop Options Header:
            // When presence, must immediately follow the IPv6 header.
            // Bit  0 -   7 -> Next Header
            // Bit  8 -  15 -> Header Extension Length in 8-octet units, not including the first 8 octets.
            // Bit 16 - ... -> Options
            //
            // Routing Header:
            // Bit  0 -   7 -> Next Header
            // Bit  8 -  15 -> Header Extension Length in 8-octet units, not including the first 8 octets.
            // Bit 16 -  23 -> Routing Type
            // Bit 24 -  31 -> Segments Left
            // Bit 32 - ... -> Type-specific Data
            //
            // Destination Options Header:
            // Bit  0 -   7 -> Next Header
            // Bit  8 -  15 -> Header Extension Length in 8-octet units, not including the first 8 octets.
            // Bit 16 - ... -> Options

            // Get the next header ID
            uProtocol = *pCurr;
            // Get to the beginning of next header
            pCurr += (((*(pCurr + 1)) + 1) * 8);

            break;
        case IPV6_HDR_FRAGMENT:
            {
                // RFC 2460
                //
                // Fragment Header:
                // Bit  0 -  7 -> Next Header
                // Bit  8 - 15 -> Reserved
                // Bit 16 - 28 -> Fragment Offset
                // Bit 29 - 30 -> Reserved
                // Bit      31 -> 1 = More Fragments; 0 = Last Fragment
                // Bit 32 - 63 -> Identification

                // Check fragment
                // TODO: Currently ignore fragment ip packets, need handle them in the future
                UINT16 FragAndFlags = ntohs(*((UINT16*)(pCurr + 2)));
                UINT16 FragMF = FragAndFlags & 0x0001;
                UINT16 FragOffset = FragAndFlags & 0xFFF8;
                if (FragMF != 0 && FragOffset != 0)
                {
                    if (FragOffset == 0)
                        Info.IpInfo.nIpFragment = FRAG_FIRST;
                    else if (FragMF == 0)
                        Info.IpInfo.nIpFragment = FRAG_LAST;
                    else
                        Info.IpInfo.nIpFragment = FRAG_NORMAL;
                    return false;
                }

                // Get the next header ID
                uProtocol = *pCurr;
                // Get to the beginning of next header
                pCurr += 8;
                break;
            }
        case IPV6_HDR_ESP:
            // RFC 2406
            //
            // ray - Don't know how to handle this one!
            bIsIPv6Ext = false;

            // Get the next header ID
            uProtocol = *pCurr;
            // Get to the beginning of next header
            pCurr += (((*(pCurr + 1)) + 1) * 8);
            break;
        case IPV6_HDR_AH:
            // RFC 2402
            //
            // IP Authentication Header:
            // Bit  0 -   7 -> Next Header
            // Bit  8 -  15 -> Payload Len in 4-octet units minus "2"
            // Bit 16 -  31 -> Reserved
            // Bit 32 -  63 -> Security Parameters Index (SPI)
            // Bit 64 -  95 -> Sequence Number Field
            // Bit 96 - ... -> Authentication Data

            // Get the next header ID
            uProtocol = *pCurr;
            // Get to the beginning of next header
            pCurr += (((*(pCurr + 1)) + 1) * 8);
            break;
        default:
            return false;
        }
    }

    // Get IP header length
    UINT8 nIpHeaderLength = pCurr - (pData + nOffset);
    UINT16 nIpLength = ntohs(*((UINT16*)(pData + nOffset + 4))) + IPV6_HDR_LEN; // Including IP header

    // Get Transport related info
    Info.TransInfo.nTransOffset = nOffset + nIpHeaderLength;
    Info.TransInfo.nTransProto = uProtocol;
    Info.TransInfo.nTransLength = nIpLength - nIpHeaderLength;

    nOffset += nIpHeaderLength;

    return true;
}

bool StatsFrameParser::ParseIp(UINT16& nOffset, bool bUpper)
{
    UINT8 nIpType = pData[nOffset] & 0xF0;
    if (nIpType == ETHER_PROTO_IPV4_HEADER_VER)
        return ParseIpv4(nOffset, bUpper);
    else if (nIpType == ETHER_PROTO_IPV6_HEADER_VER)
        return ParseIpv6(nOffset, bUpper);
    else
        return false;
}

bool StatsFrameParser::ParseTrans(UINT16& nOffset, bool bUpper)
{
    StatsIpTransInfo& Info = (bUpper ? UpperInfo : LowerInfo);
    // Check for trans proto
    switch (Info.TransInfo.nTransProto)
    {
    case IPV4_PROTO_UDP:
        {
            Info.TransInfo.nSrcPort = *(UINT16*)(pData + nOffset);
            Info.TransInfo.nDestPort = *(UINT16*)(pData + nOffset + 2);
            if (!bUpper
                && (Info.TransInfo.nSrcPort == UDP_PORT_GTPU_SWAP
                || Info.TransInfo.nDestPort == UDP_PORT_GTPU_SWAP))
            {
                if (!ParseGTP(nOffset))
                    return false;
            }else{
                nOffset += 8; // to payload
            }
            break;
        }
    case IPV4_PROTO_GRE:
        {
            if (bUpper || !ParseGRE(nOffset))
                return false;
            break;
        }
    case IPV4_PROTO_TCP:
        {
            Info.TransInfo.nSrcPort = *(UINT16*)(pData + nOffset);
            Info.TransInfo.nDestPort = *(UINT16*)(pData + nOffset + 2);
            UINT8 dataOffset = nOffset + 12;
            nOffset += dataOffset;
            break;
        }
    case IPV4_PROTO_SCTP:
        {
            Info.TransInfo.nSrcPort = *(UINT16*)(pData + nOffset);
            Info.TransInfo.nDestPort = *(UINT16*)(pData + nOffset + 2);
            nOffset += 8;
            break;
        }
    default:
        return false;
    }

    return true;
}

bool StatsFrameParser::ParseGTP(UINT16& nOffset)
{
    nTunnelType = LAYER_GTP;

    // GTP Version 1, PT=1, E=0,S=0,PN=0
    // GTP Message type Type = GPDU
    if (((pData[nOffset] & 0xF0) == 0x30) && (pData[nOffset + 1] == GTPU_MT_GPDU))
    {
        LowerInfo.IpInfo.nFrameType = FRAME_TYPE_GTPU;
        nTEI = ntohl(*((UINT32*)(pData + nOffset + 4)));
        UpperInfo.IpInfo.nIpOffset = nOffset + 8;

        if (pData[nOffset] & 0x0F) // if any of the E.S.PN bits is set
        {
            UpperInfo.IpInfo.nIpOffset += 4;  // add 4 octets to ipUpperOffset (seq no = 16bits, n-pdu = 8bits, next ext = 8bits)

            if (pData[nOffset] & 0x04) // if next ext header exist
            {
                while (pData[UpperInfo.IpInfo.nIpOffset - 1]) // while the next extension header is not zero
                {   
                    UINT16 extLen = (pData[UpperInfo.IpInfo.nIpOffset] * 4);
                    if ((UINT16)(UpperInfo.IpInfo.nIpOffset + extLen) > 255 || extLen == 0) // prevent stuck
                        return false;   // overflow, possibly corrupted 
                    UpperInfo.IpInfo.nIpOffset += extLen; // increment the offset by extension length (unit of 4-octets).
                }
            }
        }

        nOffset = UpperInfo.IpInfo.nIpOffset;
        if (!ParseUpper(nOffset))
            return false;
    }
    // Special GTP msg type - inject by network, for signaling
    // GtpV1, End Marker = 0xFE
    else if (((pData[nOffset] & 0xF0) == 0x30)
        && ((pData[nOffset + 1] == GTPU_MT_ENDMARKER) || (pData[nOffset + 1] == GTPU_MT_ERROR_INDICATION)))
    {
        LowerInfo.IpInfo.nFrameType = FRAME_TYPE_GTPC;
    }
    return true;
}

bool StatsFrameParser::ParseGRE(UINT16& nOffset)
{
    nTunnelType = LAYER_GRE;
    LowerInfo.IpInfo.nFrameType = FRAME_TYPE_GRE;

    // Parse GRE header
    UINT32 nNextOffset = nOffset + 4; // skip past flags, version, & protocol Type
    if (pData[nOffset] & 0xC0) // Checksum or Routing bits - 4 bytes are added before the TEI.
        nNextOffset += 4;

    if (pData[nOffset] & 0x20)
    {
        nTEI = ntohl(*(UINT32*)(pData + nNextOffset));
        nNextOffset += 4;
    }
    else
        nTEI = 0;

    if (pData[nOffset] & 0x10) // sequence number bit set - 4 byte field after TEI
        nNextOffset += 4;
    if (pData[nOffset] & 0x40) // routing bit indicates additional SRE fields
    {
        // TODO - handle GRE Source Routing Entries
        // for now just return that it's GRE and don't parse above.
        return false;
    }

    UpperInfo.IpInfo.nIpOffset = nOffset = nNextOffset;
    if (!ParseUpper(nOffset))
        return false;

    return true;
}

bool StatsFrameParser::ParseUpper(UINT16& nOffset)
{
    if (!ParseIp(nOffset, true))
        return false;

    if (!ParseTrans(nOffset, true))
        return false;

    return true;
}

bool StatsFrameParser::ParseLower(UINT16& nOffset)
{
    if (!ParseIp(nOffset, false))
        return false;

    if (!ParseTrans(nOffset, false))
        return false;

    return true;
}

bool StatsFrameParser::ParseFrame(const UINT8 *pFrame)
{
    if (!pFrame)
        return false;

    Reset();

    pData = pFrame + (UINT8) sizeof(RtsdBufFmtFrameHeader);
    RtsdBufFmtFrameHeader *pHeader = (RtsdBufFmtFrameHeader *)pFrame;
    nDataLength = pHeader->storedLength; 

    // Get LIM port number
    if (pHeader->limInfoLength == 3 && !pHeader->isPPFrame)
        nLimPort = pHeader->limInfo[0] & 0x1F;

    UINT16 nOffset = 12;   // skip Ethernet addresses
    nAppLayerOffset = 0;

    // Get VLAN/MPLS Ids
    if (!ParseVLANMPLSIds(nOffset))
        return false;

    if (!ParseLower(nOffset))
        return false;

    nAppLayerOffset = nOffset;
    return true;
}
bool StatsFrameParser::ParseFrame(const UINT8* pFrame, UINT16 len, UINT8 limPort) // ethernet header
{
    pData = pFrame;
    nDataLength = len;
    nLimPort = limPort;

    UINT16 nOffset = 0;   // no skip Ethernet addresses
    nAppLayerOffset = 0;

    // Get VLAN/MPLS Ids
    if (!ParseVLANMPLSIds(nOffset))
        return false;

    if (!ParseLower(nOffset))
        return false;

    nAppLayerOffset = nOffset;
    return true;
}
} // namespace mqa