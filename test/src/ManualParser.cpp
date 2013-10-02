#include "ManualParser.h"
#include <iostream>
#include <hash_map>
using namespace std;

///////////////////////////////// Ethernet Parse /////////////////////////////////////

#define CU_ETHERNET_TYPE_8021Q  0x8100
#define CU_ETHERNET_TYPE_8023   0x0800
#define CU_ETHERNET_TYPE_IPv4   0x0800
#define CU_ETHERNET_TYPE_IPv6   0x86DD
#define CU_ETHERNET_TYPE_MPLS_UNICAST   0x8847
#define CU_ETHERNET_TYPE_MPLS_MULTICAST 0x8848
#define CU_ETHERNET_TYPE_PPPoE_SESSION_STAGE 0x8864
#define CU_ETHERNET_LENGTH_TYPE_POSN    12
#define CU_ETHERNET_802_3_HEADER_LEN    14
#define CU_ETHERNET_802_3_FCS_LEN       4

#define CU_PPP_PROTOCOL_ID_IPv4 0x21
#define CU_PPP_PROTOCOL_ID_IPv6 0x57
#define CU_PPP_PROTOCOL_ID_MPLS_UNICAST 0x281
#define CU_PPP_PROTOCOL_ID_MPLS_MULTICAST 0x283

const unsigned long ethernet_oic_mac_address[] = {
    0x401597, // Protect America, Inc.
    0x406186, // MICRO-STAR INT'L CO.,LTD
    0x4097D1, // BK Electronics cc
    0x40A6A4, // PassivSystems Ltd
    0x40ECF8, // Siemens AG
    0x40EF4C, // Fihonest communication co.,Ltd
    0x40F52E, // Leica Microsystems (Schweiz) AG
    0x44568D, // PNC Technologies  Co., Ltd.
    0x4456B7, // Spawn Labs, Inc
    0x44C9A2, // Greenwald Industries
    0x48343D, // IEP GmbH
    0x60380E, // Alps Electric Co.,
    0x60391F, // ABB Ltd
    0x609F9D, // CloudSwitch
    0x60D0A9, // Samsung Electronics Co.,Ltd
    0x60D30A, // Quatius Limited
    0x60F13D, // JABLOCOM s.r.o.
    0x60FB42, // Apple, Inc
    0x6416F0, // Shehzhen Huawei Communication Technologies Co., Ltd.
    0x644BC3, // Shanghai WOASiS Telecommunications Ltd., Co.
    0x644F74, // LENUS Co., Ltd.
    0x6465C0, // Nuvon, Inc
    0x64680C, // COMTREND
    0x647D81, // YOKOTA INDUSTRIAL CO,.LTD
    0x64A837, // Juni Korea Co., Ltd
    0x64BC11, // CombiQ AB
    0x64C6AF, // AXERRA Networks Ltd
    0x64D4DA, // Intel Corporate
    0x64ED57, // Motorola MDb/Broadband
    0x688540, // IGI Mobile, Inc.
    0x68A1B7, // Honghao Mingchuan Technology (Beijing) CO.,Ltd.
    0x68AAD2, // DATECS LTD.,
    0x68CC9C, // Mine Site Technologies
    0x6C0E0D, // Sony Ericsson Mobile Communications AB
    0x6C0F6A, // JDC Tech Co., Ltd.
    0x6C8CDB, // Otus Technologies Ltd
    0x6CAC60, // Venetex Corp
    0x6CF049, // GIGA-BYTE TECHNOLOGY CO.,LTD.
    0x000000 };

uint16_t cu_CalcVLANOffset(const uint8_t *pData,uint16_t *uType, uint32_t nRequestVLANCount = 0, uint16_t* pnVLANID = NULL)
{
    uint16_t uOffset=0;
    uint32_t   nVLANCount = 0;

    *uType = (((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN])<<8)+((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN+1]);
    if((*uType)!=CU_ETHERNET_TYPE_8021Q)
        return 0;

    while((*uType)==CU_ETHERNET_TYPE_8021Q)
    {
        uOffset+=4;
        *uType = (((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN+uOffset])<<8)+
            ((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN+1+uOffset]);

        if(nVLANCount < nRequestVLANCount &&
            pnVLANID)
        {
            pnVLANID[nVLANCount] = (((uint16_t)(pData[CU_ETHERNET_LENGTH_TYPE_POSN+uOffset - 2]&0x0f))<<8)+
                ((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN+1+uOffset - 2]);
            nVLANCount++;
        }
    }
    return uOffset;
}

uint16_t cu_CalcPPPoEOffset(const uint8_t *pData,uint16_t *uType,uint16_t* pnPPPoEID = NULL)
{
    uint16_t uOffset=0;

    *uType = (((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN])<<8)+((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN+1]);

    // Ignore Discovery stage
    if ( (*uType)==CU_ETHERNET_TYPE_PPPoE_SESSION_STAGE  )
    {
        uOffset += 8; // for the PPPoE part, and the EtherType

        if(pnPPPoEID)
        {
            *pnPPPoEID = ( ( (uint16_t) pData[uOffset + CU_ETHERNET_LENGTH_TYPE_POSN - 4])<<8) +
                ( (uint16_t) pData[uOffset + CU_ETHERNET_LENGTH_TYPE_POSN + 1 - 4]);
        }

        // ISO 3309 style protocol (all protocols must be odd)
        // Can be 8 or 16 bits, depending on the value of the first
        // byte.
        if ( pData[uOffset + CU_ETHERNET_LENGTH_TYPE_POSN] & 0x01 )
        {
            // set uType to the PPP Protocol ID
            *uType = (uint16_t) pData[uOffset + CU_ETHERNET_LENGTH_TYPE_POSN];
            ++uOffset;
        }
        else
        {
            // set uType to the PPP Protocol ID
            *uType = ( ( (uint16_t) pData[uOffset + CU_ETHERNET_LENGTH_TYPE_POSN])<<8)
                +( (uint16_t) pData[uOffset + CU_ETHERNET_LENGTH_TYPE_POSN + 1]);
            uOffset += 2;
        }

        // If this is LCP, don't skip.
        if ( *uType & 0xc000 )
        {
            return 0;
        }

        // Convert the PPP Protocol ID (http://www.iana.org/assignments/ppp-numbers)
        // to an ethertype number (http://www.iana.org/assignments/ethernet-numbers)
        switch(*uType)
        {
        case CU_PPP_PROTOCOL_ID_IPv4: // IP
            *uType = CU_ETHERNET_TYPE_8023;
            break;
        case CU_PPP_PROTOCOL_ID_IPv6: // IP
            *uType = CU_ETHERNET_TYPE_IPv6;
            break;
        case CU_PPP_PROTOCOL_ID_MPLS_UNICAST: // MPLS unicast
            *uType = CU_ETHERNET_TYPE_MPLS_UNICAST;
            break;
        case CU_PPP_PROTOCOL_ID_MPLS_MULTICAST: // MPLS unicast
            *uType = CU_ETHERNET_TYPE_MPLS_MULTICAST;
            break;
        }
        // Roll back 2 (as we added 2 extra for the EtherType)
        uOffset-=2;
    }

    return uOffset;
}

// If PPPoE is found, this function will return true, and set pStartOfPPPoE to point to the
// start of the EtherType tag (0x8864)
bool cu_IsPPPoEPresent(uint8_t *pData,uint8_t **pStartOfPPPoE)
{
    uint16_t uType;
    uint16_t nOffset = cu_CalcVLANOffset( pData, &uType );
    uint16_t nPPPoEOffset = cu_CalcPPPoEOffset( pData + nOffset, &uType );
    if (nPPPoEOffset)
    {
        *pStartOfPPPoE = pData + CU_ETHERNET_LENGTH_TYPE_POSN + nOffset;
        return true;
    }
    return false;
}

struct CEthernetManufacturersMacList {
    CEthernetManufacturersMacList() {
        int index=0;
        while (0x000000 != ethernet_oic_mac_address[index])
        {
            mapMacs.insert(
                std::make_pair< unsigned long, unsigned long >(
                ethernet_oic_mac_address[index] , 
                ethernet_oic_mac_address[index] )
                );
            ++index;
        }
    }; 
    bool IsValidAssignedMaunfacturerMac( unsigned long first_three_bytes ) const {
        return mapMacs.end() != mapMacs.find( first_three_bytes );
    }
    stdext::hash_map< unsigned long , unsigned long > mapMacs;
};

uint16_t cu_CalcMPLSOffset(const uint8_t *pData,uint16_t *uType, bool uTypeValid = false, uint32_t nRequestMPLSCount = 0, uint32_t* pnMPLSID = NULL)
{
    uint32_t uOffset=0;
    uint32_t nMPLSCount = 0;

    if (false == uTypeValid)
    {
        *uType = (((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN])<<8)+((uint16_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN+1]);
    }

    if ( *uType ==CU_ETHERNET_TYPE_MPLS_UNICAST
        || *uType ==CU_ETHERNET_TYPE_MPLS_MULTICAST )
    {
        static const CEthernetManufacturersMacList g_EtherMacList;
        uOffset += 4; // skip this MPLS tag ( type(2) + label(2) )

        // Walk to the bottom of the stack.  S=1: last tag in stack.
        //
        // 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ Label
        //|                Label                  | Exp |S|       TTL     | Stack
        //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ Entry
        //                    Label:  Label Value, 20 bits
        //                    Exp:    Experimental Use, 3 bits
        //                    S:      Bottom of Stack, 1 bit
        //                    TTL:    Time to Live, 8 bits

        while ( 0 == ( pData[ CU_ETHERNET_LENGTH_TYPE_POSN + uOffset ] & 0x01 ) )  // Check the S bit
        {
            if(nMPLSCount < nRequestMPLSCount && pnMPLSID)
            {
                pnMPLSID[nMPLSCount] = ((((uint32_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset - 2]) << 16) +
                    (((uint32_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset+1 - 2]) << 8) +
                    ((uint32_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset+2 - 2])) >> 4;
                nMPLSCount++;
            }
            uOffset += 4;
        }

        // Record last item.
        if(nMPLSCount < nRequestMPLSCount && pnMPLSID)
        {
            // Label
            pnMPLSID[nMPLSCount] = ((((uint32_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset - 2]) << 16) +
                (((uint32_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset+1 - 2]) << 8) +
                ((uint32_t)pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset+2 - 2])) >> 4;
            nMPLSCount++;
        }

        // Auto-detect next layer.  Current protocols supported:
        //   IPv4, IPv6, Ethernet
        // Ethernet is selected when:
        //  - first nibble isn't 4 or 6, OR
        //  - first nibble is 4 or 6, and frist three bytes matches a manufacturer code
        //     (This is the first three bytes of the source MAC address)
        unsigned long nFirstThreeBytes = ( ( pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset + 2 ] ) << 16)
            |  ( ( pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset + 3 ] ) << 8)
            |  ( ( pData[CU_ETHERNET_LENGTH_TYPE_POSN + uOffset + 4 ] ) );

        // TODO: Add support for detection of PPP
        // This is really difficult, because without external configuration or information,
        // it's impossible to differentiate between a valid ethernet MAC Dest and a Pseudo-Wire MPLS control word

        unsigned char nVersionNibble = ( ( pData[ CU_ETHERNET_LENGTH_TYPE_POSN + uOffset + 2 ] & 0xf0 ) >> 4);
        bool bEthernet=false;
        if (nVersionNibble == 6 || nVersionNibble == 4)
        {
            if (g_EtherMacList.IsValidAssignedMaunfacturerMac( nFirstThreeBytes ))
            {
                bEthernet = true;
            }
        } else {
            bEthernet = true;
        }

        if (bEthernet)
        {
            uOffset += 14; // Exp+S(1) + TTL (1) + Ether (12)
            uOffset += cu_CalcVLANOffset(&pData [ uOffset ], uType); // This should start parsing from Ethernet header in MPLS
        }
        // Assume IP
        else if ( 6 == ( ( pData[ CU_ETHERNET_LENGTH_TYPE_POSN + uOffset + 2 ] & 0xf0 ) >> 4) )
        {
            *uType = CU_ETHERNET_TYPE_IPv6;
        } 
        else 
        {
            *uType = CU_ETHERNET_TYPE_IPv4;
        }
    }
    return (uint16_t)uOffset;
}

extern "C" __declspec( dllexport )uint8_t cu_EtherExtraLengthOffsetAndL2Info(const uint8_t *pData, uint16_t *uType,
                                                                           uint32_t nRequestVLANCount, uint16_t* pnVLANID, 
                                                                           uint16_t* pnPPPoEID, 
                                                                           uint32_t nRequestMPLSCount, uint32_t* pnMPLSID)
{
    // Ethernet Frame Formats:
    // Ethernet (a.k.a. Ethernet II)
    //         +---------+---------+---------+----------
    //         |   Dst   |   Src   |  Type   |  Data... 
    //         +---------+---------+---------+----------
    //          <-- 6 --> <-- 6 --> <-- 2 --> <-46-1500->
    //          Type 0x80 0x00 = TCP/IP
    //          Type 0x06 0x00 = XNS
    //          Type 0x81 0x37 = Novell NetWare
    //          
    // 802.3
    //         +---------+---------+---------+----------
    //         |   Dst   |   Src   | Length  | Data...  
    //         +---------+---------+---------+----------
    //          <-- 6 --> <-- 6 --> <-- 2 --> <-46-1500->
    // 802.2 (802.3 with 802.2 header)
    //         +---------+---------+---------+-------+-------+-------+----------
    //         |   Dst   |   Src   | Length  | DSAP  | SSAP  |Control| Data...  
    //         +---------+---------+---------+-------+-------+-------+----------
    //                                        <- 1 -> <- 1 -> <- 1 -> <-43-1497->
    // SNAP (802.3 with 802.2 and SNAP headers) 
    //         +---------+---------+---------+-------+-------+-------+-----------+---------+-----------
    //         |   Dst   |   Src   | Length  | 0xAA  | 0xAA  | 0x03  |  Org Code |   Type  | Data...   
    //         +---------+---------+---------+-------+-------+-------+-----------+---------+-----------
    //                                                                <--  3  --> <-- 2 --> <-38-1492->
    //

    int nOffset = 0;

    *uType = 0;

    // Assumptions:
    //   PPPoE can't appear over MPLS
    //   MPLS will always return IPv4 for it's uType

    int nThisOffset = cu_CalcVLANOffset(pData, uType, nRequestVLANCount, pnVLANID);
    pData += nThisOffset;
    nOffset += nThisOffset;

    nThisOffset = cu_CalcPPPoEOffset(pData, uType, pnPPPoEID );

    pData += nThisOffset;
    nOffset += nThisOffset;

    // If PPPoE exists, the uType has been rewritten from a PPP Protocol ID
    bool bMatch = 0 != nThisOffset;

    // bMatch indicates that uType was set during PPPoE,
    // and that it shouldn't be recalculated by the function.
    nThisOffset = cu_CalcMPLSOffset(pData, uType, bMatch, nRequestMPLSCount, pnMPLSID);

    nOffset += nThisOffset;
    return nOffset;
}

extern "C" __declspec( dllexport )uint8_t cu_EtherExtraLengthOffset(const uint8_t *pData, uint16_t *uType)
{
    return cu_EtherExtraLengthOffsetAndL2Info(pData, uType, 0, NULL, NULL, 0, NULL);
}

///////////////////////////////// IP Parse /////////////////////////////////////

// IP version
#define IPv4_VERSION    4
#define IPv6_VERSION    6

// IP Header Lengths (bytes)
#define IPv4_HEADER_LEN     20
#define IPv6_HEADER_LEN     40

// IPv4 Protocol Numbers
#define IPv4_PROTOCOL_TCP   6
#define IPv4_PROTOCOL_UDP   17

// IPv4 Offsets (bytes)
#define IPv4_VERSION_OFFSET                      0
#define IPv4_HEADER_LENGTH_OFFSET                0
#define IPv4_DIFFERENTIATED_SERVICES_OFFSET      1
#define IPv4_TOTAL_LENGTH_OFFSET                 2
#define IPv4_IDENTIFICATION_OFFSET               4
#define IPv4_FLAGS_OFFSET                        6
#define IPv4_FRAGMENT_OFFSET_OFFSET              6
#define IPv4_TIME_TO_LIVE_OFFSET                 8
#define IPv4_PROTOCOL_OFFSET                     9
#define IPv4_HEADER_CHECKSUM_OFFSET             10
#define IPv4_SOURCE_ADDRESS_OFFSET              12
#define IPv4_DESTINATION_ADDRESS_OFFSET         16
#define IPv4_CHECKSUM_OFFSET                    28
#define IPv4_SEQUENCE_NUMBER_OFFSET             120
#define IPv4_SGI_SEQUENCE_NUMBER_OFFSET         84

// IPv6 Offsets (bytes)
#define IPv6_VERSION_OFFSET                      0
#define IPv6_TRAFFIC_CLASS_OFFSET                0
#define IPv6_FLOW_LABEL_OFFSET                   1
#define IPv6_PAYLOAD_LENGTH_OFFSET               4
#define IPv6_NEXT_HEADER_OFFSET                  6
#define IPv6_HOP_LIMIT_OFFSET                    7
#define IPv6_SOURCE_ADDRESS_OFFSET               8
#define IPv6_DESTINATION_ADDRESS_OFFSET         24
#define IPv6_CHECKSUM_OFFSET                    48

// IPv6 Extension Header IDs
#define IPv6_EXT_HOP_BY_HOP_OPTIONS_ID                   0
#define IPv6_EXT_ROUTING_ID                             43
#define IPv6_EXT_FRAGMENT_ID                            44
#define IPv6_EXT_DESTINATION_OPTIONS_ID                 60
#define IPv6_EXT_AUTHENTICATION_ID                      51
#define IPv6_EXT_ENCAPSULATING_SECURITY_PAYLOAD_ID      50

// IPv6 Extension Offsets (bytes)
#define IPv6_EXT_FRAGMENT_FRAGMENT_OFFSET_OFFSET     2
#define IPv6_EXT_FRAGMENT_M_FLAG_OFFSET              3
#define IPv6_EXT_FRAGMENT_IDENTIFICATION_OFFSET      4

// IPv6 Extension Header Lengths (bytes)
#define IPv6_EXT_FRAGMENT_LEN   8

// IPv6 number of segments in a standard ipv6 address
#define IPv6_NUM_SEGMENT                8

// UDP
#define UDP_HEADER_LEN                  8
#define UDP_SOURCE_PORT_OFFSET          0
#define UDP_DESTINATION_PORT_OFFSET     2
#define UDP_LENGTH_OFFSET               4
#define UDP_CHECKSUM_OFFSET             6

// TCP
#define TCP_HEADER_MIN_LEN              20
#define TCP_DATAOFFSET_OFFSET           12
#define TCP_CHECKSUM_OFFSET             16

// ATM
#define AAL5_HEADER_LEN     12

uint32_t cu_CopyOffsetBitArray(uint8_t *pInputData, 
                             uint32_t uInputStartBit, 
                             uint8_t *pOutputData, 
                             uint32_t uOutputStartBit, 
                             uint32_t uLengthInBits,
                             uint32_t uOutputBufferSize,
                             bool  *pTruncateIndicator)
{
    // Declare counters
    uint32_t uInputBitIndex = 0, uInputByteIndex = 0;
    uint32_t uOutputBitIndex = 0, uOutputByteIndex = 0;
    uint32_t uBitCounter = 0, uLengthInBitsToCopy;
    uint8_t  uBitValue = 0, uLengthBytes, uLengthBits;

    *pTruncateIndicator = false;

    // In case there is an exceptional error, where uLengthInBits is already
    // much greater than the uOutputBufferSize, adding the uOutputStartBit
    // would just cause a roll-over and we will not be able to catch this
    // exception. So we check uLengthInBits by itself first.
    if(uLengthInBits > uOutputBufferSize)
    {
        // Truncate the frame by copying up to uOutputBufferSize        
        // Mark Truncated
        *pTruncateIndicator = true;                    

        // Reduce Bits to Copy
        uLengthInBits = uOutputBufferSize - uOutputStartBit;
    }
    else
    {
        // 2nd check if total size, i.e. existing and new data to 
        // copy is greater than buffer size, truncate
        // Truncate if necessary
        uLengthInBitsToCopy = uOutputStartBit + uLengthInBits;

        if(uLengthInBitsToCopy > uOutputBufferSize)
        {
            // Mark Truncated
            *pTruncateIndicator = true;                    

            // Reduce Bits to Copy
            uLengthInBits = uLengthInBits - ((uLengthInBitsToCopy - uOutputBufferSize));
        }
    }
    uLengthBits  = uLengthInBits&0x7;

    // Check if the input and output are byte aligned, this the minimum we should
    // be doing to avoid wasting time... 16 is a selected optimization factor, which 
    // makes more sense to do bit copy as compared to memcpy...
    if(((uInputStartBit&0x7)==0)&&((uOutputStartBit&0x7)==0)&&(uLengthBits==0))
    {
        uLengthBytes = uLengthInBits>>3;

        if(uLengthBits>0)
        {
            uLengthBytes+=1;
        }

        memcpy(&pOutputData[uOutputStartBit>>3],&pInputData[uInputStartBit>>3],uLengthBytes);

        uBitCounter = uLengthInBits;
    }
    else
    {
        // Loop away
        while(uBitCounter < uLengthInBits)
        {
            // Work out which byte is being referenced by the bit number
            uInputByteIndex = (uInputStartBit + uBitCounter)>>3;
            uOutputByteIndex = (uOutputStartBit + uBitCounter)>>3;

            // Work out the bit position in the byte
            uInputBitIndex = 7- (uInputStartBit + uBitCounter)&0x7;
            uOutputBitIndex =7- (uOutputStartBit + uBitCounter)&0x7;

            // Work out bit value in the Input array
            uBitValue = (pInputData[uInputByteIndex] & (1<<uInputBitIndex));

            // Copy the bit from the InputData to the OutputData
            if(uBitValue) // Bit is one
                pOutputData[uOutputByteIndex] = (pOutputData[uOutputByteIndex] | (1<<uOutputBitIndex));
            else // Bit is zero
                pOutputData[uOutputByteIndex] = (pOutputData[uOutputByteIndex] & ~(1<<uOutputBitIndex));   

            // Increment BitCounter
            uBitCounter++;
        }
    }
    // Return number of bits copied
    return uBitCounter;
}

bool cu_ExtractBits(uint8_t *pData, uint32_t uFrameSize, 
                    uint32_t &nBitOffset, uint32_t uLengthInBits, 
                    uint8_t *pOutputBuffer, bool bReverseByte)
{
    bool bTruncateInd;
    uint32_t nOutputBufferSize = 0;
    uint32_t nNumberOfBitsCopied = 0;
    uint16_t nTemp16 = 0;
    uint32_t nTemp32 = 0;

    //--------------------------------------------------------------------
    //Performance improvement:
    //--------------------------------------------------------------------
    //
    //  Original Code:
    //
    //  if( (nBitOffset + uLengthInBits) > (unsigned)(uFrameSize * 8) )
    //      return false;
    //
    //--------------------------------------------------------------------
    //
    //  New Code:
    //
    if( (nBitOffset + uLengthInBits) > (uFrameSize << 3) )
        return false;
    //
    //--------------------------------------------------------------------

    if     (uLengthInBits <= 8)  nOutputBufferSize = 8;
    else if(uLengthInBits <= 16) nOutputBufferSize = 16;
    else                         nOutputBufferSize = 32;

    if(nOutputBufferSize < uLengthInBits)
        return false;

    memset(pOutputBuffer, 0, (nOutputBufferSize/8));

    switch(nOutputBufferSize)
    {
    case 8:
        nNumberOfBitsCopied = cu_CopyOffsetBitArray(pData, 
            nBitOffset, pOutputBuffer, nOutputBufferSize-uLengthInBits, 
            uLengthInBits, nOutputBufferSize, &bTruncateInd);
        break;
    case 16:
        nNumberOfBitsCopied = cu_CopyOffsetBitArray(pData, 
            nBitOffset, (uint8_t*)&nTemp16, nOutputBufferSize-uLengthInBits, 
            uLengthInBits, nOutputBufferSize, &bTruncateInd);
        if(bReverseByte)
        {
            nTemp16 = ((nTemp16 & 0xFF) << 8) + (nTemp16 >> 8);
        }
        memcpy(pOutputBuffer, (const void *)&nTemp16, 2);
        break;
    case 32:
        nNumberOfBitsCopied = cu_CopyOffsetBitArray(pData, 
            nBitOffset, (uint8_t*)&nTemp32, nOutputBufferSize-uLengthInBits, 
            uLengthInBits, nOutputBufferSize, &bTruncateInd);
        if(bReverseByte)
        {
            nTemp32 = ((nTemp32 & 0xFF) << 24) + ((nTemp32 & 0xFF00) << 8)
                + ((nTemp32 & 0xFF0000) >> 8) + (nTemp32 >> 24);
        }
        memcpy(pOutputBuffer, (const void *)&nTemp32, 4);
        break;
    }

    if(nNumberOfBitsCopied != uLengthInBits)
        return false;

    // Update bitoffset
    nBitOffset += uLengthInBits;

    return true;
}

bool cu_IpPacketIsIPv6(const uint8_t* pIpPacket)
{
    if (IPv6_VERSION == pIpPacket[0] >> 4)
        return true;
    return false;
}

uint16_t cu_IpPacketExtractIpHeaderLength(const uint8_t* pIpPacket)
{
    uint16_t uIpHeaderLen = 0;
    bool bIsIPv6 = cu_IpPacketIsIPv6(pIpPacket);
    if (false != bIsIPv6)
    {
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
        uint8_t uProtocol = pIpPacket[IPv6_NEXT_HEADER_OFFSET];
        // Get to the beginning of next header
        uint8_t* pCurr = const_cast<uint8_t*>(pIpPacket) + IPv6_HEADER_LEN;

        // Traverse thru all extension headers, if exist.
        for (bool bIsIPv6Ext = true; (false != bIsIPv6Ext); )
        {
            switch (uProtocol)
            {
            case IPv6_EXT_HOP_BY_HOP_OPTIONS_ID:
            case IPv6_EXT_ROUTING_ID:
            case IPv6_EXT_DESTINATION_OPTIONS_ID:
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
                {
                    // Get the header extension length in bytes
                    uint8_t uLen = ((*(pCurr + 1)) + 1) * 8;
                    if ((uint8_t)0 == uLen)
                    {
                        bIsIPv6Ext = false;
                        break;
                    }   
                    // Get to the beginning of next header
                    pCurr += uLen;

                }
                break;
            case IPv6_EXT_FRAGMENT_ID:
                // RFC 2460
                //
                // Fragment Header:
                // Bit  0 -  7 -> Next Header
                // Bit  8 - 15 -> Reserved
                // Bit 16 - 28 -> Fragment Offset
                // Bit 29 - 30 -> Reserved
                // Bit      31 -> 1 = More Fragments; 0 = Last Fragment
                // Bit 32 - 63 -> Identification

                // Get the next header ID
                uProtocol = *pCurr;
                // Get to the beginning of next header
                pCurr += IPv6_EXT_FRAGMENT_LEN;
                break;
            case IPv6_EXT_ENCAPSULATING_SECURITY_PAYLOAD_ID:
                // RFC 2406
                //
                // ray - Don't know how to handle this one!
                bIsIPv6Ext = false;
                break;
            case IPv6_EXT_AUTHENTICATION_ID:
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
                {
                    // Get the header extension length in bytes
                    uint8_t uLen = ((*(pCurr + 1)) + 2) * 4;
                    if (uLen == (uint8_t)0)
                    {
                        bIsIPv6Ext = false;
                        break;
                    }  
                    // Get to the beginning of next header
                    pCurr += uLen;
                }
                break;
            default:
                bIsIPv6Ext = false;
                break;
            }
        }

        uIpHeaderLen = (uint16_t)(pCurr - pIpPacket);
    }
    else
    {
        // IPv4 Header:
        // Bit   0 -   3 -> Version
        // Bit   4 -   7 -> Header Length
        // Bit   8 -  15 -> Differentiated Services
        // Bit  16 -  31 -> Total Length
        // Bit  32 -  47 -> Identification
        // Bit  48 -  50 -> Flags
        // Bit  51 -  63 -> Fragment Offset
        // Bit  64 -  71 -> Time to Live
        // Bit  72 -  79 -> Protocol
        // Bit  80 -  95 -> Header Checksum
        // Bit  96 - 127 -> Source Address
        // Bit 128 - 159 -> Destination Address
        // Bit 160 - 191 -> Options

        // Get to the Total Length field
        uIpHeaderLen = ((uint16_t) pIpPacket[IPv4_HEADER_LENGTH_OFFSET] & 0x0F) << 2;
    }
    return uIpHeaderLen;
}

uint8_t cu_IpPacketExtractIpProtocol(const uint8_t* pIpPacket)
{
    uint8_t uProtocol = 0;
    bool bIsIPv6 = cu_IpPacketIsIPv6(pIpPacket);
    if(false != bIsIPv6)
    {
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
        uProtocol = pIpPacket[IPv6_NEXT_HEADER_OFFSET];
        // Get to the beginning of next header
        uint8_t* pCurr = const_cast<uint8_t*>(pIpPacket) + IPv6_HEADER_LEN;

        // Traverse thru all extension headers, if exist.
        for (bool bIsIPv6Ext = true; (false != bIsIPv6Ext); )
        {
            switch (uProtocol)
            {
            case IPv6_EXT_HOP_BY_HOP_OPTIONS_ID:
            case IPv6_EXT_ROUTING_ID:
            case IPv6_EXT_DESTINATION_OPTIONS_ID:
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
                {
                    // Get the header extension length in bytes
                    uint8_t uLen = ((*(pCurr + 1)) + 1) * 8;
                    if ((uint8_t)0 == uLen)
                    {
                        bIsIPv6Ext = false;
                        break;
                    }
                    // Get to the beginning of next header
                    pCurr += uLen;
                }
                break;
            case IPv6_EXT_FRAGMENT_ID:
                // RFC 2460
                //
                // Fragment Header:
                // Bit  0 -  7 -> Next Header
                // Bit  8 - 15 -> Reserved
                // Bit 16 - 28 -> Fragment Offset
                // Bit 29 - 30 -> Reserved
                // Bit      31 -> 1 = More Fragments; 0 = Last Fragment
                // Bit 32 - 63 -> Identification

                // Get the next header ID
                uProtocol = *pCurr;
                // Get to the beginning of next header
                pCurr += IPv6_EXT_FRAGMENT_LEN;
                break;
            case IPv6_EXT_ENCAPSULATING_SECURITY_PAYLOAD_ID:
                // RFC 2406
                //
                // ray - Don't know how to handle this one!
                bIsIPv6Ext = false;
                break;
            case IPv6_EXT_AUTHENTICATION_ID:
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
                {
                    // Get the header extension length in bytes
                    uint8_t uLen = ((*(pCurr + 1)) + 2) * 4;
                    if ((uint8_t)0 == uLen)
                    {
                        bIsIPv6Ext = false;
                        break;
                    }
                    // Get to the beginning of next header
                    pCurr += uLen;
                }
                break;
            default:
                bIsIPv6Ext = false;
                break;
            }
        }
    }
    else
    {
        // IPv4 Header:
        // Bit   0 -   3 -> Version
        // Bit   4 -   7 -> Header Length
        // Bit   8 -  15 -> Differentiated Services
        // Bit  16 -  31 -> Total Length
        // Bit  32 -  47 -> Identification
        // Bit  48 -  50 -> Flags
        // Bit  51 -  63 -> Fragment Offset
        // Bit  64 -  71 -> Time to Live
        // Bit  72 -  79 -> Protocol
        // Bit  80 -  95 -> Header Checksum
        // Bit  96 - 127 -> Source Address
        // Bit 128 - 159 -> Destination Address
        // Bit 160 - 191 -> Options

        // Get to the Protocol field.
        uProtocol = pIpPacket[IPv4_PROTOCOL_OFFSET];
    }
    return uProtocol;
}

uint32_t cu_IpPacketExtractIpLength(const uint8_t* pIpPacket)
{
    uint32_t uIpLen = 0;
    bool bIsIPv6 = cu_IpPacketIsIPv6(pIpPacket);
    if (false != bIsIPv6)
    {
        // IPv6 Header:
        // Bit   0 -   3 -> Version
        // Bit   4 -  11 -> Traffic Class
        // Bit  12 -  31 -> Flow Label
        // Bit  32 -  47 -> Payload Length
        // Bit  48 -  55 -> Next Header (equivalent to IPv4 protocol field)
        // Bit  56 -  63 -> Hop Limit
        // Bit  64 - 191 -> Source Address
        // Bit 192 - 319 -> Destination Address

        // Get to the Payload Length field
        uIpLen  = ((uint32_t) pIpPacket[IPv6_PAYLOAD_LENGTH_OFFSET] << 8) +
            ((uint32_t) pIpPacket[IPv6_PAYLOAD_LENGTH_OFFSET + 1]);
        uIpLen += IPv6_HEADER_LEN;
    }
    else
    {
        // IPv4 Header:
        // Bit   0 -   3 -> Version
        // Bit   4 -   7 -> Header Length
        // Bit   8 -  15 -> Differentiated Services
        // Bit  16 -  31 -> Total Length
        // Bit  32 -  47 -> Identification
        // Bit  48 -  50 -> Flags
        // Bit  51 -  63 -> Fragment Offset
        // Bit  64 -  71 -> Time to Live
        // Bit  72 -  79 -> Protocol
        // Bit  80 -  95 -> Header Checksum
        // Bit  96 - 127 -> Source Address
        // Bit 128 - 159 -> Destination Address
        // Bit 160 - 191 -> Options

        // Get to the Total Length field
        uIpLen = ((uint32_t) pIpPacket[IPv4_TOTAL_LENGTH_OFFSET] << 8) +
            ((uint32_t) pIpPacket[IPv4_TOTAL_LENGTH_OFFSET + 1]);
    }
    return uIpLen;
}

// Check PPP pattern, do not advance [nBitOffset]
static bool IsMatchingPPPPattern(uint8_t *pData, uint16_t uFrameSize, uint32_t nBitOffset, bool *bIsDoubleOctetProtocol, bool *bHasIPUpperOverPPP)
{
    uint8_t nSingleOctetPattern = 0;
    uint16_t nDoubleOctetPattern = 0;

    cu_ExtractBits(pData, uFrameSize, nBitOffset, 16, (uint8_t *)&nDoubleOctetPattern, true);
    nBitOffset-=16;
    switch(nDoubleOctetPattern)
    {
    case 0x0001:
    case 0x0003:
    case 0x0021:
    case 0x0023:
    case 0x0025:
    case 0x0027:
    case 0x0029:
    case 0x002b:
    case 0x002d:
    case 0x002f:
    case 0x0031:
    case 0x0033:
    case 0x0035:
    case 0x0037:
    case 0x0039:
    case 0x003b:
    case 0x003d:
    case 0x005d:
    case 0x007d:
    case 0x00cf:
    case 0x00fd:
    case 0x00ff:
    case 0x0201:
    case 0x0203:
    case 0x0231:
    case 0x0233:
    case 0x8021:
    case 0x8023:
    case 0x8025:
    case 0x8027:
    case 0x8029:
    case 0x802b:
    case 0x802d:
    case 0x802f:
    case 0x8031:
    case 0x8033:
    case 0x8035:
    case 0x8037:
    case 0x8039:
    case 0x803b:
    case 0x803d:
    case 0x80fd:
    case 0x80ff:
    case 0xc021:
    case 0xc023:
    case 0xc025:
    case 0xc223:
        if(nDoubleOctetPattern == 0x0021)
            *bHasIPUpperOverPPP = true;

        *bIsDoubleOctetProtocol = true;
        return true;
    }

    cu_ExtractBits(pData, uFrameSize, nBitOffset, 8, (uint8_t *)&nSingleOctetPattern, true);
    nBitOffset-=8;
    switch(nSingleOctetPattern)
    {
    case 0x01:
    case 0x03:
    case 0x05:
    case 0x07:
    case 0x09:
    case 0x0b:
    case 0x0d:
    case 0x0f:
        // case 0x11: // Transparency inefficient: no length and id information, so do not handle
    case 0x13:
    case 0x15:
    case 0x17:
    case 0x19:
    case 0x1b:
    case 0x1d:
    case 0x1f:
    case 0x21:
    case 0x23:
    case 0x25:
    case 0x27:
    case 0x29:
    case 0x2b:
    case 0x2d:
    case 0x2f:
    case 0x31:
    case 0x33:
    case 0x35:
    case 0x37:
    case 0x39:
    case 0x3b:
    case 0x3d:
    case 0x5d:
    case 0x7d:
    case 0xcf:
    case 0xfd:
    case 0xff:
        if(nSingleOctetPattern == 0x21)
            *bHasIPUpperOverPPP = true;

        *bIsDoubleOctetProtocol = false;
        return true;
    }

    return false;
}

// Check IP existed
static bool IsMatchingIPPattern(uint8_t *pData, uint16_t uFrameSize, uint32_t nBitOffset)
{
    uint8_t  nVersion, nInternetHeaderLength;
    uint8_t  nBytePattern;

    // skip fake MAC header
    cu_ExtractBits(pData, uFrameSize, nBitOffset, 8, (uint8_t *)&nBytePattern, true);
    nBitOffset-=8;
    if(nBytePattern == 0xFF) // fake MAC header present
        nBitOffset += 40; 

    cu_ExtractBits(pData, uFrameSize, nBitOffset, 4, (uint8_t *)&nVersion, true);
    if(nVersion != 0x04 && nVersion != 0x06) // IPv4
        return false;

    if(nVersion == 0x04)
    {
        // Check Internet Header length is 4 bits field, minimum value is 5 and max is 15
        cu_ExtractBits(pData, uFrameSize, nBitOffset, 4, (uint8_t *)&nInternetHeaderLength, true);
        if( (nInternetHeaderLength < 5) || (nInternetHeaderLength > 15) )
            return false;
    } 
    //Tighten up the checking, if V6 , then add up 40 bytes header length and check it does not exceed the total frame size.
    if(nVersion == 0x06)
    {
        if(((nBitOffset-4)/8+40) > uFrameSize)
            return false;
    }

    return true;
}

bool cu_IpPacketIsIpFragmented(const uint8_t* pIpPacket/*, uint32_t* puFragmentId,
                               uint16_t* puFragmentOffset, bool* pbMoreFragment, uint16_t* puFragmentExtOffset*/)
{
    bool bIsFragmented = false;
    bool bIsIPv6 = cu_IpPacketIsIPv6(pIpPacket);
    if (false != bIsIPv6)
    {
        // IPv6 Header:
        // Bit   0 -   3 -> Version
        // Bit   4 -  11 -> Traffic Class
        // Bit  12 -  31 -> Flow Label
        // Bit  32 -  47 -> Payload Length
        // Bit  48 -  55 -> Next Header (equivalent to IPv4 protocol field)
        // Bit  56 -  63 -> Hop Limit
        // Bit  64 - 191 -> Source Address
        // Bit 192 - 319 -> Destination Address

        //*puFragmentId        = 0;
        //*puFragmentOffset    = 0;
        //*pbMoreFragment      = false;
        //*puFragmentExtOffset = 0;

        // Get the next header ID
        uint8_t uProtocol = pIpPacket[IPv6_NEXT_HEADER_OFFSET];
        // Get to the beginning of next header
        uint8_t* pCurr = const_cast<uint8_t*>(pIpPacket) + IPv6_HEADER_LEN;

        // Look for fragment extension header, if exist.
        for (bool bContinue = true; (false != bContinue); )
        {
            switch (uProtocol)
            {
            case IPv6_EXT_HOP_BY_HOP_OPTIONS_ID:
            case IPv6_EXT_ROUTING_ID:
            case IPv6_EXT_DESTINATION_OPTIONS_ID:
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
                {
                    // Get the header extension length in bytes
                    uint8_t uLen = ((*(pCurr + 1)) + 1) * 8;
                    // Get to the beginning of next header
                    if ((uint8_t)0 == uLen)
                    {
                        bContinue = false;
                        break;
                    }  
                    pCurr += uLen;
                }
                break;
            case IPv6_EXT_FRAGMENT_ID:
                // RFC 2460
                //
                // Fragment Header:
                // Bit  0 -  7 -> Next Header
                // Bit  8 - 15 -> Reserved
                // Bit 16 - 28 -> Fragment Offset
                // Bit 29 - 30 -> Reserved
                // Bit      31 -> 1 = More Fragments; 0 = Last Fragment
                // Bit 32 - 63 -> Identification

                {
                    //*puFragmentId        = ((uint32_t) pCurr[IPv6_EXT_FRAGMENT_IDENTIFICATION_OFFSET    ] << 24) +
                    //    ((uint32_t) pCurr[IPv6_EXT_FRAGMENT_IDENTIFICATION_OFFSET + 1] << 16) +
                    //    ((uint32_t) pCurr[IPv6_EXT_FRAGMENT_IDENTIFICATION_OFFSET + 2] <<  8) +
                    //    ((uint32_t) pCurr[IPv6_EXT_FRAGMENT_IDENTIFICATION_OFFSET + 3]);
                    uint16_t uFragmentOffset = ((uint16_t) pCurr[IPv6_EXT_FRAGMENT_FRAGMENT_OFFSET_OFFSET] << 8) +
                        ((uint16_t) (pCurr[IPv6_EXT_FRAGMENT_FRAGMENT_OFFSET_OFFSET + 1] & 0xF8));
                    bool bMoreFragment = pCurr[IPv6_EXT_FRAGMENT_M_FLAG_OFFSET] & 0x01;
                    /**puFragmentExtOffset = pCurr - pIpPacket;*/
                    bIsFragmented        = (0 < uFragmentOffset) || (bMoreFragment);
                }
                bContinue = false;
                break;
            case IPv6_EXT_ENCAPSULATING_SECURITY_PAYLOAD_ID:
                // RFC 2406
                //
                // ray - Don't know how to handle this one!
                bContinue = false;
                break;
            case IPv6_EXT_AUTHENTICATION_ID:
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
                {
                    // Get the header extension length in bytes
                    uint8_t uLen = ((*(pCurr + 1)) + 2) * 4;
                    if (uLen == (uint8_t)0)
                    {
                        bContinue = false;
                        break;
                    }  
                    // Get to the beginning of next header
                    pCurr += uLen;
                }
                break;
            default:
                bContinue = false;
                break;
            }
        }
    }
    else
    {
        // IPv4 Header:
        // Bit   0 -   3 -> Version
        // Bit   4 -   7 -> Header Length
        // Bit   8 -  15 -> Differentiated Services
        // Bit  16 -  31 -> Total Length
        // Bit  32 -  47 -> Identification
        // Bit  48 -  50 -> Flags
        // Bit  51 -  63 -> Fragment Offset
        // Bit  64 -  71 -> Time to Live
        // Bit  72 -  79 -> Protocol
        // Bit  80 -  95 -> Header Checksum
        // Bit  96 - 127 -> Source Address
        // Bit 128 - 159 -> Destination Address
        // Bit 160 - 191 -> Options

        //*puFragmentId        = ((uint32_t) pIpPacket[IPv4_IDENTIFICATION_OFFSET] << 8) +
        //    ((uint32_t) pIpPacket[IPv4_IDENTIFICATION_OFFSET + 1]);
        uint16_t uFragmentOffset    = ((uint16_t) (pIpPacket[IPv4_FRAGMENT_OFFSET_OFFSET] & 0x1F) << 8) +
            ((uint16_t) pIpPacket[IPv4_FRAGMENT_OFFSET_OFFSET + 1]);
        bool bDontFragment   = (pIpPacket[IPv4_FLAGS_OFFSET] & 0x40) != 0;
        bool bMoreFragment      = (pIpPacket[IPv4_FLAGS_OFFSET] & 0x20) != 0;
        //*puFragmentExtOffset = 0;
        bIsFragmented        = (0 < uFragmentOffset) ||
            (!bDontFragment && (bMoreFragment));
    }
    return bIsFragmented;
}

void ExtractPPPFrameData(uint8_t *pData, uint32_t nFrameSize, uint32_t &nBitOffsetToPPPCode,
                         uint16_t *nPPPFrameId,
                         uint16_t *nPPPLength)
{
    uint32_t nBitOffset = nBitOffsetToPPPCode; 

    nBitOffset += 8; // skip the code
    *nPPPFrameId = 0; // this is 16 bits, but data is only 8 bits so need to reset
    cu_ExtractBits(pData, nFrameSize, nBitOffset, 8, (uint8_t *)nPPPFrameId, true);

    nBitOffset += 8;
    cu_ExtractBits(pData, nFrameSize, nBitOffset, 16, (uint8_t *)nPPPLength, true);
}

bool CMQmonFrameInfo::ExtractIPUpperFrameData(uint8_t* pFrame, uint16_t nFrameLength, uint32_t& nBitOffset)
{
    bool bIpv6, bFrag;
    if (!ExtractIPInfo(pFrame, nFrameLength, nBitOffset, nIpUpperOffset, bIpv6, bFrag, nProtocolUpper, nProtocolUpperOffset))
        return false;
    SetIpUpperIpv6(bIpv6);
    SetIpUpperFrag(bFrag);

    return true;
}

bool CMQmonFrameInfo::ExtractIPInfo(uint8_t* pFrame, uint16_t nFrameLength, uint32_t& nBitOffset,
                                  uint16_t& nIpOffset, bool& bIpv6, bool& bFrag, uint8_t& nProtocol, uint16_t& nProtocolOffset)
{
    uint8_t nBytePattern;
    uint8_t nIPVersion;

    // skip fake MAC header
    if (!cu_ExtractBits(pFrame, nFrameLength, nBitOffset, 8, (uint8_t *)&nBytePattern, true))
        return false;
    nBitOffset -= 8;
    if(nBytePattern == 0xFF) // fake MAC header present
        nBitOffset += 40;

    // Get the IP version
    nIpOffset = nBitOffset/8;
    cu_ExtractBits(pFrame, nFrameLength, nBitOffset, 4, (uint8_t *)&nIPVersion, true);

        // IP extraction
    if (nIPVersion == 0x04) // IPv4
    {
        // If IPv4 we need to look for the existence of IP Header first, if not available then just return false
        if(nFrameLength  < (nIpOffset + IPv4_HEADER_LEN)) 
            return false;
        bIpv6 = false;
    }
    else if (nIPVersion == 0x06) // IPv6
    {
        // If IPv6 we need to look for the existence of IP Header first, if not available then just return false
        if(nFrameLength  < (nIpOffset + IPv6_HEADER_LEN)) 
            return false;
        bIpv6 = true;
    }
    else
        return false;
    nProtocol = cu_IpPacketExtractIpProtocol(pFrame + nIpOffset);

    // Fragment
    bFrag = cu_IpPacketIsIpFragmented(pFrame + nIpOffset);

    // Offset to next protocol
    nProtocolOffset = nIpOffset + cu_IpPacketExtractIpHeaderLength(pFrame + nIpOffset);
    nBitOffset = nProtocolOffset * 8;

    return true;
}

bool CMQmonFrameInfo::Parse(uint8_t* pFrame, uint16_t nFrameLength)
{
    uint16_t nType;
    uint32_t nBitOffset = 0;
    uint8_t nHasNextExtensionHeader = 0;
    uint8_t nHasSequenceNumber = 0;
    uint8_t nHasNPDUNumber = 0;
    uint8_t nGTPMessageType;
    uint16_t nGTPLength;

    m_pFrame = pFrame;
    m_nFrameLength = nFrameLength;

    if(nFrameLength < 34) // MAC header alone is 14 bytes, assuming no VLAN, etc also need 20 bytes more for IP header 
        return false;

    // skip MAC header
    nBitOffset = 96;// skip MAC header (not including VLAN, MPLS, PPPoE), 14 bytes
    nBitOffset += (8 * cu_EtherExtraLengthOffset(pFrame, &nType));
    if((nType != 0x0800) && (nType != 0x86DD)) // not IP frame
        return false;
    nBitOffset += 16;

    bool bIpv6, bFrag;
    if (!ExtractIPInfo(pFrame, nFrameLength, nBitOffset, nIpLowerOffset, bIpv6, bFrag, nProtocolLower, nProtocolLowerOffset))
        return false;
    SetIpLowerIpv6(bIpv6);
    SetIpLowerFrag(bFrag);
    if (bFrag)
        return false;

    if (nProtocolLower == CU_PROTOCOL_TYPE_UDP) //UDP
    {
        // If we have enough info to retrieve port details in the msg then we go ahead otherwise just return false
        // 4 bytes look ahead to retrieve the ports info
        if(nFrameLength  < ((nBitOffset/8) + 4)) 
            return false;

        // Skip port
        nBitOffset += 32;
        if (SrcPortLower() == 0x0868 || DstPortLower() == 0x0868 || // GTP-U (2152) port
            SrcPortLower() == 0x084B || DstPortLower() == 0x084B)  // GTP-C (2123) port
        {
            // Skip GTP
            nBitOffset += 37;
            cu_ExtractBits(pFrame, nFrameLength, nBitOffset, 1, (uint8_t *)&nHasNextExtensionHeader, true);
            cu_ExtractBits(pFrame, nFrameLength, nBitOffset, 1, (uint8_t *)&nHasSequenceNumber, true);
            cu_ExtractBits(pFrame, nFrameLength, nBitOffset, 1, (uint8_t *)&nHasNPDUNumber, true);
            cu_ExtractBits(pFrame, nFrameLength, nBitOffset, 8, (uint8_t *)&nGTPMessageType, true);
            cu_ExtractBits(pFrame, nFrameLength, nBitOffset, 16, (uint8_t *)&nGTPLength, true);

            if (DstPortLower() == 0x0868) // GTP-U (2152) port
            {
                if(nGTPMessageType == 0xFF) // GTP Message Type=G-PDU
                {
                    nBitOffset += 32;

                    if( nHasNextExtensionHeader || nHasSequenceNumber || nHasNPDUNumber )
                        nBitOffset += 32;

                    bool bIsDoubleOctetProtocol = false;
                    bool bHasIPUpperOverPPP = false;
                    if(IsMatchingPPPPattern(pFrame, nFrameLength, nBitOffset, &bIsDoubleOctetProtocol, &bHasIPUpperOverPPP))
                    {
                        return false; 
                    }
                    else if(IsMatchingIPPattern(pFrame, nFrameLength, nBitOffset))
                    {
                        SetTunnel(true);
                        // 3. IP(Lower) -> GTP -> IP(Upper)
                        if (!ExtractIPUpperFrameData(pFrame, nFrameLength, nBitOffset))
                            return false;
                    }
                    else
                    {
                        if (IsIpLowerIpv6())
                            return false;
                    }
                }
            }
        }
    }
    else if (nProtocolLower == CU_PROTOCOL_TYPE_TCP) // TCP
    {
        return true;
    }
    else if (nProtocolLower == CU_PROTOCOL_TYPE_SCTP) // SCTP
    {
        return true;
    }
    else
    {
        // We consider this else part when the protocols are either ICMP, SCTP etc.,
        nBitOffset += 64;

        bool bIsDoubleOctetProtocol = false;
        bool bHasIPUpperOverPPP = false;
        if(IsMatchingPPPPattern(pFrame, nFrameLength, nBitOffset, &bIsDoubleOctetProtocol, &bHasIPUpperOverPPP))
        {
            return false;
        }
        else if ((nProtocolLower == CU_PROTOCOL_TYPE_IPV4) || (nProtocolLower == CU_PROTOCOL_TYPE_IPV6))
        {
            SetTunnel(true);
            // 6. IP(Lower) -> IP(Upper)
            if (!ExtractIPUpperFrameData(pFrame, nFrameLength, nBitOffset))
                return false;
        }
        else
        {
            if (IsIpLowerIpv6())
                return false;
        }
    }        

    return true;
}

bool CMQmonFrameInfo::Extract(uint8_t* pFrame, uint16_t nFrameLength, bool bExtractIpInfo)
{
    if (!Parse(pFrame, nFrameLength))
        return false;

    if (bExtractIpInfo)
    {
        tIpInfo.aSrcAddr.bIpv6 = tIpInfo.aDstAddr.bIpv6 = (IsTunnel() ? IsIpUpperIpv6() : IsIpLowerIpv6());
        uint32_t nBitOffset;
        uint32_t nAddrLength = tIpInfo.aSrcAddr.bIpv6 ? 16*8 : 4*8;
        nBitOffset = (uint32_t)(SrcAddrInnerPtr() - m_pFrame)*8;
        if (!cu_ExtractBits(m_pFrame, m_nFrameLength, nBitOffset, nAddrLength, tIpInfo.aSrcAddr.Ipv6, false))
            return false;
        nBitOffset = (uint32_t)(DstAddrInnerPtr() - m_pFrame)*8;
        if (!cu_ExtractBits(m_pFrame, m_nFrameLength, nBitOffset, nAddrLength, tIpInfo.aDstAddr.Ipv6, false))
            return false;

        if (tIpInfo.aSrcAddr.bIpv6)
        {
            nBitOffset = (uint32_t)(IpInnerPtr() - m_pFrame)*8+4;
            if (!cu_ExtractBits(m_pFrame, m_nFrameLength, nBitOffset, 8, &tIpInfo.nTOS, false))
                return false;
            nBitOffset = (uint32_t)(IpInnerPtr() - m_pFrame + 7)*8;
            if (!cu_ExtractBits(m_pFrame, m_nFrameLength, nBitOffset, 8, &tIpInfo.nTTL, false))
                return false;
        }
        else
        {
            nBitOffset = (uint32_t)(IpInnerPtr() - m_pFrame + 1)*8;
            if (!cu_ExtractBits(m_pFrame, m_nFrameLength, nBitOffset, 8, &tIpInfo.nTOS, false))
                return false;
            nBitOffset = (uint32_t)(IpInnerPtr() - m_pFrame + 8)*8;
            if (!cu_ExtractBits(m_pFrame, m_nFrameLength, nBitOffset, 8, &tIpInfo.nTTL, false))
                return false;
        }

        tIpInfo.pProtocolFrame = m_pFrame + (IsTunnel() ? nProtocolUpperOffset : nProtocolLowerOffset);
        tIpInfo.nProtocolLength = (uint16_t)(IpInnerPtr() + IpInnerLength() - tIpInfo.pProtocolFrame);

        SetIpInfoExtract(true);
    }

    return true;
}

uint16_t CMQmonFrameInfo::IpLowerLength() const
{
    return (uint16_t)cu_IpPacketExtractIpLength(m_pFrame + nIpLowerOffset);
}
uint16_t CMQmonFrameInfo::IpUpperLength() const
{
    return (uint16_t)cu_IpPacketExtractIpLength(m_pFrame + nIpUpperOffset);
}

uint16_t CMQmonFrameInfo::SrcAddrLowerOffset() const
{
    return (nIpLowerOffset + (IsIpLowerIpv6() ? IPv6_SOURCE_ADDRESS_OFFSET : IPv4_SOURCE_ADDRESS_OFFSET));
}
uint16_t CMQmonFrameInfo::SrcAddrUpperOffset() const
{
    return (nIpUpperOffset + (IsIpLowerIpv6() ? IPv6_SOURCE_ADDRESS_OFFSET : IPv4_SOURCE_ADDRESS_OFFSET));
}
uint16_t CMQmonFrameInfo::DstAddrLowerOffset() const
{
    return (nIpLowerOffset + (IsIpLowerIpv6() ? (IPv6_SOURCE_ADDRESS_OFFSET + 32) : (IPv4_SOURCE_ADDRESS_OFFSET + 4)));
}
uint16_t CMQmonFrameInfo::DstAddrUpperOffset() const
{
    return (nIpUpperOffset + (IsIpLowerIpv6() ? (IPv6_SOURCE_ADDRESS_OFFSET + 32) : (IPv4_SOURCE_ADDRESS_OFFSET + 4)));
}

