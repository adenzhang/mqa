#ifndef _RTSDSUP_H_
#define _RTSDSUP_H_
///////////////////////////////////////////////////////////////////////////////
//
// File       : RtsdSupport.h
// Author     : Bill Ives
// Description: RunTime Store to Disk Support routines.
//
//              This is used by the RTSDMeas on the Xscale VxWorks based
//              system for encoding hardware counter difference
//              records into the RtsdBufFmtOtherHeader data.
//              Note, this is linked directly with the RTMeas.out for VxWorks.
//              It is used by the MultiUser (MU) Server to decode and encode
//              the hardware counter records.
//               
//              It is also used by the Elwood DLL system on the Network Analyzer
//              MU Client to decode the hardware counters.
//              
//              NOTE: For code sharing, this is linked into the MultiUser.dll
//                    for the MU Server and Elwood (elwbasp.dll) for NA.
//                    It is been separated out into plain C code to allow
//                    simple linkage on both systems.
//
// Copyright(c) 2006 by Agilent Technologies, Inc.
//
// $Log: RtsdSupport.h $
// Revision 76 2013/09/03 11:09:19 +0800 zha60372 /AnalyzerCommon/7.50.100_Tip/ANT_SASE_RELEASE_7.50.100.003/7.50.100.003-MQA-1-YuKuan/7.50.100.003-MQA-1-Common-ZhangJie
// Redeign variable nVLANMPLSIds and RtsdBufFmtVQStatsEntryCount.
// 
// Revision 73 2013/08/01 16:39:47 +0800 YuK56819 /AnalyzerCommon/7.50.000_Tip/ANT_SASE_RELEASE_7.50.000.041/7.50.000.041-MQA-1-YuKuan
// IQ00357661 (YuKuan): DS-1602 Media QoS Analysis (Telchemy integration)
// 
// Revision 72 2012/06/15 16:39:35 +0800 sha55547 /AnalyzerCommon/7.30.000_Tip/ANT_SASE_RELEASE_7.30.000.054/7.30.000.054-SigcompLicensing-1-Fysal
// Content copied from revision 71
// 
// Revision 70 2012/06/15 11:37:20 +0800 far55528 /AnalyzerCommon/7.30.000_Tip
// IQ00354162 (Fysal): LS-642 DS-1525 [VoLTE]: Integration of Aricent Sigcomp stack and decompression of Sigcomp messages
// 
// Revision 69 2012/03/28 13:29:24 +0800 zha55429 /AnalyzerCommon/7.30.000_Tip/ANT_SASE_RELEASE_7.30.000.028/VHA FP demux
// VHA FP demux
// 
// Revision 68 2012/03/14 21:58:04 +0800 ive55253 /AnalyzerCommon/7.30.000_Tip/NA_PP
// Remove structure tag for RtsdBufFmtOtherHeader that someone let in,
// which is a dup of type typedef tag for the struct...guessing most compilers
// are ok with this, but VxWorks compiler doesn't like it.
// 
// Revision 67 2012/03/05 00:43:04 -0700 jia57196 /AnalyzerCommon/7.30.000_Tip/ANT_SASE_RELEASE_7.30.000.021/ANT_SASE_RELEASE_7.30.000.021-IQ00348518-2-HP
// Automatic merge of parallel changes into revision [64]
// 
// Revision 66 2012/03/05 15:40:40 +0800 jia57196 /AnalyzerCommon/7.30.000_Tip/ANT_SASE_RELEASE_7.30.000.012/ANT_SASE_RELEASE_7.30.000.012-IQ00348518-1-HP

// Revision 64 2012/02/16 20:59:50 +0800 yao57890 /AnalyzerCommon/7.30.000_Tip/ANT_SASE_RELEASE_7.30.000.014/ANT_SASE_RELEASE_7.30.000.014-Yijia-PiAuth
// 
// Revision 62 2012/02/13 10:08:35 +0800 jia57196 /AnalyzerCommon/7.30.000_Tip/ANT_SASE_RELEASE_7.30.000.010/ANT_SASE_RELEASE_7.30.000.010-IQ00348518-1-HP
// IQ00348518(hp):2, add RtsdCounterForPPProbeAccuGapEntry stuff.
// 
// Revision 61 2012/02/08 16:31:23 +0800 jia57196 /AnalyzerCommon/7.30.000_Tip/ANT_SASE_RELEASE_7.30.000.010/ANT_SASE_RELEASE_7.30.000.010-IQ00348518-1-HP
// IQ00348518: remove the accuGap variable in struct of RtsdBufFmtOtherHeader
// 
// Revision 60 2011/12/29 10:55:13 +0800 jia57196 /AnalyzerCommon/7.20.100_Tip/ANT_SASE_RELEASE_7.20.100.014/ANT_SASE_RELEASE_7.20.100.014-IQ00348518-1-HP
// IQ00348518: add probe id const
// 
// Revision 59 2011/12/12 13:14:50 +0800 jia57196 /AnalyzerCommon/7.20.100_Tip/ANT_SASE_RELEASE_7.20.100.014/ANT_SASE_RELEASE_7.20.100.014-IQ00348518-1-HP
// IQ00348518:[PP] L1/L2 measurement and per probe packet loss statistics
// Revision 57 2011/11/15 16:37:39 +0800 YuK56819 /AnalyzerCommon/7.20.100_Tip/ANT_SASE_RELEASE_7.20.100.006/ANT_SASE_RELEASE_7.20.100.006_PP_1_YuKuan
// IQ00348214 (YuKuan): [PP] uuDataSource should start sending timestamp rtsm frame even if it never receives data from feeder
// 
// Revision 56 2011/09/09 12:02:55 +0800 din57058 /AnalyzerCommon/7.20.000_Tip/CTO-Apps/7.20.000.015/7.20.000.015-Reena-IQ00346803
// Automatic merge of parallel changes into revision [55]
// 
// Revision 55 2011/09/09 11:37:38 +0800 cha57621 /AnalyzerCommon/7.20.000_Tip/CTO-Apps/7.20.000.015/7.20.000.015-Reena-IQ00346803
// Automatic merge of parallel changes into revision [51]
// 
// Revision 54 2011/09/08 14:07:02 +0800 lee55391 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.015/7.20.000.015-IQ00347301-1-BillIves
// Automatic merge of parallel changes into revision [51]
// 
// Revision 53 2011/09/08 11:19:31 +0800 cha57621 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.014/ANT_SASE_RELEASE_7.20.000.014-Reena
// Moved spare:1 before isPPFrame:1 in RtsdFrame structure
// 
// Revision 52 2011/09/08 09:27:57 +0800 cha57621 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.014/ANT_SASE_RELEASE_7.20.000.014-Reena
// changing isPPFrame:2 to isPPFrame:1 in RtsdFrame structure
//
// Revision 51 2011/09/05 15:13:39 +0800 and55460 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.014/ANT_SASE_RELEASE_7.20.000.014-RtsdOtherAuthenticationSubtype
// IQ00347305 (Bob): [MtAuth][SART/MT] Add RTSD Other Subtype for Authentication frames
//
// Revision 49 2011/09/02 15:13:00 +0800 YuK56819 /AnalyzerCommon/7.20.000_Tip/CTO-Apps/7.20.000.013/7.20.000.013-PP-YK
// IQ00347275 (YuKuan): [PP] Store FR sequence value in rtsm frame header
// 
// Revision 47 2011/09/02 09:24:01 +0800 YuK56819 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.013/ANT_SASE_RELEASE_7.20.000.013-PP-YK
// IQ00347275 (YuKuan): [PP] Store FR sequence value in rtsm frame header
// 
// Revision 45 2011/08/26 21:24:13 +0800 ive55253 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.008/7.20.000.008.PacketPortal-Dev-BillIves
// Added packet protal layer1 struct to RtsdHWLayer1Config union.
//
// Revision 44 2011/06/13 16:14:03 +0800 YuK56819 /AnalyzerCommon/7.10.000_Tip/ANT_SASE_RELEASE_7.10.000.025/7.10.000.025-PacketPortal-Dev/7.10.000.025-PacketPortal-Dev-1-YuKuan
// PP Integration (YuKuan): When new client connects, MUS sends PacketPortal probe id map to the new client (add RtsdHWPPPacketType)
// 
// Revision 43 2011/05/05 15:53:16 +0800 YuK56819 /AnalyzerCommon/7.10.000_Tip/ANT_SASE_RELEASE_7.10.000.025/7.10.000.025-PacketPortal-Dev/7.10.000.025-PacketPortal-Dev-1-YuKuan
// PP Integration (YuKuan): Set spare bit in rtsm header
// 
// Revision 42 2011/04/27 10:33:40 +0800 YuK56819 /AnalyzerCommon/7.10.000_Tip/ANT_SASE_RELEASE_7.10.000.025/7.10.000.025-PacketPortal-Dev/7.10.000.025-PacketPortal-Dev-1-YuKuan
// PP Integrate (YuKuan): Add probe id map
// 
// Revision 38 2010/09/10 05:04:12 +0800 ive55253 /AnalyzerCommon/7.00.100_Tip/COS/10G and MP Gig Fixes
// Minor fix to make old C code that is not compiled as C++ work too.
// 
// Revision 37 2010/09/09 14:43:37 -0600 bi4723 /AnalyzerCommon/7.00.100_Tip/COS/10G and MP Gig Fixes
// Automatic merge of parallel changes into revision [34]
// 
// Revision 36 2010/09/03 11:43:19 -0600 bi4723 /AnalyzerCommon/6.65_Extensions/6.65.101/bi4723 Directional Network Vitals & RTP Stats Enhancements
// Remove now defunct 10G 4 port struct ref in RtsdHWLayer1Config now that
// we have switch over to using 5port structure
// 
// Revision 35 2010/08/23 07:56:15 -0600 bi4723 /AnalyzerCommon/6.65_Extensions/6.65.102/RecFilePlayback
// Fix to copy/paste error on last checkin (oops).
// 
// Revision 34 2010/08/12 21:28:44 -0600 yua55582 /AnalyzerCommon/6.70.100_Tip/ANT_SASE_RELEASE_6.70.100.101/6.70.100.101-BundleStats-LimPort-2-Zijing
// IQ00189297 [Zijing][Bundlestats] move LimPortNum to layer1 header
// 
// Revision 33 2010/08/05 09:56:29 +0800 zijiyuan /AnalyzerCommon/6.70.100_Tip/ANT_SASE_RELEASE_6.70.100.100/6.70.100.100-UPStats-Version2-Zijing
// Add LimPort source identifier for bundlestats
// 
// Revision 32 2010/08/02 11:30:19 -0600 bi4723 /AnalyzerCommon/6.65_Extensions/6.65.102/RecFilePlayback
// Minor fix to MAX_RTSM_PACKET_SIZE for VxWorks and update comments
// to make this clear.
// 
// Revision 29 2010/06/09 16:52:44 -0600 thohenry /AnalyzerCommon/6.65_Extensions/6.65.101/RTSD/backport
// Content copied from revision 28
// 
// Revision 25 2010/05/14 16:22:27 -0600 tm4457 /AnalyzerCommon/6.70.100_Tip/COS/SARTRocket/tm4457
// Add 5 port support for SARTRocket.
// 
// Revision 24 2009/12/11 13:46:04 -0700 sb4450 /AnalyzerCommon/6.70.100_Tip/COS/SARTRocket
// Changed name of MAX macro due to conflict with DNA Measurments in SART.
// 
// Revision 23 2009/12/10 09:35:43 -0700 sb4450 /AnalyzerCommon/6.70.100_Tip/COS/SARTRocket
// Increased Maximum RTSM packet size for BladeRocket to 256K plus 32K (Extra space for the card to overrun with the last frame).
// 
// Revision 22 2009/11/24 12:32:01 -0700 sb4450 /AnalyzerCommon/6.65.100_Tip/SARTRocket
// Updated UP Statistics structures.
// 
// Revision 21 2009/11/20 12:21:50 -0700 sb4450 /AnalyzerCommon/6.65.100_Tip/SARTRocket
// First Pass at structures for SARTRocket UP Statistics.
// 
// Revision 20 2009/02/04 14:26:39 -0700 bi4723 /AnalyzerCommon/6.60.100_Tip/BladeRocket
// Added first pass 10G layer1 config structure to RtsdHWLayer1Config union.
// 
// Revision 19 2008/12/09 09:20:37 -0700 bi4723 /AnalyzerCommon/6.50.100_Tip/ChannelStatsRTSM
// Fix for alignStatus handling for WAN channel stats -- it is not a counter and so the diff counter methods used for it we not correct.
// 
// Revision 18 2008/12/05 11:42:21 -0700 bi4723 /AnalyzerCommon/6.50.100_Tip/ChannelStatsRTSM
// Add first pass Channel, HDLC trib & ATM trib stats support to RTSM stream.
// 
// Revision 17 2008/05/29 23:37:06 -0600 sb4450 /AnalyzerCommon/6.50.100_Tip/Scott-DS-465
// Added slicing bit to RTSM header for DS-465
// 
// Revision 16 2007/11/27 16:59:43 -0700 tm4457 /AnalyzerCommon/6.00.100_Tip/HDDNA/MobileDNA/tm4457
// Add support for Multi-port Gig LIMs.
// 
// Revision 15 2007/11/19 17:09:27 -0700 thohenry /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Automatic merge of parallel changes into revision [14]
// 
// Revision 14 2007/11/16 11:14:39 -0700 thohenry /AnalyzerCommon/5.60.100_Tip/MobileDNA/TH5605
// Protected inline bool functions in BufferFormat.h from begin compiled in extern "C" blocks.
// 
// Revision 11 2007/10/22 15:48:31 -0600 tm4457 /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Add  LimL1CfgMPortLANEx data member to RtsdHWLayer1Config union to support multi-port Gig LIMs.
// 
// Revision 10 2007/08/09 13:31:08 -0600 tm4457 /AnalyzerCommon/5.50.100_Tip/DNA_Measurement_Merge
// Change from TCHAR to char for embedded side compilation.
// 
// Revision 9 2007/05/23 13:38:02 -0600 tm4457 /AnalyzerCommon/5.40.100_Tip/MobileDNA/tm4457
// Wrapped RtsdHWCountersTypeEx structure in #ifdef WIN32 so embedded side would build
// properly.
// 
// Revision 8 2007/05/22 10:48:41 -0600 tm4457 /AnalyzerCommon/5.40.100_Tip/MobileDNA/tm4457
// Add RtsdHWCountersTypeEx structure.
// 
// Revision 7 2007/05/08 15:10:22 -0600 ch4726 /AnalyzerCommon/5.40.100_Tip/MobileDNA/CH4726 MobileDNA
// Added Protocol Vitals and Line Status enums
// 
// Revision 6 2006/11/09 16:39:39 -0700 bi4723 /AnalyzerCommon/5.20_Release/NA_Multiuser
// Added ATM IMA stats support. Also changed the RTSD headers to be packed on VxWorks (and
// Windows) since VxWorks compiler started failing in weird way without these.
// 
// Revision 5 2006/11/02 10:05:00 -0700 bi4723 /AnalyzerCommon/5.20_Release/NA_Multiuser
// Major rework of the ATM vpvc and FR dlci stats structures to be UINT64s since the LIM side always
// sends difference counters for these.  Also added encode support for the non-VxWorks modes --
// SART MU server and NA MU client.
// 
// Revision 4 2006/08/22 13:08:57 -0600 bi4723 /AnalyzerCommon/Nova/NA_Multiuser
// Moved max rtsm packet size constant to this common location for system to have just 1 copy.
// 
// Revision 3 2006/07/10 15:35:49 -0600 bi4723 /AnalyzerCommon/Nova/NA_Multiuser
// Changed RtsdBufFmtTimeHeader to match what ScramJet had defined and to make it more obvious
// that it had a flags field.
// 
// Revision 2 2006/03/24 13:43:04 -0700 bi4723 /AnalyzerCommon/Nova/NA_Multiuser
// Fixed up include for sharing of bufferformat with SART MU Server.
// 
// Revision 1 2006/03/22 14:50:26 -0700 bi4723 /AnalyzerCommon/ScramJet/InitialStructure/NA_Multiuser
// First pass breakout sharing of common defs between SART and NA.
// 
// Revision 1 2006/02/06 15:12:44 -0700 bi4723 /Analyzer/Nova/NA_Multiuser
// First pass NA MU support.
// 
//
///////////////////////////////////////////////////////////////////////////////

// Include other support/shared definitions.

#include "BufferFormat.h"
#include "LimSharedDefs.h" // for hardware counter constants, LimLineStatus etc
#include "SarSharedDefs.h" // for SAR constants used to send SAR config, SARDrvGenConfig etc

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RTSD_MAX
#define RTSD_MAX(A,B) (((A)>(B)) ? (A) : (B))
#endif

#ifdef WIN32
#pragma pack( push, RTSDTIME1, 1 )
#endif

typedef struct
{
   UINT8   recordType:2;     
   UINT8   limSide:2;        
   UINT8   discontinuity:1;
   UINT8   smartSliced:1;   // SAB Added 6.50: MU Server smart packet slicing (1=sliced)
   UINT8   isPPAdjusted:1;  // Set if PacketPortal frame is time adjusted by UuDataSource
   UINT8   isPPFrame:1;     // SAB prior to 6.50, this was 3 bits
                            // bit mask 01 is used for packets from Packet Portal
   UINT8   hardwareBits;    // store upper 8 bit of PacketPortal sequence number
   UINT8   statusBits;
   UINT8   PPSequenceNumberLowerByte;   // store lower 8 bit of PacketPortal sequence number

   UINT32  timeSecs;       
   UINT32  timeNSecs;      
                            // fields aligned with RtsdBufFmtEventHeader up until this point
   UINT16  storedLength;
   UINT16  rcvLength;      
   UINT32  filtersMatched;  
   UINT8   limInfoLength;   
   UINT8   limInfo[BUFFMT_LIMINFO_MAX_LENGTH];
                            // if packet from Packet Portal, limInfo[0-1] is used for mapped probeid
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdBufFmtFrameHeader;

typedef struct
{
   UINT8   recordType:2;     
   UINT8   limSide:2;        
   UINT8   discontinuity:1;
   UINT8   spare:3;  
   UINT8   reserved1;
   UINT8   reserved2;
   UINT8   reserved;

   UINT32  timeSecs;       // high bits of timestamp: seconds since 1970
   UINT32  timeNSecs;      // low bits of time stamp: 0 to 999,999,999 nanoseconds

                           // fields aligned with RtsdBufFmtFrameHeader up until this point 

   UINT32  eventInfo1;     // Extra event info will fit
   UINT32  eventInfo2;     // in these 5 fields.
   UINT32  eventInfo3;
   UINT32  eventInfo4;
   UINT32  eventInfo5;
   UINT32  eventInfo6;
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdBufFmtEventHeader;

typedef struct
{
   UINT32  recordType:2;  // Set to BUFFMT_TYPE_EMPTY to allow other info to be sent from RTSD  
   UINT32  subType   :6;  //  subtypes used to pass other info from RTSD
   UINT32  length    :24; // length of this "other" record (does not include this header).

   UINT32  timeSecs;     // timeStamp of when info was sent or is relevant.   
   UINT32  timeNSecs;      
                         // fields aligned with RtsdBufFmtEventHeader up until this point
   // "length" bytes of data following
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdBufFmtOtherHeader;


typedef struct
{
   UINT8   recordType:2;   // type BUFFMT_TYPE_TIME
   UINT8   reserved:6;     // not used.  
   UINT8   flags;
   UINT32  timeSecs;       
   UINT32  timeNSecs;      
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdBufFmtTimeHeader;

#ifdef WIN32
#pragma pack( pop, RTSDTIME1 )
#endif


// Structure for Rtsd buffer record headers.  Use this typedef for headers within Keystone.
typedef union
{
    RtsdBufFmtFrameHeader   frame;
    RtsdBufFmtEventHeader   event;
    RtsdBufFmtOtherHeader   other;
} RtsdFmtHeader; 

// For RtsdBufFmtOtherHeader.subType, the following values are defined.
#define RTSD_OTHER_SUBTYPE_HWCOUNTER_ABS    1
#define RTSD_OTHER_SUBTYPE_HWCOUNTER_DIFF   2
#define RTSD_OTHER_SUBTYPE_LIM_CONFIG       3 
#define RTSD_OTHER_SUBTYPE_LINK_STATUS      4
#define RTSD_OTHER_SUBTYPE_SAR_CONFIG       5
#define RTSD_OTHER_SUBTYPE_UP_STATISTICS    6
#define RTSD_OTHER_SUBTYPE_PP_PROBEID_MAP   7
#define RTSD_OTHER_SUBTYPE_AUTHENTICATION   8
#define RTSD_OTHER_SUBTYPE_PROBE_STATISTIC  9 //for packetportal statistics.
#define RTSD_OTHER_SUBTYPE_VQ_STATISTICS    10 //for video/voice quality
// For Hardware Counters subtype(RTSD_OTHER_SUBTYPE_HWCOUNTER_xxx), the following
// overlayed general structure is defined.
// The only difference between xx_ABS (absolute) and xx_DIFF is that the absolute
//   counters are typically(should be?) only sent ONCE at the beginning of the run
//   and thus indicate to the receiver to latch the base values.  This contrasts with
//   xx_DIFF counters which are added to the current "cummalative" state counters on
//   the receiver. For example:
//              RTSD_OTHER_SUBTYPE_HWCOUNTER_ABS with "counterX"=1000
//              RTSD_OTHER_SUBTYPE_HWCOUNTER_DIFF with "counterX"=100 --> counterX(real value)=1100
//              RTSD_OTHER_SUBTYPE_HWCOUNTER_DIFF with "counterX"=200 --> counterX(real value)=1300
//   In addition, of a counter is NOT specified in either the value is considered to be 0(for xxx_ABS) or
//    unchanged(for xxx_DIFF).  
// Note that some "counters" are NOT really counters and their state is sent whenever the value
//  changes.  An example of this is the RTSD_HW_CNT_LIM_STATUS_PORTx hardware counter values.
//  Also, some counters will never be sent if not applicable to the technology. Thus only FrameRelay
//  and ATM will ever have the DLCI/Vpi.Vci (RTSD_HW_CNT_FR_ATM_STATS) stats.
//typedef struct
//{
//    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
//    UINT32                m_nCountersPresent;  // BitMapped counters present flags
//    UINT8                 m_packetData[];
//} RtsdBufFmtCounterHeader;

// The data portion is composed of counters as defined by the bitmapped field
//  m_nCountersPresent which has the following bit mapped values are defined.
#define RTSD_HW_CNT_LIM_CNTS_BASE1   0x00000001
#define RTSD_HW_CNT_LIM_CNTS_BASE2   0x00000002
#define RTSD_HW_CNT_LIM_CNTS_PORT1   0x00000004
#define RTSD_HW_CNT_LIM_CNTS_PORT2   0x00000008
#define RTSD_HW_CNT_LIM_CNTS_PORT3   0x00000010
#define RTSD_HW_CNT_LIM_CNTS_PORT4   0x00000020
#define RTSD_HW_CNT_LIM_CNTS_PORT5   0x00000040
#define RTSD_HW_CNT_LIM_CNTS_PORT6   0x00000080
#define RTSD_HW_CNT_LIM_CNTS_PORT7   0x00000100
#define RTSD_HW_CNT_LIM_CNTS_PORT8   0x00000200
#define RTSD_HW_CNT_LIM_STATUS_PORT1 0x00000400
#define RTSD_HW_CNT_LIM_STATUS_PORT2 0x00000800
#define RTSD_HW_CNT_LIM_STATUS_PORT3 0x00001000
#define RTSD_HW_CNT_LIM_STATUS_PORT4 0x00002000
#define RTSD_HW_CNT_LIM_STATUS_PORT5 0x00004000
#define RTSD_HW_CNT_LIM_STATUS_PORT6 0x00008000
#define RTSD_HW_CNT_LIM_STATUS_PORT7 0x00010000
#define RTSD_HW_CNT_LIM_STATUS_PORT8 0x00020000
#define RTSD_HW_CNT_FR_DLCI_STATS    0x00040000
#define RTSD_HW_CNT_ATM_VPVC_STATS   0x00080000
#define RTSD_HW_CNT_IMA_STATS        0x00100000
#define RTSD_HW_CNT_CHANNEL_STATS    0x00200000
#define RTSD_HW_CNT_HDLCTRIB_STATS   0x00400000
#define RTSD_HW_CNT_ATMTRIB_STATS    0x00800000

// The counters are in their bit mapped order within the m_packetData and each have their own format
// as described:
//  LIM CNTS BASE1 format: (present if bit RTSD_HW_CNT_LIM_CNTS_BASE1 set in main m_nCountersPresent mask )
//    UINT64  nCountersPresent[2] ;
//    UINTx   counters[] ; // size of counters and number of counters defined by bits.
//    (see limDriver.h for constants) 
//    First 32 LIM base counters from  CNT_LINE_STATUS(1) to CNT_CBIT_PARITY(32)
//      Each counter is defined by 2 bits: 
//        where counter size/presence is defined as (0=No counter,1=16bit,1=32bit,3=64bit)
//      Starting with CNT_LINE_STATUS is lowest 2 bits (0x0000 0000 0000 0003)
//      The counters are in order from side 0 to side 1...thus all side 0 counters are in-order
//             and then followed by side 1 counters.
//
//  LIM CNTS BASE2 format: (present if bit RTSD_HW_CNT_LIM_CNTS_BASE2 set in main m_nCountersPresent mask )
//    UINT64  nCountersPresent[2] ;
//    UINTx   counters[] ; // size of counters and number of counters defined by bits.
//    Next 26 LIM base counters from  CNT_PLCP_BIP(33) to CNT_AAL2_FRAME_BYTES(58) (can handle upto 32 in future)
//      Each counter is defined by 2 bits: 
//        where counter size/presence is defined as (0=No counter,1=16bit,1=32bit,3=64bit)
//      Starting with CNT_PLCP_BIP is lowest 2 bits (0x0000 0000 0000 0003)
//      The counters are in order from side 0 to side 1...thus all side 0 counters are in-order
//             and then followed by side 1 counters.
//
//  RTSD_HW_CNT_LIM_CNTS_PORTx format:(present if bit RTSD_HW_CNT_LIM_CNTS_PORTx set in main m_nCountersPresent mask )
//    UINT64 nCountersPresent[2] ; // per side
//    UINTx  counters[] ; // size of counters and number of counters defined by bits.
//    26 counters for port x from CNT_LCV_PORTx to CNT_SHORT_FRAMES_PORTx 
//      Each counter is defined by 2 bits: 
//        where counter size/presence is defined as (0=No counter,1=16bit,1=32bit,3=64bit)
//      Starting with CNT_LCV_PORTx is lowest 2 bits (0x0000 0000 0000 0003)
//      The counters are in order from side 0 to side 1...thus all side 0 counters are in-order
//             and then followed by side 1 counters.
//
//  RTSD_HW_CNT_LIM_STATUS_PORTx format:(present if bit RTSD_HW_CNT_LIM_STATUS_PORTx set in main m_nCountersPresent mask )
//   Lim line status entries are always absolute, not "diffs" like other counters.
//      LimLineStatus  counters[2]; // one for each side, typedef in LimDriver.h.
//
//  RTSD_HW_CNT_FR_DLCI_STATS & RTSD_HW_CNT_ATM_VPVC_STATS format:
//   (present if bit RTSD_HW_CNT_xxx_STATS set in main m_nCountersPresent mask )
//      UINT16  rows[2]; // number of rows on each side
//      UINT32  flags ;  // extra flags used to define the stats
//            RTSD_HW_CNT_STATS_FLAG_LIMSRC is set if the counters
//                       are LIM sourced difference counters.
//      UINT*   data[] ;// size of counters and number of counters defined by rows.
//    For FrameRelay DLCI stats: each DLCI stat entry is the following format contained in data
//     UINT32 dlci;
//     UINT32 nCountersPresent; 
//     UINTx  counters[] ; // size of counters and number of counters defined by bits.
//     Each counter is defined by 2 bits: 
//        where counter size/presence is defined as (0=No counter,1=16bit,1=32bit,3=64bit)
//     12 counters are defined in-order as: 
//         frames, bytes, FECNs, BECNs, DEs,
//          size63, size127,size255, size511, size1023, size1518, sizePlus
//      Starting with "frames" counter which is the lowest 2 bits (0x0000 0000 0000 0003)
//    For ATM Vpi.Vci stats: each stat entry is the following format contained in data
//      UINT32 aal     :4  ;
//      UINT32 vpi     :12 ;
//      UINT32 vci     :16 ;
//      UINT32 id      :10 ;    // Port number / Tributary Number / Channel ID
//      UINT32 unused2 :22 ;
//      UINT32 nCountersPresent; ;
//     UINTx  counters[] ; // size of counters and number of counters defined by bits.
//     Each counter is defined by 2 bits: 
//        where counter size/presence is defined as (0=No counter,1=16bit,1=32bit,3=64bit)
//     14 counters are defined in-order as: 
//         busyCount, CLPCount, congCount, F5OAMCells, frameCount, errorCount, aal2byteCount,
//         size63,size127,size255, size511, size1023, size1518, sizePlus
//      Starting with "busyCount" counter which is the lowest 2 bits (0x0000 0000 0000 0003)
//       
//    The counters are in order from side 0 to side 1...thus all side 0 counters are in-order
//       and then followed by side 1 counters.
//
//  RTSD_HW_CNT_IMA_STATS format:
//   (present if bit RTSD_HW_CNT_IMA_STATS set in main m_nCountersPresent mask )
//      UINT8   ports;   // bit mapped port value: 1=port is selected 0=port is not selected
//      UINT8   receiverMode; // IMA receiver mode
//      For each side, the following fields are present:
//        UINT8    m_status;  //Bit mapped per port status (1:link is bad,0:link is good)
//        UINT8    m_ica;     //Bit mapped per port status (1:icp cell is available,0:icp cell is not available)
//        UINT8    m_delay;   //Bit mapped per port status (1:delay is available, 0:delay is not available(0xFFFFFFFF))
//        UINT32   m_linkInfo[8][2]; // per port status of the 32 links
//                          // where each 2-bit group represent status states:
//                          //   IMA_LINK_STATUS_NOT_IN_GROUP
//                          //   IMA_LINK_STATUS_UNUSABLE
//                          //   IMA_LINK_STATUS_USABLE
//                          //   IMA_LINK_STATUS_ACTIVE
//                          //   (as defined in LimSharedDefs.h)
//                          // Starting from LSB, linkInfo[N][0] contains
//                          // the status of the 1st 16 links on portN, and
//                          // linkInfo[N][1] contains the status of the other 16 links
//                          // on portN. 
//        UINT8 m_group[x]; //Per port IMA group number when ica is set for a specific port(otherwise N/A)
//                          //  There may not be any of these if no ica bits are set.
//        UINT8 m_link[x];  //Per port IMA link number when ica is set for a specific port(otherwise N/A)
//                          //  There may not be any of these if no ica bits are set.
//        UINT32 m_delayValue[x];//Per port link delay when delay is set for a specific port(otherwise N/A(0xffffffff))
//                               //  There may not be any of these if no delay bits are set.
//    These IMA status fields are in order from side 0 to side 1...thus all side 0 status fields are in-order
//       and then followed by side 1 status fields.
//
#define RTSD_HW_CNT_STATS_FLAG_LIMSRC 1
#define MAX_DLCI_VPVC_STATS 1024
#define MAX_CHANNEL_STATS 1024
#define MAX_HDLCTRIB_STATS 336
#define MAX_ATMTRIB_STATS 336

#define CONST_PROBID_LENGTH 6


typedef UINT64                RtsdHWLimCountersType[N_SIDES][N_COUNTERS] ;
typedef LimLineStatus         RtsdHWLimStatusType[NUM_OF_PORTS][N_SIDES];

typedef enum 
{ 
    RTSD_HWSTAT_TYPE_NONE=0,
    RTSD_HWSTAT_TYPE_FR_DLCI,
    RTSD_HWSTAT_TYPE_ATM_VPVC,
    RTSD_HWSTAT_TYPE_CHANNEL,
    RTSD_HWSTAT_TYPE_HDLCTRIB,
    RTSD_HWSTAT_TYPE_ATMTRIB
} RtsdHWStatType;


#ifdef WIN32
#pragma pack( push, RTSDTIME1, 1 )
#endif

typedef struct
{
  UINT64 size63;       // histogram frames of length    0 -   63
  UINT64 size127;      // histogram frames of length   64 -  127
  UINT64 size255;      // histogram frames of length  128 -  255
  UINT64 size511;      // histogram frames of length  256 -  511
  UINT64 size1023;     // histogram frames of length  512 - 1023
  UINT64 size1518;     // histogram frames of length 1024 - 1518
  UINT64 sizePlus;     // histogram frames of length > 1518
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdHWStatFrameSizeType; 

// Rtsd DLCI hardware stat type, contains TOTALs for each DLCI, unlike the internal
//   LimDriver StatDlci which is only per-sample counts.  For this reason,
//   the values are laid out in the same order as StatDlci but are UINT64s.
typedef struct
{
  UINT32 dlci;
  UINT64 frames;
  UINT64 bytes;
  UINT64 FECNs;
  UINT64 BECNs;
  UINT64 DEs;
  RtsdHWStatFrameSizeType frameSizes;
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdHWStatDlciType ;

// Rtsd VpiVci hardware stat type, contains TOTALs for each VpVc, unlike the internal
//   LimDriver StatVpvc which is only per-sample counts.  For this reason,
//   the values are laid out in the same order as StatVpvc but are UINT64s.

typedef struct
{
  UINT32 aal     :4  ;
  UINT32 vpi     :12 ;
  UINT32 vci     :16 ;
  UINT32 id      :10 ;    // Port number / Tributary Number / Channel ID
  UINT32 unused2 :22 ;

  //---------------------------------------------------------------------------
  // These following counters have a slightly different meaning 
  // depending on mode of operation: Cell Mode, AAL2 and AAL5 
  UINT64 busyCount;    // Busy Cell count

  UINT64 CLPCount;     // Cell mode: Cell count with CLP=1
                       // AAL2/AAL5 mode: Frames with at least one cell with CLP=1

  UINT64 congCount;    // Cell mode: Congested Cell count
                       // AAL2/AAL5 mode: Frames with at least one congested cell
  
  UINT64 F5OAMCells;   // F5 OAM cell count

  //---------------------------------------------------------------------------
  // All subsequent Counters are used in AAL2 or AAL5 only, not used in Cell Mode.
  UINT64 frameCount;   // Total Frame Count

  UINT64 errorCount;   // AAL5 Mode: frames with CRC32 error
                       // AAL2 Mode: Mini-cell hec error count

  // Histogram for Frame Lengths
  RtsdHWStatFrameSizeType frameSizes;

  //---------------------------------------------------------------------------
  // This counter is only used in AAL2
  UINT64 byteCount;    // Byte count: mini-Cell header + payload)

} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdHWStatVpvcType;

// Rtsd HDLC Trib hardware stat type, contains TOTALs for each TribID, unlike the internal
//   LimDriver StatHDLCTrib which is only per-sample counts.  For this reason,
//   the values are laid out in the same order as StatHDLCTrib but are UINT64s.
// NOTE: this is ONLY on 4Port OC3 LIM.

typedef struct
{
  UINT32 id         :16;          // TribID - See LimOctoTribID
  UINT32 configured :1;           // 1 = Trib is configured;
  UINT32 status     :1;           // 1 = Trig has good framing.
  UINT32 unused     :14;

  UINT64 frameCount ;
  UINT64 byteCount ;
  UINT64 shortCount ;
  UINT64 abortCount ;
  UINT64 fcsErrorCount ;
  UINT64 fsyncErrorCount ;
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdHWStatHDLCTribType;

// Rtsd ATM Trib hardware stat type, contains TOTALs for each TribID, unlike the internal
//   LimDriver StatATMTrib which is only per-sample counts.  For this reason,
//   the values are laid out in the same order as StatATMTrib but are UINT64s.
// NOTE: this is ONLY on 4Port OC3 LIM.
typedef struct
{
  UINT32 id         :16;          // TribID - See LimOctoTribID
  UINT32 configured :1;           // 1 = Trib is configured;
  UINT32 status     :1;           // 1 = Trig has good framing.
  UINT32 unused     :14;

  UINT64 frameCount;   // Total Frame Count
  UINT64 busyCount;    // Busy Cell count
  UINT64 byteCount;    // Byte count
  UINT64 congCount;    // Congested Cell count
  UINT64 idleCellCount ;
  UINT64 crcErrorCount ;
  UINT64 LCDCount ;
  UINT64 HECErrorCount ;
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdHWStatATMTribType;

// Rtsd Channel hardware stat type, contains TOTALs for each channel, unlike the internal
//   LimDriver StatChannel which is only per-sample counts.  For this reason,
//   the values are laid out in the same order as StatChannel but are UINT64s.
// NOTE: this is currently ONLY on 8PortT1/E1 LIMs for HDLC channelized mode.
typedef struct
{
  UINT32 id;             // Channel Number
  UINT32 alignStatus;    // Alignment Link Status
  UINT64 frameCount;     // used for msuFrame counter.
  UINT64 byteCount;      // used for msuByte counter.
  UINT64 fcsErrorCount;  // crcErrorCounter
  UINT64 abortCount;
  UINT64 shortCount;
  UINT64 lssuFrameCount; // Link Status Signal Unit
  UINT64 loaCount;       // Alignment Loss counter  
  UINT64 lenMismatchCount;
} 
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RtsdHWStatChannelType;


// Rtsd ATM IMA status type, used to convey the IMA status information per port
//  for a single side. See RTSD_HW_CNT_IMA_STATS format comments above for additional
//  usage.
typedef struct
{
   UINT8    m_status;  //Bit mapped per port status (1:link is bad,0:link is good)
   UINT8    m_ica;     //Bit mapped per port status (1:icp cell is available,0:icp cell is not available)
   UINT32   m_linkInfo[NUM_OF_PORTS][2]; // per port status of the 32 links
                          // where each 2-bit group represent status states:
                          //   IMA_LINK_STATUS_NOT_IN_GROUP
                          //   IMA_LINK_STATUS_UNUSABLE
                          //   IMA_LINK_STATUS_USABLE
                          //   IMA_LINK_STATUS_ACTIVE
                          //   (as defined in LimSharedDefs.h)
                          // Starting from LSB, linkInfo[N][0] contains
                          // the status of the 1st 16 links on portN, and
                          // linkInfo[N][1] contains the status of the other 16 links
                          // on portN.
   UINT8    m_group[NUM_OF_PORTS];// Per port IMA group number when ica is set for a specific port(otherwise N/A)
                             //   There may not be any of these if no ica bits are set.
   UINT8    m_link[NUM_OF_PORTS]; // Per port IMA link number when ica is set for a specific port(otherwise N/A)
                             //   There may not be any of these if no ica bits are set.
   UINT32   m_delay[NUM_OF_PORTS];// Per port link delay for a specific port (otherwise N/A(0xffffffff))
}
#ifndef WIN32
__attribute__ ((packed)) //VxWorks packed
#endif
RstdHwStatImaStatusType;


#ifdef WIN32
#pragma pack( pop, RTSDTIME1 )
#endif

#ifdef WIN32
typedef struct 
{
   UINT32             m_nStatCount ;
   union
   {
     RtsdHWStatDlciType  m_dlciStat[MAX_DLCI_VPVC_STATS] ; // 0 to 1024(current hardware limit) of DLCI stats.
     RtsdHWStatVpvcType  m_vpvcStat[MAX_DLCI_VPVC_STATS] ; // 0 to 1024(current hardware limit) of VpVc stats.
     RtsdHWStatChannelType m_channelStat[MAX_CHANNEL_STATS] ; // 0 to 1024(current hardware limit) of Channel stats.
     RtsdHWStatHDLCTribType m_hdlcTribStat[MAX_HDLCTRIB_STATS]; // 0 to 336(current hardware limit) of HDLC Trib stats.
     RtsdHWStatATMTribType  m_atmTribStat[MAX_ATMTRIB_STATS]; // 0 to 336(current hardware limit) of ATM Trib stats.
   };
} RtsdHwStatUnionType [N_SIDES]; // Line & Eqpt
#endif

// Structure used to hold all current (last latched) counter values. This
//   structure is used to determine differences for counter update packets.
typedef struct
{
    UINT32                m_nSecs ;
    UINT32                m_nNsecs;

    RtsdHWLimCountersType m_limCounters;
    RtsdHWLimStatusType   m_limLineStatus;

    RtsdHWStatType        m_eStatType; // see enum above..

    #ifdef WIN32
    RtsdHwStatUnionType   m_stats;
    // The following IMA status info are only present when running in ATM IMA modes
    BOOL    m_bImaStats;        // TRUE if IMA stats present.
    UINT8   m_uImaPorts;        // IMA bit mapped port value: 1=port is selected 0=port is not selected
    UINT8   m_uImaReceiverMode; // IMA receiver mode
    RstdHwStatImaStatusType m_imaStats[N_SIDES] ;
    #else 
      // WEI NOTE: VxWorks 
      //   We could define the above union with StatDlci and StatVpVc for
      //   the VxWorks side, but it is not necessary so we save space in
      //   this structure.
    #endif

} RtsdHWCountersType;

// Structure used to hold all current (last latched) counter values plus the
// unique interface key of the interface for the counters.
#if defined(WIN32) && !defined(WIN_ELWOOD)
typedef struct
{
    TCHAR strIfKey[64];
    RtsdHWCountersType RtsdHWCounters;
} RtsdHWCountersTypeEx;
#endif

#define RTSDHWCOUNTERS 1
#define RTSDSTATUS     2
#define RTSDPPCOUNTERS 3

// Sructure used to hold status information created by recorderGUI
#if defined(WIN32) && !defined(WIN_ELWOOD)
typedef struct
{
    float fMBWritten;
    float fCurrentMbps;
    float fAvgMbps;
    float fMaxMbps;
    UINT64 nFrames;
} RtsdStatusType;
#endif

// Packet size calculations from above definitions. (currently this is 528)
#define MAX_RTSDHWCNT_BASECNT_SIZE    (2*sizeof(UINT64) + 2*( sizeof(UINT64) * 32 ) )

#ifdef WIN32
// ATM VpVc counters in the packet (2 UINT16 row counts, 1 UINT32 flags, 2 sides of 14 stats
//                                    (each of which can be upto UINT64) for upto MAX_DLCI_VPVC_STATS VPVCs).
//   which when currently calculated is 253960
#define MAX_RTSDHWCNT_ATMCNT_SIZE     (2*sizeof(UINT16) + sizeof(UINT32) + \
                                        2* (MAX_DLCI_VPVC_STATS*((3* sizeof(UINT32)) + (14*sizeof(UINT64)))) )

// FR Dlci counters in the packet (2 UINT16 row counts, 1 UINT32 flags, 2 sides of 12 stats
//                                    (each of which can be upto UINT64) for upto MAX_DLCI_VPVC_STATS DLCIs).
//#define MAX_RTSDHWCNT_FRCNT_SIZE     (2*sizeof(UINT16) + sizeof(UINT32) + \
//                                        2* (MAX_DLCI_VPVC_STATS* ((2* sizeof(UINT32)) + (12*sizeof(UINT64)) ) ) )
#else // VxWorks
// On VxWorks the max size of each stat in the VpVc and Dlci is only UINT32 
// ATM VpVc counters in the packet (2 UINT16 row counts, 1 UINT32 flags, 2 sides of 14 stats
//                                    (each of which can be upto UINT32) for upto MAX_DLCI_VPVC_STATS VPVCs).
//   which when currently calculated is 139272
#define MAX_RTSDHWCNT_ATMCNT_SIZE     (2*sizeof(UINT16) + sizeof(UINT32) + \
                                          2* (MAX_DLCI_VPVC_STATS*((3* sizeof(UINT32)) + (14*sizeof(UINT32)))) )

// FR Dlci counters in the packet (2 UINT16 row counts, 1 UINT32 flags, 2 sides of 12 stats
//                                    (each of which can be upto UINT32) for upto MAX_DLCI_VPVC_STATS DLCIs).
//#define MAX_RTSDHWCNT_FRCNT_SIZE     (2*sizeof(UINT16) + sizeof(UINT32) + \
//                                        2* (MAX_DLCI_VPVC_STATS* ((2* sizeof(UINT32)) + (12*sizeof(UINT32)) ) ) )

#endif

// ATM IMA status counters in the packet (2 UINT8 row counts, 
//    for each side ( 3 UINT8s fields, 16 UINT32 linkInfo fields,
//                    8 UINT8 group numbers, 8 UINT8 link numbers
//                    8 UINT32 delayvalues )
// 
#define MAX_RTSDHWCNT_ATMIMA_SIZE (( 3 + 16*sizeof(UINT32) + NUM_OF_PORTS*3 ) * 2 )
//   which when currently calculated is 182

// Calculate the max rtsd hardware counter packet data size. 
//  Currently on WIN32 this is 259432 and VxWorks this is 144744.
#define MAX_RTSDHWCNT_PACKETDATA_SIZE ( (2*MAX_RTSDHWCNT_BASECNT_SIZE ) + \
                                        (NUM_OF_PORTS*MAX_RTSDHWCNT_BASECNT_SIZE) + \
                                        ( NUM_OF_PORTS * 2 * sizeof(LimLineStatus)) + \
                                        MAX_RTSDHWCNT_ATMCNT_SIZE + \
                                        MAX_RTSDHWCNT_ATMIMA_SIZE )


#ifdef WIN32
#pragma pack( push, RTSDHWCNTPKT1, 1 )
#endif
//  Simple "variable" length packet struct for "absolute" and "difference" hardware counter updates.
//  The m_packetData portion is maximum size based on the above hardware counter struct,
//  but is loaded with only differences on subsequent hardware counter
//  updates. In these cases, only the portion of the packet needed for the update is sent.
typedef struct
{
    UINT32                m_uSocketHeaderData;
    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
    UINT32                m_nCountersPresent;  // BitMapped counters present flags
    UINT8                 m_packetData[MAX_RTSDHWCNT_PACKETDATA_SIZE+4]; //4 extra bytes for Debug tag
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWCountersPacketType ;

// WEI NOTE:
//   The following constant tells the maximum size of the RTSM packet.
//   This size MUST handle the max hardware counters on ATM systems
//    (see MAX_RTSDHWCNT_PACKETDATA_SIZE) being ~145k on VxWorks, but we
//    we want to set this have some pad in it and on a even 1K bountry.
//   NOTE: on older TCP based only systems, the "block" and packet size was limited
//         to 64000 due to the RTSDQueue setup.
// SAB NOTE: 12/09/2009
//   Increased Maximum RTSM packet size for BladeRocket(10Gig Ethernet) using 256*1024 (256K) block sizes
//   and allowing hardware buffer overrun of up to 32K. ---> this is for frame data from
//   the 10G card NOT the hardware counters from from the card.
//   NOTE: Since the blade rocket card is ONLY PC side code, NOT VxWorks, we leave the
//     setup for the MAX_RTSM_PACKET_SIZE as setup from ONLY the RtsdHWCountersPacketType for VxWorks
#ifdef WIN32
static const UINT32 MAX_RTSM_PACKET_SIZE = RTSD_MAX((256+32)*1024, sizeof(RtsdHWCountersPacketType) + (0x0400 -( sizeof(RtsdHWCountersPacketType) & 0x03FF )));
#else // VxWorks
static const UINT32 MAX_RTSM_PACKET_SIZE = sizeof(RtsdHWCountersPacketType) + (0x0400 -( sizeof(RtsdHWCountersPacketType) & 0x03FF ));
#endif

#define MAX_FP_DEMUX_BUFFER_SIZE ((MAX_RTSM_PACKET_SIZE + sizeof(ubit32)) * 2)
#define MAX_SIPDECOMP_BUFFER_SIZE ((MAX_RTSM_PACKET_SIZE + sizeof(ubit32)) * 2)
// The following unions are used to support the hardware LIM configuration
typedef union 
{
  LimL1CfgT1E1     t1e1L1Config ;
  LimL1CfgT3E3     t3e3L1Config ;
  LimL1CfgOC3      oc3L1Config ;
  LimL1CfgVseries  vseriesL1Config ;
  LimL1CfgLAN      lanL1Config ;
  LimL1CfgWLAN     wlanL1Config;
  LimL1CfgHssi     hssiL1Config ;
  LimL1Cfg8PortT1E1 t1e18PortL1Config ;
  LimL1CfgOC12      oc12L1Config ;  
  LimL1CfgMPortLANEx    mportlanL1Config;
  LimL1Cfg5Port10GLANEx tenGig5PortL1Config;
  LimL1CfgPacketPortalEx  ppL1Config;
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWLayer1Config ;

typedef union
{
  LimL2CfgFR    frL2Config ;
  LimL2CfgBOP   bopL2Config ;
  LimL2CfgATM   atmL2Config ;
  LimL2CfgPPP   pppL2Config ;
  LimL2CfgEther lanL2Config ;
  LimL2CfgLAPD  lapdL2Config;
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWLayer2Config ; 


// For Hardware LIM configuration subtype(RTSD_OTHER_SUBTYPE_LIM_CONFIG), the following
// overlayed general structure is defined. The Layer1 and Layer2 configurations
// are packed into their smallest size as appropriate with a UINT16 for size
// in bytes before each(to allow for changes in the structures without killing
// any older clients in the future). The basic form is :
//    UINT16 layer1size;
//    LimL1xxx layer1config;
//    UINT16 layer2size;
//    LimL2xxx layer2config;
// The structure below allows for the MAXIMUM size, but ONLY amount of space required
// is actually sent.
// So for example, for V-Series with FrameRelay, the structure would be:
//    UINT16          layer1size = 44(sizeof(LimL1CfgVseries));
//    LimL1CfgVseries layer1config;
//    UINT16          layer2size= 4(sizeof(LimL2CfgFR)) ;
//    LimL2CfgFR      layer2config;
//  For a total of 2 + 44 + 2 + 4 = 52 bytes.
typedef struct
{
    UINT32                m_uSocketHeaderData;
    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
    UINT8                 m_limConfig[ sizeof(UINT16)+ sizeof(RtsdHWLayer1Config) + 
                                       sizeof(UINT16)+ sizeof(RtsdHWLayer2Config) ];
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWLimConfigPacketType ;



// For Hardware LIM link status subtype(RTSD_OTHER_SUBTYPE_LINK_STATUS), the following
// overlayed general structure is defined.
typedef struct
{
    UINT32                m_uSocketHeaderData;
    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
    UINT32                m_nLimStatus[3]; // LIM specific status
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWLinkStatusPacketType ;


// For Hardware SAR configuration subtype(RTSD_OTHER_SUBTYPE_SAR_CONFIG), the following
// overlayed general structure is defined. The SAR configuration is
// packed into their smallest size as appropriate with a UINT16 for size
// in bytes before each(to allow for changes in the structures without killing
// any older clients in the future). The basic form is :
//    SARDrvGenConfig       m_sarConfig ;
//    SARDrvVcDefinitionSet m_sarDefnSet ; //note: count field contains number of
//                                                 SARDrvVcDefinition that follow.
//    SARDrvVcDefinition    defn[x]; 
typedef struct
{
    UINT32                m_uSocketHeaderData;
    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
    SARDrvGenConfig       m_sarConfig ;
    SARDrvVcDefinitionSet m_sarDefSet; // note: count field contains number of SARDrvVcDefinition
                                       //  that follow
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWLimSarConfigPacketType ;

/////////////////////////////////////////////////////////////////////////////
// Rtsd User Plane Statistics for SART Rocket
// These are encode in a RTSM "other" message (type 0) with the subType
// set to RTSD_OTHER_SUBTYPE_UP_STATISTICS (6)
/////////////////////////////////////////////////////////////////////////////

// max size of a signle UP Stats block that can be processed by SART:
//  Note: this size is below the size of the hardware counter (RtsdHWCountersPacketType)
//    so we don't have to worry about this value affecting the MAX_RTSM_PACKET_SIZE
//    above.
#ifdef __cplusplus
static const UINT32 MAX_RTSM_UPSTATS_PACKET_SIZE = 64000;
#else
// Make old C code that is not compiled as C++ work too...
#define MAX_RTSM_UPSTATS_PACKET_SIZE 64000
#endif

// current versions for UP Stats - should be updated any time a structure
// below is modified or added
#define RTSD_UP_STATS_VERSION_MAJOR 0
#define RTSD_UP_STATS_VERSION_MINOR 2

#define RTSD_MAX_VLANMPLS_IDS 8

// VLAN/MPLS FLAGS
#define RTSD_VLAN_FLAG        0x80000000
#define RTSD_MPLS_FLAG        0x40000000

#define RTSD_MORE_PACKETS_FLAG 0x01

#define RTSD_UP_STATS_TUNNEL_TYPE_GTP 0   // GTP Tunnel
#define RTSD_UP_STATS_TUNNEL_TYPE_GRE 1	  // GRE Tunnel

#define RTSD_UP_STATS_IPV4          0       // IPv4
#define RTSD_UP_STATS_IPV6          1       // IPv6

// Entry types
#define RTSD_UP_STATS_TUNNEL_ENTRY 0        // IPv4 or IPv6 tunnel entry
#define RTSD_UP_STATS_CONN_ENTRY 1          // IPv4 or IPv6 connection entry
#define RTSD_UP_STATS_TUNNEL_SUB_ENTRY 2    //
#define RTSD_UP_STATS_CONN_SUB_ENTRY 3    

#define RTSD_UP_STATS_LAYER_ONE    0x00     // layer 1 entry for all stats type
#define RTSD_UP_STATS_LAYER_TWO    0x01	    // layer 2 entry for all stats type
#define RTSD_UP_STATS_LAYER_THREE  0x02	    // future layer 3? Not in use
#define RTSD_UP_STATS_LAYER_FOUR   0x03	    // future layer 4? Not in use

// SAB NOTE: the following UP Stats structures were added for SARTRocket
// UP Stats formats
typedef struct
{
   UINT8   versionMajor;         // Major Version – initially 1 at first release
   UINT8   versionMinor;         // Minor Version – initially 0 at first release
   UINT8   flags;                // bit 0 => 1 = more packets
   UINT8   nReserved1;
   UINT32  nSampleStartTimeSecs; // seconds since Jan 1, 1970
   UINT32  nSampleStartTimeNSecs;
   UINT32  nSampleEndTimeSecs;
   UINT32  nSampleEndTimeNSecs;
   UINT32  nTunneledEntries;     // Number of tunnel entries
   UINT32  nTunneledEntrySize;   // Size of tunnel entries (offset to Conn entries)
   UINT32  nConnEntries;         // Number of Connection entries (non-tunneled)
   UINT32  nConnEntrySize;       // Size of connection entries
   UINT32  nOtherEntries;        // Number of entries (non-tunneled)
   UINT32  nOtherEntrySize;      // Size of other entries
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtUPStatsHeader;

// 
// RtsdBufFmtUPStatsEntryHeader defines a standard header for all entries
// in UP Stats
typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    UINT16 nLength;             // Length of structure
    UINT8  uEntryType:4;        // tells what type of entry this is
    UINT8  upStatsLayer:2;      // 00 = layer 1, 01 = layer 2, ...
    UINT8  bIPVersion:1;        // 0 = IPv4, 1 = IPv6
    UINT8  other:1;             // free bit - may be used by derived structures
    UINT8  nSubElements;        // number of nested sub-elements following
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtUPStatsEntryHeader;

//
// RtsdBufFmtUPStatsEntryCount defines the statistical count for the link, transport
// and frame level
typedef struct
{
    // Counts                   // should these be 32 or 64 bit?
    UINT64 uLinkBytes;          // Aggregated link (Ethernet) bytes for all traffic in the tunnel
    UINT64 uTransportBytes;     // Aggregated link Transport bytes for all traffic in the tunnel
    UINT64 uFrames;
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtUPStatsEntryCount;

typedef struct
{
// Starts with standard 16 bit lenght, and 8 bit type
    RtsdBufFmtUPStatsEntryHeader header; // uEntryType = RTSD_UP_STATS_TUNNEL_ENTRY
    UINT8  bTunnelType:2;       // 0 = GTP, 1 = GRE, 2 & 3 reserved
    UINT8  unused:1;
    UINT8  nLimPortNum:5;       // port number of the characterized LIM
    UINT8  unused2;             // pad to 32-bit word allignment
    UINT16 unused3;
// --- BEGIN KEY --- 
    // VLAN & MPLS Info
    // up to 8 layers, a 0 value means end of list
    UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS]; // upper bits may be used to tell between VLAN/MPLS
    UINT32 ipLowerSrc;
    UINT32 ipLowerDest;
    // Tunnel info
    UINT32 TEI;                 // GTP TEID, or GRE Key
// --- END KEY --- 
RtsdBufFmtUPStatsEntryCount upStatsCount;
    // followed by 0 or more sub-elements of the possible types:
    //    RtsdBufFmtUPStatsTunnelSubEntry - for IP Upper flows within the tunnel
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtUPStatsIPv4TunnelEntry;
// Likewise, IPv6 would be the same structure with IPV6 addresses

typedef struct
{
    // Starts with standard 16 bit lenght, and 8 bit type
    RtsdBufFmtUPStatsEntryHeader header; // uEntryType = RTSD_UP_STATS_TUNNEL_SUB_ENTRY
    // --- BEGIN KEY --- 
    UINT32 ipUpperSrc;               // IP Upper Source Address
    UINT32 ipUpperDest;              // IP Upper Destination Address
    UINT8  ipUpperProtocol;          // IP Next protocol field
    UINT16 srcPort;             // TCP/UDP Source Port
    UINT16 destPort;            // TCP/UDP Destination Port
    // --- END KEY --- 
RtsdBufFmtUPStatsEntryCount upStatsCount;
    // followed by 0 or more sub-elements:
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtUPStatsIPv4TunnelSubEntry;

typedef struct
{
    // Starts with standard 16 bit lenght, and 8 bit type
    // type information
    RtsdBufFmtUPStatsEntryHeader header; // uEntryType = RTSD_UP_STATS_CONN_SUB_ENTRY
    // --- BEGIN KEY --- 
    UINT8  unused:3;
    UINT8  nLimPortNum:5;       // port number of the characterized LIM
    UINT8  unused2;             // pad to 32-bit word allignment
    UINT16 unused3;
    // VLAN & MPLS Info
    // up to 8 layers, a 0 value means end of list
    UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS]; // upper bits may be used to tell between VLAN/MPLS
    UINT32 ipLowerSrc;
    UINT32 ipLowerDest;
    // --- END KEY --- 
RtsdBufFmtUPStatsEntryCount upStatsCount[2]; // statsCount[0] for uplink 
    // and statsCount[1] for downlink
    // followed by 0 or more sub-elements of the possible types:
    //    RtsdBufFmtUPStatsIPv4ConnSubEntry
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtUPStatsIPv4ConnEntry;

typedef struct
{
    // Starts with standard 16 bit lenght, and 8 bit type
    RtsdBufFmtUPStatsEntryHeader header; // UINT8 bitmap set to 000000100
    // --- BEGIN KEY --- 
    UINT8  ipProtocol;          // IP Next protocol field
    UINT8  reserved1;
    UINT16 reserved2;
    UINT16 srcPort;             // TCP/UDP Source Port
    UINT16 destPort;            // TCP/UDP Destination Port
    // --- END KEY --- 
RtsdBufFmtUPStatsEntryCount upStatsCount[2]; // statsCount[0] src to dest 
    // and statsCount[1] dest to src

    // followed by 0 or more sub-elements:
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtUPStatsIPv4ConnSubEntry;


// Generalized packet type structure for UP stats subtype(RTSD_OTHER_SUBTYPE_UP_STATISTICS)...
typedef struct
{
    UINT32                m_uSocketHeaderData;
    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
    UINT8                 m_upStats[MAX_RTSM_UPSTATS_PACKET_SIZE]; // UP specific stats using definitions above.
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWUPStatsPacketType ;


///////////////////////////// END OF UP STATS ///////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Rtsd Video/Voice Quality Statistics for SART
// These are encode in a RTSM "other" message (type 0) with the subType
// set to RTSD_OTHER_SUBTYPE_VQ_STATISTICS (10)
/////////////////////////////////////////////////////////////////////////////

// max size of a single VQ Stats block that can be processed by SART:
//  Note: this size is below the size of the hardware counter (RtsdHWCountersPacketType)
//    so we don't have to worry about this value affecting the MAX_RTSM_PACKET_SIZE
//    above.
#ifdef __cplusplus
static const UINT32 MAX_RTSM_VQSTATS_PACKET_SIZE = 64000;
#else
// Make old C code that is not compiled as C++ work too...
#define MAX_RTSM_VQSTATS_PACKET_SIZE 64000
#endif

// current versions for VQ Stats - should be updated any time a structure
// below is modified or added
#define RTSD_VQ_STATS_VERSION_MAJOR 0
#define RTSD_VQ_STATS_VERSION_MINOR 1

#define RTSD_VQ_STATS_TUNNEL_TYPE_GTP 0   // GTP Tunnel
#define RTSD_VQ_STATS_TUNNEL_TYPE_GRE 1	  // GRE Tunnel

#define RTSD_VQ_STATS_IPV4          0       // IPv4
#define RTSD_VQ_STATS_IPV6          1       // IPv6

// Entry types
#define RTSD_VQ_STATS_TUNNEL_ENTRY 0        // IPv4 or IPv6 tunnel entry
#define RTSD_VQ_STATS_CONN_ENTRY 1          // IPv4 or IPv6 connection entry
//#define RTSD_VQ_STATS_TUNNEL_SUB_ENTRY 2    //
//#define RTSD_VQ_STATS_CONN_SUB_ENTRY 3    

#define RTSD_VQ_STATS_LAYER_ONE    0x00     // layer 1 entry for all stats type
#define RTSD_VQ_STATS_LAYER_TWO    0x01	    // layer 2 entry for all stats type
#define RTSD_VQ_STATS_LAYER_THREE  0x02	    // future layer 3? Not in use
#define RTSD_VQ_STATS_LAYER_FOUR   0x03	    // future layer 4? Not in use

// The following VQ Stats structures were added for SART
// VQ Stats formats
typedef struct
{
    UINT8   versionMajor;         // Major Version – initially 0 at first release
    UINT8   versionMinor;         // Minor Version – initially 1 at first release
    UINT8   flags;                // bit 0 => 1 = more packets
    UINT8   nReserved1;
    UINT32  nSampleStartTimeSecs; // seconds since Jan 1, 1970
    UINT32  nSampleStartTimeNSecs;
    UINT32  nSampleEndTimeSecs;
    UINT32  nSampleEndTimeNSecs;
    UINT32  nTunneledEntries;     // Number of tunnel entries
    UINT32  nTunneledEntrySize;   // Size of tunnel entries (offset to Conn entries)
    UINT32  nConnEntries;         // Number of Connection entries (non-tunneled)
    UINT32  nConnEntrySize;       // Size of connection entries
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsHeader;

// 
// RtsdBufFmtVQStatsEntryHeader defines a standard header for all entries
// in VQ Stats
typedef struct
{
    UINT8  bTunnel:1;           // tells if this entry is tunnel or not
    UINT8  uStatsLayer:2;       // 00 = layer 1, 01 = layer 2, ...
    UINT8  uStatsDir:2;         // stats presence: 01: src->dst; 10: dst->src; 00: none; 11: both
    UINT8  bIPVersion:1;        // 0 = IPv4, 1 = IPv6
    UINT8  unused:2;            // free bit - may be used by derived structures
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsEntryHeader;

enum RtsdBufFmtVQStatsType{RtsdBufFmtVQStatsTypeBegin_T=1, 
    RtsdBufFmtVQStatsEntryCountTypeMOS_T=RtsdBufFmtVQStatsTypeBegin_T, 
    RtsdBufFmtVQStatsEntryCountTypeEnd_T};

//
// RtsdBufFmtVQStatsEntryCount defines the statistical count for Video/Voice Quality
typedef float RtsdBufFmtVQStatsMOS;
typedef struct
{
    UINT8                VQStatsType;  // RtsdBufFmtVQStatsEntryCountTypeMOS_T
    RtsdBufFmtVQStatsMOS MOS_Audio; // Audio MOS score
    RtsdBufFmtVQStatsMOS MOS_Video; // Video MOS score
    RtsdBufFmtVQStatsMOS MOS_Voice; // Voice MOS score
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsEntryCountTypeMOS;

#define RtsdBufFmtVQStatsEntryCount_BlockMaxSize 256
typedef struct
{
    UINT8 nTypeCount;              // RtsdBufFmtVQStatsType
    UINT8 Block[RtsdBufFmtVQStatsEntryCount_BlockMaxSize];                // struct such as RtsdBufFmtVQStatsEntryCountTypeMOS, ... 
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsEntryCount;

#define NVLANMPLS_BITS 5
#define NVLANMPLS_MASK ((1<<NVLANMPLS_BITS)-1)

typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    RtsdBufFmtVQStatsEntryHeader header; // uEntryType = RTSD_VQ_STATS_TUNNEL_ENTRY
    UINT8  bTunnelType:2;       // 0 = GTP, 1 = GRE, 2 & 3 reserved
    UINT8  unused:1;
    UINT8  nLimPortNum:5;       // port number of the characterized LIM
    //UINT8  unused2;             // pad to 32-bit word alignment
    //UINT16 unused3;
    // --- BEGIN KEY --- 
    UINT8 ipLowerSrc[4];
    UINT8 ipLowerDest[4];
    // Tunnel info
    UINT32 TEI;                 // GTP TEID, or GRE Key
    // VLAN & MPLS Info
    // up to 8 layers
    UINT8  unused2:(8-NVLANMPLS_BITS);
    UINT8  nVLANMPLSIds:NVLANMPLS_BITS;      // number of VLAN/MPLS Ids, 0-8
    UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS]; // upper bits may be used to tell between VLAN/MPLS
    // --- END KEY --- 
    // followed by 0 or more sub-elements of the possible types:
    //    RtsdBufFmtVQStatsTunnelSubEntry - for IP Upper flows within the tunnel
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsIPv4TunnelEntry;

typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    RtsdBufFmtVQStatsEntryHeader header; // uEntryType = RTSD_VQ_STATS_TUNNEL_ENTRY
    UINT8  bTunnelType:2;       // 0 = GTP, 1 = GRE, 2 & 3 reserved
    UINT8  unused:1;
    UINT8  nLimPortNum:5;       // port number of the characterized LIM
    //UINT8  unused2;             // pad to 32-bit word alignment
    //UINT16 unused3;
    // --- BEGIN KEY --- 
    UINT8 ipLowerSrc[16];
    UINT8 ipLowerDest[16];
    // Tunnel info
    UINT32 TEI;                 // GTP TEID, or GRE Key
    // VLAN & MPLS Info
    // up to 8 layers
    UINT8  unused2:(8-NVLANMPLS_BITS);
    UINT8  nVLANMPLSIds:NVLANMPLS_BITS;      // number of VLAN/MPLS Ids, 0-8
    UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS]; // upper bits may be used to tell between VLAN/MPLS
    // --- END KEY --- 
    // followed by 0 or more sub-elements of the possible types:
    //    RtsdBufFmtVQStatsTunnelSubEntry - for IP Upper flows within the tunnel
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsIPv6TunnelEntry;

typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    RtsdBufFmtVQStatsEntryHeader header; // uEntryType = RTSD_VQ_STATS_TUNNEL_SUB_ENTRY
    // --- BEGIN KEY --- 
    UINT8 ipUpperSrc[4];             // IP Upper Source Address
    UINT8 ipUpperDest[4];            // IP Upper Destination Address
    UINT8 ipUpperProtocol;           // IP Next protocol field
    //UINT8  unused1;             // pad to 32-bit word alignment
    //UINT16 unused2;
    UINT16 srcPort;             // TCP/UDP Source Port
    UINT16 destPort;            // TCP/UDP Destination Port
    // --- END KEY --- 
    RtsdBufFmtVQStatsEntryCount tStatsCount; // src->dst
    // followed by 0 or more sub-elements:
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsIPv4TunnelSubEntry;

typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    RtsdBufFmtVQStatsEntryHeader header; // uEntryType = RTSD_VQ_STATS_TUNNEL_SUB_ENTRY
    // --- BEGIN KEY --- 
    UINT8 ipUpperSrc[16];            // IP Upper Source Address
    UINT8 ipUpperDest[16];           // IP Upper Destination Address
    UINT8 ipUpperProtocol;           // IP Next protocol field
    //UINT8  unused1;             // pad to 32-bit word alignment
    //UINT16 unused2;
    UINT16 srcPort;             // TCP/UDP Source Port
    UINT16 destPort;            // TCP/UDP Destination Port
    // --- END KEY --- 
    RtsdBufFmtVQStatsEntryCount tStatsCount; // src->dst
    // followed by 0 or more sub-elements:
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsIPv6TunnelSubEntry;

typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    // type information
    RtsdBufFmtVQStatsEntryHeader header; // uEntryType = RTSD_VQ_STATS_CONN_SUB_ENTRY
    // --- BEGIN KEY --- 
    UINT8  unused:3;
    UINT8  nLimPortNum:5;       // port number of the characterized LIM
    //UINT8  unused2;             // pad to 32-bit word alignment
    //UINT16 unused3;
    UINT8 ipLowerSrc[4];
    UINT8 ipLowerDest[4];
    // VLAN & MPLS Info
    // up to 8 layers
    UINT8  unused2:(8-NVLANMPLS_BITS);
    UINT8  nVLANMPLSIds:NVLANMPLS_BITS;      // number of VLAN/MPLS Ids, 0-8
    UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS]; // upper bits may be used to tell between VLAN/MPLS
    // --- END KEY --- 
    // followed by 0 or more sub-elements of the possible types:
    //    RtsdBufFmtVQStatsIPv4ConnSubEntry
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsIPv4ConnEntry;

typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    // type information
    RtsdBufFmtVQStatsEntryHeader header; // uEntryType = RTSD_VQ_STATS_CONN_SUB_ENTRY
    // --- BEGIN KEY --- 
    UINT8  unused:3;
    UINT8  nLimPortNum:5;       // port number of the characterized LIM
    //UINT8  unused2;             // pad to 32-bit word alignment
    //UINT16 unused3;
    UINT8 ipLowerSrc[16];
    UINT8 ipLowerDest[16];
    // VLAN & MPLS Info
    // up to 8 layers
    UINT8  unused2:(8-NVLANMPLS_BITS);
    UINT8  nVLANMPLSIds:NVLANMPLS_BITS;      // number of VLAN/MPLS Ids, 0-8
    UINT32 aVLANMPLSIds[RTSD_MAX_VLANMPLS_IDS]; // upper bits may be used to tell between VLAN/MPLS
    // --- END KEY --- 
    // followed by 0 or more sub-elements of the possible types:
    //    RtsdBufFmtVQStatsIPv4ConnSubEntry
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsIPv6ConnEntry;

typedef struct
{
    // Starts with standard 16 bit length, and 8 bit type
    RtsdBufFmtVQStatsEntryHeader header; // UINT8 bitmap set to 000000100
    // --- BEGIN KEY --- 
    UINT8  ipProtocol;          // IP Next protocol field
    //UINT8  unused1;             // pad to 32-bit word alignment
    //UINT16 unused2;
    UINT16 srcPort;             // TCP/UDP Source Port
    UINT16 destPort;            // TCP/UDP Destination Port
    // --- END KEY --- 
    RtsdBufFmtVQStatsEntryCount tStatsCount[2]; // see RtsdBufFmtVQStatsEntryHeader:uStatsDir

    // followed by 0 or more sub-elements:
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtVQStatsIPv4ConnSubEntry;

typedef RtsdBufFmtVQStatsIPv4ConnSubEntry RtsdBufFmtVQStatsIPv6ConnSubEntry;

// Generalized packet type structure for UP stats subtype(RTSD_OTHER_SUBTYPE_VQ_STATISTICS)...
typedef struct
{
    UINT32                m_uSocketHeaderData;
    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
    UINT8                 m_upStats[MAX_RTSM_VQSTATS_PACKET_SIZE]; // VQ specific stats using definitions above.
} 
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWVQStatsPacketType ;


///////////////////////////// END OF VQ STATS ///////////////////////////////

///////////////////////////// BEGIN OF PACKET PORTAL ///////////////////////////////

// Packet type structure for Packet Portal probe map table
typedef struct
{
    UINT8 ProbeId[6];   // 6 bytes ProbeID
    UINT8 MapId[2];     // 2 bytes MapID in liminfo
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtPPProbeIdMapEntry;

typedef struct
{
    UINT32 nCount;      // Number of map entries
    RtsdBufFmtPPProbeIdMapEntry Entries[1]; // Array of map entries
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdBufFmtPPProbeIdMap;

typedef struct
{
    UINT32                m_uSocketHeaderData;
    RtsdBufFmtOtherHeader m_rtsdOtherHeader;
    RtsdBufFmtPPProbeIdMap m_ProbeIdMap;
}
#ifndef WIN32
__attribute__ ((packed)) // VxWorks packing of structure.
#endif
RtsdHWPPPacketType;

#if defined(WIN32) && !defined(WIN_ELWOOD)
typedef struct
{
    TCHAR  strProbeId[CONST_PROBID_LENGTH];
    UINT64 uAccuGap;                          //the accumulative sequence gap
}RtsdCounterForPPProbeAccuGapEntry;
#endif

#if defined(WIN32) && !defined(WIN_ELWOOD)
typedef struct
{
    UINT32 nCount;                     // Number of probes
    RtsdCounterForPPProbeAccuGapEntry  AccuGap[1];
}RtsdCounterForPPProbeAccuGap;
#endif

#if defined(WIN32) && !defined(WIN_ELWOOD)
typedef struct
{
    TCHAR  strIfKey[64];
    TCHAR  strProbeId[CONST_PROBID_LENGTH];
    RtsdHWCountersType RtsdHWCounters;
} RtsdHWCountersForPacketPortalProbeEntry;
#endif

#if defined(WIN32) && !defined(WIN_ELWOOD)
typedef struct
{
    TCHAR  strIfKey[64];
    TCHAR  strProbeId[CONST_PROBID_LENGTH];
    UINT64 ProbeSequenceGap[N_SIDES];
} RtsdProbeSequenceGap;
#endif

//need to consider move below to DNAMeasurementexport.h file.
//#if defined(WIN32) && !defined(WIN_ELWOOD)
//typedef struct
//{
//    UINT32 nCount;
//    RtsdHWCountersForPacketPortalProbeEntry  ProbeCounter[1];
//} RtsdHWCountersForPacketPortalProbe;
//#endif
///////////////////////////// END OF PACKET PORTAL ///////////////////////////////


#ifdef WIN32
#pragma pack( pop, RTSDHWCNTPKT1 )
#endif

// Encode the absolute or differencial packet from the RtsdHardwareCountersType
#ifdef WIN32
void RtsdHWEncode( BOOL bSendDiffs,
                   RtsdHWCountersPacketType * pDest,
                   UINT8                      uNumPorts,
                   const RtsdHWCountersType * pBaseCounters, 
                   const RtsdHWCountersType * pCurrentCounters );
#else 

// VxWorks specific encoder
void RtsdHWEncode( BOOL bSendDiffs,
                   RtsdHWCountersPacketType * pDest,
                   UINT8                      uNumPorts,
                   const RtsdHWCountersType * pBaseCounters, 
                   const RtsdHWLimCountersType * pLimCounters,
                   const RtsdHWLimStatusType * pLimLineStatus,
                   RtsdHWStatType  eStatType, // only looked at if pStatArrayLen&pStatArray is present.
                   const UINT16 (*pStatArrayLen)[N_SIDES],
                   const UINT8* (*pStatArray)[N_SIDES],
                   const UINT8* pLimAtmImaStats); // NULL if no IMA stats.

#endif

void RtsdHWDecode(RtsdHWCountersType * pBaseCounters,
                  RtsdHWCountersPacketType* pSrc);

#ifdef __cplusplus
}
#endif

#endif

