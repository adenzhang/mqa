#ifndef _LIMSHRDEFN_H
#define _LIMSHRDEFN_H
///////////////////////////////////////////////////////////////////////////////
//
// File       : LimSharedDefs.h
// Description: NA LIM Shared definitions. 
//
//              This is used by LimDriver, RTSDMeas and other system components
//              on the Xscale VxWorks system. It is also used on the
//              SART MU Server system for parsing information that is
//              sent on the RTSD data stream. For these reasons, these
//              "shared definitions" are in this separate H file so that
//              they are easily shared accross these and other apps.
//
//              NOTE: These were in LimDriver.h but were moved here.
//               
//              This is also used by the Elwood DLL system on the Network Analyzer
//              MU Client to decode the hardware counters and may in the future
//              be used by enhanced SART clients.
//
// Copyright(c) 2010-2011 by JDSU
//
// $Log: LimSharedDefs.h $
// Revision 50 2011/12/01 23:34:01 +0800 ive55253 /AnalyzerCommon/7.20.100_Tip/NA_PP
// Remove Agilent copyright -- JDSU owns the code including all copyrights.
// 
// Revision 49 2011/11/07 18:36:37 -0700 mor55175 /AnalyzerCommon/7.20.000_Tip/DNA_HD_SUPS
// Automatic merge of parallel changes into revision [47]
// 
// Revision 48 2011/11/03 15:39:51 -0600 ive55253 /AnalyzerCommon/7.20.000_Tip
// Fix constant value for CM_10_GIG that did not align with how the 10G driver actually get/set
// the value.
// 
// Revision 47 2011/10/30 16:56:54 -0600 mor55175 /AnalyzerCommon/7.20.000_Tip/DNA_HD_SUPS
// Automatic merge of parallel changes into revision [46]
// 
// Revision 46 2011/10/17 10:27:37 -0600 qua55168 /AnalyzerCommon/7.20.000_Tip/DNA_HD_SUPS/qua55168
// Split out two bits from the aggregation field of the multi-port gig LIM L1 configuration structure to hold the selected IPM mode.
// 
// Revision 45 2011/10/14 12:03:49 -0600 ive55253 /AnalyzerCommon/7.20.000_Tip/NaMultimode
// Automatic merge of parallel changes into revision [40]
// 
// Revision 44 2011/10/03 11:07:39 -0600 ive55253 /AnalyzerCommon/7.20.000_Tip/DTEnhancements/NA_PP
// Fix for NA DDS support and corrections for other comments.
// 
// Revision 43 2011/09/07 09:55:59 -0600 mor55175 /AnalyzerCommon/7.20.000_Tip/DNA_HD_SUPS/multi-pat-encaps/pre-6.70-merge/jm4399-6.70-merge
// Automatic merge of parallel changes into revision [37]
// 
// Revision 41 2011/08/26 07:41:22 -0600 ive55253 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.008/7.20.000.008.PacketPortal-Dev-BillIves
// Added first pass packet portal layer1 support defs and sturctures.
// 
// Revision 40 2011/08/22 20:24:20 -0600 qua55168 /AnalyzerCommon/7.20.000_Tip/NaMultimode
// Add NA multimode support.
// 
// Revision 37 2010/12/09 13:18:58 -0700 mah55258 /AnalyzerCommon/7.00.100_Tip/ANT_SASE_RELEASE_7.00.100.066/tm4457
// Add support for dynamic SUPS.
// 
// Revision 36 2010/10/19 12:33:02 -0600 kro55737 /AnalyzerCommon/7.00.100_Tip/TunnelingSlicing
// Implement Tunneling / Non-Tunneling Smart Slicing for Multiport Gig Lim - Change L1 Config Structure.
// 
// Revision 35 2010/09/03 11:46:43 -0600 bi4723 /AnalyzerCommon/6.65_Extensions/6.65.101/bi4723 Directional Network Vitals & RTP Stats Enhancements
// Remove now defunct 10G 4 port struct now that
// we have switch over to using 5port structure completely.
// 
// Revision 34 2010/07/29 16:02:02 -0600 tm4457 /AnalyzerCommon/6.65_Extensions/6.65.102/RecFilePlayback
// Add stronger checking for valid/invalid line speed and GBIC types.
// 
// Revision 31 2010/05/14 16:23:55 -0600 tm4457 /AnalyzerCommon/6.70.100_Tip/COS/SARTRocket/tm4457
// Add 5 port support for SARTRocket.
// 
// Revision 30 2010/02/01 19:55:54 -0700 sb4450 /AnalyzerCommon/6.70.100_Tip/06_Merge_SmartSlicing
// Automatic merge of parallel changes into revision [29]
// 
// Revision 29 2009/12/03 10:25:52 -0700 jkronba /AnalyzerCommon/6.70.100_Tip/SmartSlicing/jk4657
// Change LimPortCfgGeneral structure by adding SmartSlicing parameters. This affects the L1 Config for the Multiport Gig Lim.
// 
// Revision 28 2009/10/13 16:18:22 -0600 tm4457 /AnalyzerCommon/6.65.100_Tip/BladeRocket/tm4457
// Add new Ten Gig GBIC support functions.
// 
// Revision 27 2009/08/24 15:02:51 -0600 bi4723 /AnalyzerCommon/6.60.100_Tip/BladeRocket
// Fixed 10G count ordering
// 
// Revision 26 2009/08/20 15:51:00 -0600 bi4723 /AnalyzerCommon/6.60.100_Tip/BladeRocket
// Added 10G counters (now that they are slightly different than MP gig, we need
// separate constants).
// 
// Revision 25 2009/07/30 14:33:34 -0600 tm4457 /AnalyzerCommon/6.60.100_Tip/BladeRocket/tm4457
// Update LIM_LAN_GBICTypes for Ten Gig.
// 
// Revision 24 2009/03/31 11:39:19 -0600 tm4457 /AnalyzerCommon/6.50.100_Tip/BladeRocket/tm4457
// Add GBIC and line speed support functions
// 
// Revision 23 2009/03/10 11:26:44 -0600 tm4457 /AnalyzerCommon/6.50.100_Tip/BladeRocket/tm4457
// Add new GBIC Types for Ten Gig.
// 
// Revision 22 2009/02/04 14:25:46 -0700 bi4723 /AnalyzerCommon/6.50.100_Tip/BladeRocket
// Added first pass 10G layer1 config structures (these will change as we bring up the interface).
// 
// Revision 21 2009/01/27 11:58:44 -0700 bi4723 /AnalyzerCommon/6.50.100_Tip/BladeRocket
// Add 10G line speed const
// 
// Revision 20 2008/11/07 10:25:28 -0700 thohenry /AnalyzerCommon/6.50.100_Tip/BladeRocket/TH5605
// Update to include BladRocket card.
// 
// Revision 19 2008/02/29 20:35:00 -0700 jkronba /AnalyzerCommon/6.00.100_Tip/HDDNA
// Fix Tossed Frame counter indexes for Multiport Gigabit lim.
// 
// Revision 18 2008/02/20 14:12:11 -0700 thohenry /AnalyzerCommon/6.00.100_Tip/VLan/HDDNA
// Add tossed frame counters to 16 port gig lim counter type.
// 
// Revision 17 2008/01/17 11:13:15 -0700 jkronba /AnalyzerCommon/6.00.100_Tip/MobileDNA
// Implement K1K2 bytes config and swap config for 4-PortOC3 L1 structure
// 
// Revision 16 2008/01/16 10:27:13 -0700 jkronba /AnalyzerCommon/6.00.100_Tip/MobileDNA/UpDownLinkSwap
// Implement Up/DownLink Swap
// 
// Revision 15 2007/11/26 15:42:25 -0700 jkronba /AnalyzerCommon/6.00.100_Tip/MobileDNA
// merge with "lock breaking" version from Thomas Henry and Terry Mahoney.
// 
// Revision 14 2007/11/19 17:09:32 -0700 thohenry /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Automatic merge of parallel changes into revision [13]
// 
// Revision 13 2007/10/22 16:30:15 -0600 jkronba /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Merge Fix: LimGBICType
// 
// Revision 12 2007/10/22 12:08:52 -0600 jkronba /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Merge fixes
// 
// Revision 11 2007/10/19 11:56:34 -0600 jkronba /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Merge fixes
// 
// Revision 8 2007/02/21 13:56:47 -0700 bi4723 /AnalyzerCommon/5.40.400_Tip/5.40.100_5.40.300Merge
// Merge with changes made to original LimDriver.h again (this time from the 5.40.300 branch).
// 
// Revision 7 2007/02/01 18:30:23 -0700 bi4723 /AnalyzerCommon/5.40.100_Tip/MergeANT_SASE_RELEASE_5.40.200.010
// Merge with changes made to original LimDriver.h again.
// 
// Revision 5 2006/11/17 21:50:41 -0700 bi4723 /AnalyzerCommon/5.40.200_Tip/NA_Multiuser
// Moved definition of LLPRevType to this file since SART uses it.
// 
// Revision 4 2006/11/09 16:36:48 -0700 bi4723 /AnalyzerCommon/5.20_Release/NA_Multiuser
// Move IMA link status enum into LimSharedDefs.h since it is shared with NA MU and SART etc.
// 
// Revision 3 2006/11/02 09:55:26 -0700 bi4723 /AnalyzerCommon/5.20_Release/NA_Multiuser
// Put LIM family IDs and analog type enums in this file since they are shared. Also removed the LIM
// side only structure definitions for ATM vpvc stats and FR dlci stats since they are no longer shared.
// 
// Revision 2 2006/03/28 10:38:03 -0700 bi4723 /AnalyzerCommon/ScramJet/InitialStructure/NA_Multiuser
// Changed CTID struct defs to typedefs due to compiler errors on sart side with bm_buffermanagerdll
// not working with them.
// 
// Revision 1 2006/03/22 14:23:11 -0700 bi4723 /AnalyzerCommon/ScramJet/InitialStructure
// First pass breakout sharing of common defs between SART and NA.
// 
// Revision 1 2006/02/06 16:24:48 -0700 bi4723 /Platform/Nova/NA_Multiuser
// Moved shared definitions out of driver file so that SART MU server and NA MU
// client could use just the definitions. This shared definition files will be
// moved to another snapshot db that will be shared between SART and NA.
// 
//
///////////////////////////////////////////////////////////////////////////////

// abstract the NETWORK and EQUIPMENT sides of a LIM (each side has Tx and Rx ports)

enum
{
  NET_SIDE = 0,
  EQP_SIDE,

  N_SIDES
};

#define NUM_OF_PORTS            8
#define OCTOP_NUM_OF_PORTS      4
#define MPLIM_NUM_OF_PORTS      16


// LLP Board revisions 
enum LLPRevType
{
    LLP_INVALID = 0,
    LLP_REV_BETA,
    LLP_REV_A,
    LLP_REV_B,
    LLP_REV_C         // ScramJet LLP Board Revision
};

// LIM Family Types represent the analog board with out connector type 
enum LimHwFamilyType
{
    LIM_FAMILY_INVALID = 0,
    LIM_FAMILY_NONE,
    LIM_FAMILY_VSERIES,
    LIM_FAMILY_T1E1,
    LIM_FAMILY_J2,
    LIM_FAMILY_UTP25,
    LIM_FAMILY_T3E3,
    LIM_FAMILY_ETHER,
    LIM_FAMILY_FASTETHER,
    LIM_FAMILY_GIGAETHER,
    LIM_FAMILY_TOKENRING, // This is a place-holder for PC side support via NDIS & Offline
    LIM_FAMILY_FDDI,      // This is a place-holder for PC side support via NDIS & Offline
    LIM_FAMILY_WLAN,      // This is a place-holder for PC side support via, WLAN card, NDIS & Offline
    LIM_FAMILY_OC3,
    LIM_FAMILY_OC12,
    LIM_FAMILY_HSSI,
    LIM_FAMILY_8PORTT1E1,
    LIM_FAMILY_MPORTGIGABIT,
    LIM_FAMILY_TENGIGAETHER, // J6872A Ten Gigabit card
    LIM_FAMILY_PACKETPORTAL,  // J6890A Packet Portal system
    LIM_FAMILY_DDS            // This is a place-holder for PC side support for Offline (import from Internet Advisor)
};

// LIM Analog Types represent the analog board including connector type 
enum LimHwAnalogType 
{
    LIM_AN_TYPE_INVALID = 0,
    LIM_AN_TYPE_NONE,     // No LIM installed
    LIM_AN_TYPE_VSERIES,  // J6820A X.21, v.24(RS-232), v.35, v.36(RS-449)
    LIM_AN_TYPE_T1E1B,    // J6815A RJ48 with Bantam
    LIM_AN_TYPE_T1E1D,    // J6816A RJ48 with DB9
    LIM_AN_TYPE_E1BNC,    // J6817A BNCs
    LIM_AN_TYPE_J2,       // J6814A
    LIM_AN_TYPE_UTP25,    // J6818A UTP25
    LIM_AN_TYPE_E3,       // J6813A BNCs, aka DS3
    LIM_AN_TYPE_T3E3,     // J6813A BNCs, aka DS3
    LIM_AN_TYPE_T3,       // J6813A BNCs, aka DS3
    LIM_AN_TYPE_ETHTX,    // J6830A 10/100 base Tx, RJ48C + MII
    LIM_AN_TYPE_ETHFX,    // J6831A 10/100 base Fx, 1300nm MM
    LIM_AN_TYPE_OC3,      // J6811A optical
    LIM_AN_TYPE_EC3,      // J6812A STM-1e 
    LIM_AN_TYPE_UTP155,   // J6819A RJ48
    LIM_AN_TYPE_GIGABIT,  // J6832A 1000 base Fx with G-BICs
    LIM_AN_TYPE_OC12,     // J6810A dual rate
    LIM_AN_TYPE_UNKNOWN,
    LIM_AN_TYPE_HSSI,     // J6821A HSSI with SCSI2 cable
    LIM_AN_TYPE_8PORTT1E1,
    LIM_AN_TYPE_4PORTOC3,
    LIM_AN_TYPE_8PORTGIGABIT,  // J6850 8   port 10/100/1000
    LIM_AN_TYPE_16PORTGIGABIT,  // J6851 16  port 10/100/1000
    LIM_AN_TYPE_TENGIGABIT,     // J6872A Ten Gigabit card
    LIM_AN_TYPE_PACKETPORTAL    // J6890A Packet Portal system
};

// LIM Digital Types 
enum LimHwDigitalType 
{
    LIM_DIG_TYPE_INVALID = 0,
    LIM_DIG_TYPE_NONE,
    LIM_DIG_TYPE_A,
    LIM_DIG_TYPE_B,
    LIM_DIG_TYPE_C,
    LIM_DIG_TYPE_D
};


// generic 96-bit struct for statusBits et al
typedef struct
{
  UINT32 statusWord;     // statusBits, as an int, see BufferFormat.h for the bits
   INT16 sigAmplitude;   // mV peak or dBm, scaled 10x
  UINT16 lineStatusCntr; // bit sum of the statusWord (not used)
  UINT32 frequency;      // line speed measurement for Vseries, bits/sec

} LimLineStatus;


// AAL identification to be used with aal member of structure StatVPVC below.
enum
{
  LIM_STATVPVC_AAL_2,
  LIM_STATVPVC_AAL_2_CID, // this is not used now
  LIM_STATVPVC_AAL_5,
  LIM_STATVPVC_CELLMODE   // Cell mode no reassembly
};

// counters
typedef enum
{
  //  counters common to every limType 
  CNT_TIMESTAMP = 0,
  CNT_LINE_STATUS,  // sum of lineStat counts
  CNT_BYTE_COUNT,
  CNT_FRAME_COUNT,
  CNT_STORED_FRAMES, // post-filter frames to buffer
  CNT_FCS_ERRORS,

  // LAN counters
  CNT_RX_LOCAL_COLL,
  CNT_RX_REMOTE_COLL,
  CNT_RX_SHORT_FRAME, // runt
  CNT_RX_LONG_FRAME,  // jabber
  CNT_RX_ALIGN_ERROR,
  CNT_RX_DRIBBLE,
  CNT_TX_BYTE_COUNT,
  CNT_TX_FRAME_COUNT,
  CNT_TX_LOCAL_COLL,

  // WAN counters
  CNT_ABORT_FRAMES,
  CNT_SHORT_FRAMES,
  CNT_TOT_CELLS,
  CNT_IDLE_CELLS,
  CNT_UNASSIGNED,
  CNT_BUSY_CELLS,
  CNT_SHEC_ERRORS,
  CNT_MHEC_ERRORS,
  CNT_LIN_COD_VIOL, // BPV
  CNT_REMOTE_ALARM, // RDI, YELLOW
  CNT_CELL_SYNC,    // LCD
  CNT_FAS,
  CNT_LINE_FEBE,    // REI
  CNT_PATH_FEBE,
  CNT_LINE_AIS,
  CNT_PATH_AIS,
  CNT_P1P2_PARITY,
  CNT_CBIT_PARITY,
  CNT_PLCP_BIP,
  CNT_PLCP_FEBE,
  CNT_PLCP_YELLOW,
  CNT_PLCP_OOF,
  CNT_OOF,          // LOF
  CNT_MISMATCH,
  CNT_CRC,
  CNT_ESF_CRC,
  CNT_FRAME_BIT,
  CNT_FRAME_LOSS,
  CNT_ONES_DENSITY,
  CNT_EXCESS_ZEROS,
  CNT_BIP,
  CNT_BIP2,
  CNT_BIP3,
  CNT_CONGESTED,
  CNT_CLP,          // cell loss priority
  CNT_OAM,
  CNT_OAM_F3,
  CNT_ICP,
//  CNT_ICPCHG,
  CNT_FILLER,

  //IAM LIM spicific counters
  CNT_AAL5_FRAME,
  CNT_AAL5_CRC_ERR,
  CNT_AAL5_FRAME_BYTES,
  CNT_AAL2_SSCS_FRAME,
  CNT_AAL2_FRAME_BYTES,

  CNT_LCV_PORT1, 
  CNT_LOF_PORT1,   
  CNT_FAS_PORT1,    
  CNT_CRC_PORT1,      
  CNT_ONES_DENSITY_PORT1, 
  CNT_EXCESS_ZEROS_PORT1, 
  CNT_LCD_PORT1,    
  CNT_IDLE_CELLS_PORT1,
  CNT_UAS_CELLS_PORT1,
  CNT_BUSY_CELLS_PORT1,
  CNT_OAM_F1_CELLS_PORT1,
  CNT_OAM_F3_CELLS_PORT1,
  CNT_RM_CELLS_PORT1,
  CNT_SHEC_ERRORS_PORT1,  
  CNT_MHEC_ERRORS_PORT1,  
  CNT_CLP_CELLS_PORT1,
  CNT_CONG_CELLS_PORT1,
  CNT_ICP_CELLS_PORT1,   
  CNT_CHANGING_ICP_CELLS_PORT1,
  CNT_IMA_FILLER_CELLS_PORT1,
  CNT_FRAMES_PORT1,
  CNT_BYTES_PORT1,
  CNT_FCS_PORT1,
  CNT_ABORT_FRAMES_PORT1,
  CNT_SHORT_FRAMES_PORT1,

  CNT_LCV_PORT2, 
  CNT_LOF_PORT2,   
  CNT_FAS_PORT2,    
  CNT_CRC_PORT2,      
  CNT_ONES_DENSITY_PORT2, 
  CNT_EXCESS_ZEROS_PORT2, 
  CNT_LCD_PORT2,    
  CNT_IDLE_CELLS_PORT2,
  CNT_UAS_CELLS_PORT2,
  CNT_BUSY_CELLS_PORT2,
  CNT_OAM_F1_CELLS_PORT2,
  CNT_OAM_F3_CELLS_PORT2,
  CNT_RM_CELLS_PORT2,
  CNT_SHEC_ERRORS_PORT2,  
  CNT_MHEC_ERRORS_PORT2,  
  CNT_CLP_CELLS_PORT2,
  CNT_CONG_CELLS_PORT2,
  CNT_ICP_CELLS_PORT2,   
  CNT_CHANGING_ICP_CELLS_PORT2,
  CNT_IMA_FILLER_CELLS_PORT2,
  CNT_FRAMES_PORT2,
  CNT_BYTES_PORT2,
  CNT_FCS_PORT2,
  CNT_ABORT_FRAMES_PORT2,
  CNT_SHORT_FRAMES_PORT2,

  CNT_LCV_PORT3, 
  CNT_LOF_PORT3,   
  CNT_FAS_PORT3,    
  CNT_CRC_PORT3,      
  CNT_ONES_DENSITY_PORT3, 
  CNT_EXCESS_ZEROS_PORT3, 
  CNT_LCD_PORT3,    
  CNT_IDLE_CELLS_PORT3,
  CNT_UAS_CELLS_PORT3,
  CNT_BUSY_CELLS_PORT3,
  CNT_OAM_F1_CELLS_PORT3,
  CNT_OAM_F3_CELLS_PORT3,
  CNT_RM_CELLS_PORT3,
  CNT_SHEC_ERRORS_PORT3,  
  CNT_MHEC_ERRORS_PORT3,  
  CNT_CLP_CELLS_PORT3,
  CNT_CONG_CELLS_PORT3,
  CNT_ICP_CELLS_PORT3,   
  CNT_CHANGING_ICP_CELLS_PORT3,
  CNT_IMA_FILLER_CELLS_PORT3,
  CNT_FRAMES_PORT3,
  CNT_BYTES_PORT3,
  CNT_FCS_PORT3,
  CNT_ABORT_FRAMES_PORT3,
  CNT_SHORT_FRAMES_PORT3,

  CNT_LCV_PORT4, 
  CNT_LOF_PORT4,   
  CNT_FAS_PORT4,    
  CNT_CRC_PORT4,      
  CNT_ONES_DENSITY_PORT4, 
  CNT_EXCESS_ZEROS_PORT4, 
  CNT_LCD_PORT4,    
  CNT_IDLE_CELLS_PORT4,
  CNT_UAS_CELLS_PORT4,
  CNT_BUSY_CELLS_PORT4,
  CNT_OAM_F1_CELLS_PORT4,
  CNT_OAM_F3_CELLS_PORT4,
  CNT_RM_CELLS_PORT4,
  CNT_SHEC_ERRORS_PORT4,  
  CNT_MHEC_ERRORS_PORT4,  
  CNT_CLP_CELLS_PORT4,
  CNT_CONG_CELLS_PORT4,
  CNT_ICP_CELLS_PORT4,   
  CNT_CHANGING_ICP_CELLS_PORT4,
  CNT_IMA_FILLER_CELLS_PORT4,
  CNT_FRAMES_PORT4,
  CNT_BYTES_PORT4,
  CNT_FCS_PORT4,
  CNT_ABORT_FRAMES_PORT4,
  CNT_SHORT_FRAMES_PORT4,

  CNT_LCV_PORT5, 
  CNT_LOF_PORT5,   
  CNT_FAS_PORT5,    
  CNT_CRC_PORT5,      
  CNT_ONES_DENSITY_PORT5, 
  CNT_EXCESS_ZEROS_PORT5, 
  CNT_LCD_PORT5,    
  CNT_IDLE_CELLS_PORT5,
  CNT_UAS_CELLS_PORT5,
  CNT_BUSY_CELLS_PORT5,
  CNT_OAM_F1_CELLS_PORT5,
  CNT_OAM_F3_CELLS_PORT5,
  CNT_RM_CELLS_PORT5,
  CNT_SHEC_ERRORS_PORT5,  
  CNT_MHEC_ERRORS_PORT5,  
  CNT_CLP_CELLS_PORT5,
  CNT_CONG_CELLS_PORT5,
  CNT_ICP_CELLS_PORT5,   
  CNT_CHANGING_ICP_CELLS_PORT5,
  CNT_IMA_FILLER_CELLS_PORT5,
  CNT_FRAMES_PORT5,
  CNT_BYTES_PORT5,
  CNT_FCS_PORT5,
  CNT_ABORT_FRAMES_PORT5,
  CNT_SHORT_FRAMES_PORT5,

  CNT_LCV_PORT6, 
  CNT_LOF_PORT6,   
  CNT_FAS_PORT6,    
  CNT_CRC_PORT6,      
  CNT_ONES_DENSITY_PORT6, 
  CNT_EXCESS_ZEROS_PORT6, 
  CNT_LCD_PORT6,    
  CNT_IDLE_CELLS_PORT6,
  CNT_UAS_CELLS_PORT6,
  CNT_BUSY_CELLS_PORT6,
  CNT_OAM_F1_CELLS_PORT6,
  CNT_OAM_F3_CELLS_PORT6,
  CNT_RM_CELLS_PORT6,
  CNT_SHEC_ERRORS_PORT6,  
  CNT_MHEC_ERRORS_PORT6,  
  CNT_CLP_CELLS_PORT6,
  CNT_CONG_CELLS_PORT6,
  CNT_ICP_CELLS_PORT6,   
  CNT_CHANGING_ICP_CELLS_PORT6,
  CNT_IMA_FILLER_CELLS_PORT6,
  CNT_FRAMES_PORT6,
  CNT_BYTES_PORT6,
  CNT_FCS_PORT6,
  CNT_ABORT_FRAMES_PORT6,
  CNT_SHORT_FRAMES_PORT6,

  CNT_LCV_PORT7, 
  CNT_LOF_PORT7,   
  CNT_FAS_PORT7,    
  CNT_CRC_PORT7,      
  CNT_ONES_DENSITY_PORT7, 
  CNT_EXCESS_ZEROS_PORT7, 
  CNT_LCD_PORT7,    
  CNT_IDLE_CELLS_PORT7,
  CNT_UAS_CELLS_PORT7,
  CNT_BUSY_CELLS_PORT7,
  CNT_OAM_F1_CELLS_PORT7,
  CNT_OAM_F3_CELLS_PORT7,
  CNT_RM_CELLS_PORT7,
  CNT_SHEC_ERRORS_PORT7,  
  CNT_MHEC_ERRORS_PORT7,  
  CNT_CLP_CELLS_PORT7,
  CNT_CONG_CELLS_PORT7,
  CNT_ICP_CELLS_PORT7,   
  CNT_CHANGING_ICP_CELLS_PORT7,
  CNT_IMA_FILLER_CELLS_PORT7,
  CNT_FRAMES_PORT7,
  CNT_BYTES_PORT7,
  CNT_FCS_PORT7,
  CNT_ABORT_FRAMES_PORT7,
  CNT_SHORT_FRAMES_PORT7,

  CNT_LCV_PORT8, 
  CNT_LOF_PORT8,   
  CNT_FAS_PORT8,    
  CNT_CRC_PORT8,      
  CNT_ONES_DENSITY_PORT8, 
  CNT_EXCESS_ZEROS_PORT8, 
  CNT_LCD_PORT8,    
  CNT_IDLE_CELLS_PORT8,
  CNT_UAS_CELLS_PORT8,
  CNT_BUSY_CELLS_PORT8,
  CNT_OAM_F1_CELLS_PORT8,
  CNT_OAM_F3_CELLS_PORT8,
  CNT_RM_CELLS_PORT8,
  CNT_SHEC_ERRORS_PORT8,  
  CNT_MHEC_ERRORS_PORT8,  
  CNT_CLP_CELLS_PORT8,
  CNT_CONG_CELLS_PORT8,
  CNT_ICP_CELLS_PORT8,   
  CNT_CHANGING_ICP_CELLS_PORT8,
  CNT_IMA_FILLER_CELLS_PORT8,
  CNT_FRAMES_PORT8,
  CNT_BYTES_PORT8,
  CNT_FCS_PORT8,
  CNT_ABORT_FRAMES_PORT8,
  CNT_SHORT_FRAMES_PORT8,

  N_COUNTERS
};


// O12 Octopus - Counters below are remapped to existing per port counters. 
// OC12 Extra Demux Counters - Map them to LanCounters to save counter space.
// THERE CAN BE NO MORE EXTRA COUNTERS THAN THE LAN COUNTERS
enum
{
  CNT_OC12_T3_LOF = CNT_RX_LOCAL_COLL,
  CNT_OC12_T3_AIS,
  CNT_OC12_T3_RDI,
  CNT_OC12_T1_LOF,
  CNT_OC12_T1_AIS,
  CNT_OC12_T1_RDI
};


// O12 Octopus - Counters below are remapped to existing per port counters. 
// These remaped counters cannot go beyond CNT_EXCESS_ZEROS_PORTx
// because the remaining counter are or will be used.
enum
{
  CNT_RDI_L_PORT1      = CNT_LCV_PORT1,
  CNT_RDI_P_PORT1,
  CNT_REI_L_PORT1,
  CNT_AIS_L_PORT1,
  CNT_AIS_P_PORT1,
  CNT_B1_BIP_ERR_PORT1,
  
  CNT_RDI_L_PORT2      = CNT_LCV_PORT2,
  CNT_RDI_P_PORT2,
  CNT_REI_L_PORT2,
  CNT_AIS_L_PORT2,
  CNT_AIS_P_PORT2,
  CNT_B1_BIP_ERR_PORT2,
  
  CNT_RDI_L_PORT3      = CNT_LCV_PORT3,
  CNT_RDI_P_PORT3,
  CNT_REI_L_PORT3,
  CNT_AIS_L_PORT3,
  CNT_AIS_P_PORT3,
  CNT_B1_BIP_ERR_PORT3,

  CNT_RDI_L_PORT4      = CNT_LCV_PORT4,
  CNT_RDI_P_PORT4,
  CNT_REI_L_PORT4,
  CNT_AIS_L_PORT4,
  CNT_AIS_P_PORT4,
  CNT_B1_BIP_ERR_PORT4
};


// Multiport Gigabit - Counters below are remapped to existing port counters.
// These remaped counters cannot go beyond CNT_EXCESS_ZEROS_PORTx
// because the remaining counter are or will be used.
// That is there can be No more than 25 counters per port.
// In this case, only 11 counters are used.
// In addition, the original enumeration defines 8 counter sets to map to 
// eight ports that are used in a bidimensional array (two columns)
// Each column of the array used to refer to EQP side or LN side.
// In this case up to 16 ports are needed, so in order to save memory,
// the same array is used, but LN side will contain counters for ports 1-8
// while that EQP side will contain counters for ports 9-16.
// Therefore this enumeration remapping will declare counters 1-8 and 9-16 
// with the same mapping.
enum
{
  CNT_MPGIG_BYTES_PORT1      = CNT_LCV_PORT1,
  CNT_MPGIG_FRAMES_PORT1,
  CNT_MPGIG_BADFCS_PORT1,
  CNT_MPGIG_GOODRUNT_PORT1,
  CNT_MPGIG_BADRUNT_PORT1,
  CNT_MPGIG_GOODJABBER_PORT1,
  CNT_MPGIG_BADJABBER_PORT1,
  CNT_MPGIG_MULTICAST_PORT1,
  CNT_MPGIG_BROADCAST_PORT1,
  CNT_MPGIG_ANA_TOSS_PORT1,
  CNT_MPGIG_DIG_TOSS_PORT1,

  CNT_MPGIG_BYTES_PORT2      = CNT_LCV_PORT2,
  CNT_MPGIG_FRAMES_PORT2,
  CNT_MPGIG_BADFCS_PORT2,
  CNT_MPGIG_GOODRUNT_PORT2,
  CNT_MPGIG_BADRUNT_PORT2,
  CNT_MPGIG_GOODJABBER_PORT2,
  CNT_MPGIG_BADJABBER_PORT2,
  CNT_MPGIG_MULTICAST_PORT2,
  CNT_MPGIG_BROADCAST_PORT2,
  CNT_MPGIG_ANA_TOSS_PORT2,
  CNT_MPGIG_DIG_TOSS_PORT2,

  CNT_MPGIG_BYTES_PORT3      = CNT_LCV_PORT3,
  CNT_MPGIG_FRAMES_PORT3,
  CNT_MPGIG_BADFCS_PORT3,
  CNT_MPGIG_GOODRUNT_PORT3,
  CNT_MPGIG_BADRUNT_PORT3,
  CNT_MPGIG_GOODJABBER_PORT3,
  CNT_MPGIG_BADJABBER_PORT3,
  CNT_MPGIG_MULTICAST_PORT3,
  CNT_MPGIG_BROADCAST_PORT3,
  CNT_MPGIG_ANA_TOSS_PORT3,
  CNT_MPGIG_DIG_TOSS_PORT3,

  CNT_MPGIG_BYTES_PORT4      = CNT_LCV_PORT4,
  CNT_MPGIG_FRAMES_PORT4,
  CNT_MPGIG_BADFCS_PORT4,
  CNT_MPGIG_GOODRUNT_PORT4,
  CNT_MPGIG_BADRUNT_PORT4,
  CNT_MPGIG_GOODJABBER_PORT4,
  CNT_MPGIG_BADJABBER_PORT4,
  CNT_MPGIG_MULTICAST_PORT4,
  CNT_MPGIG_BROADCAST_PORT4,
  CNT_MPGIG_ANA_TOSS_PORT4,
  CNT_MPGIG_DIG_TOSS_PORT4,

  CNT_MPGIG_BYTES_PORT5      = CNT_LCV_PORT5,
  CNT_MPGIG_FRAMES_PORT5,
  CNT_MPGIG_BADFCS_PORT5,
  CNT_MPGIG_GOODRUNT_PORT5,
  CNT_MPGIG_BADRUNT_PORT5,
  CNT_MPGIG_GOODJABBER_PORT5,
  CNT_MPGIG_BADJABBER_PORT5,
  CNT_MPGIG_MULTICAST_PORT5,
  CNT_MPGIG_BROADCAST_PORT5,
  CNT_MPGIG_ANA_TOSS_PORT5,
  CNT_MPGIG_DIG_TOSS_PORT5,

  CNT_MPGIG_BYTES_PORT6      = CNT_LCV_PORT6,
  CNT_MPGIG_FRAMES_PORT6,
  CNT_MPGIG_BADFCS_PORT6,
  CNT_MPGIG_GOODRUNT_PORT6,
  CNT_MPGIG_BADRUNT_PORT6,
  CNT_MPGIG_GOODJABBER_PORT6,
  CNT_MPGIG_BADJABBER_PORT6,
  CNT_MPGIG_MULTICAST_PORT6,
  CNT_MPGIG_BROADCAST_PORT6,
  CNT_MPGIG_ANA_TOSS_PORT6,
  CNT_MPGIG_DIG_TOSS_PORT6,

  CNT_MPGIG_BYTES_PORT7      = CNT_LCV_PORT7,
  CNT_MPGIG_FRAMES_PORT7,
  CNT_MPGIG_BADFCS_PORT7,
  CNT_MPGIG_GOODRUNT_PORT7,
  CNT_MPGIG_BADRUNT_PORT7,
  CNT_MPGIG_GOODJABBER_PORT7,
  CNT_MPGIG_BADJABBER_PORT7,
  CNT_MPGIG_MULTICAST_PORT7,
  CNT_MPGIG_BROADCAST_PORT7,
  CNT_MPGIG_ANA_TOSS_PORT7,
  CNT_MPGIG_DIG_TOSS_PORT7,

  CNT_MPGIG_BYTES_PORT8      = CNT_LCV_PORT8,
  CNT_MPGIG_FRAMES_PORT8,
  CNT_MPGIG_BADFCS_PORT8,
  CNT_MPGIG_GOODRUNT_PORT8,
  CNT_MPGIG_BADRUNT_PORT8,
  CNT_MPGIG_GOODJABBER_PORT8,
  CNT_MPGIG_BADJABBER_PORT8,
  CNT_MPGIG_MULTICAST_PORT8,
  CNT_MPGIG_BROADCAST_PORT8,
  CNT_MPGIG_ANA_TOSS_PORT8,
  CNT_MPGIG_DIG_TOSS_PORT8,

  CNT_MPGIG_BYTES_PORT9      = CNT_LCV_PORT1,  // Eq side
  CNT_MPGIG_FRAMES_PORT9,
  CNT_MPGIG_BADFCS_PORT9,
  CNT_MPGIG_GOODRUNT_PORT9,
  CNT_MPGIG_BADRUNT_PORT9,
  CNT_MPGIG_GOODJABBER_PORT9,
  CNT_MPGIG_BADJABBER_PORT9,
  CNT_MPGIG_MULTICAST_PORT9,
  CNT_MPGIG_BROADCAST_PORT9,
  CNT_MPGIG_ANA_TOSS_PORT9,
  CNT_MPGIG_DIG_TOSS_PORT9,

  CNT_MPGIG_BYTES_PORT10      = CNT_LCV_PORT2, // Eq side
  CNT_MPGIG_FRAMES_PORT10,
  CNT_MPGIG_BADFCS_PORT10,
  CNT_MPGIG_GOODRUNT_PORT10,
  CNT_MPGIG_BADRUNT_PORT10,
  CNT_MPGIG_GOODJABBER_PORT10,
  CNT_MPGIG_BADJABBER_PORT10,
  CNT_MPGIG_MULTICAST_PORT10,
  CNT_MPGIG_BROADCAST_PORT10,
  CNT_MPGIG_ANA_TOSS_PORT10,
  CNT_MPGIG_DIG_TOSS_PORT10,

  CNT_MPGIG_BYTES_PORT11      = CNT_LCV_PORT3, // Eq side
  CNT_MPGIG_FRAMES_PORT11,
  CNT_MPGIG_BADFCS_PORT11,
  CNT_MPGIG_GOODRUNT_PORT11,
  CNT_MPGIG_BADRUNT_PORT11,
  CNT_MPGIG_GOODJABBER_PORT11,
  CNT_MPGIG_BADJABBER_PORT11,
  CNT_MPGIG_MULTICAST_PORT11,
  CNT_MPGIG_BROADCAST_PORT11,
  CNT_MPGIG_ANA_TOSS_PORT11,
  CNT_MPGIG_DIG_TOSS_PORT11,

  CNT_MPGIG_BYTES_PORT12      = CNT_LCV_PORT4, // Eq side
  CNT_MPGIG_FRAMES_PORT12,
  CNT_MPGIG_BADFCS_PORT12,
  CNT_MPGIG_GOODRUNT_PORT12,
  CNT_MPGIG_BADRUNT_PORT12,
  CNT_MPGIG_GOODJABBER_PORT12,
  CNT_MPGIG_BADJABBER_PORT12,
  CNT_MPGIG_MULTICAST_PORT12,
  CNT_MPGIG_BROADCAST_PORT12,
  CNT_MPGIG_ANA_TOSS_PORT12,
  CNT_MPGIG_DIG_TOSS_PORT12,

  CNT_MPGIG_BYTES_PORT13      = CNT_LCV_PORT5, // Eq side
  CNT_MPGIG_FRAMES_PORT13,
  CNT_MPGIG_BADFCS_PORT13,
  CNT_MPGIG_GOODRUNT_PORT13,
  CNT_MPGIG_BADRUNT_PORT13,
  CNT_MPGIG_GOODJABBER_PORT13,
  CNT_MPGIG_BADJABBER_PORT13,
  CNT_MPGIG_MULTICAST_PORT13,
  CNT_MPGIG_BROADCAST_PORT13,
  CNT_MPGIG_ANA_TOSS_PORT13,
  CNT_MPGIG_DIG_TOSS_PORT13,

  CNT_MPGIG_BYTES_PORT14      = CNT_LCV_PORT6, // Eq side
  CNT_MPGIG_FRAMES_PORT14,
  CNT_MPGIG_BADFCS_PORT14,
  CNT_MPGIG_GOODRUNT_PORT14,
  CNT_MPGIG_BADRUNT_PORT14,
  CNT_MPGIG_GOODJABBER_PORT14,
  CNT_MPGIG_BADJABBER_PORT14,
  CNT_MPGIG_MULTICAST_PORT14,
  CNT_MPGIG_BROADCAST_PORT14,
  CNT_MPGIG_ANA_TOSS_PORT14,
  CNT_MPGIG_DIG_TOSS_PORT14,

  CNT_MPGIG_BYTES_PORT15      = CNT_LCV_PORT7, // Eq side
  CNT_MPGIG_FRAMES_PORT15,
  CNT_MPGIG_BADFCS_PORT15,
  CNT_MPGIG_GOODRUNT_PORT15,
  CNT_MPGIG_BADRUNT_PORT15,
  CNT_MPGIG_GOODJABBER_PORT15,
  CNT_MPGIG_BADJABBER_PORT15,
  CNT_MPGIG_MULTICAST_PORT15,
  CNT_MPGIG_BROADCAST_PORT15,
  CNT_MPGIG_ANA_TOSS_PORT15,
  CNT_MPGIG_DIG_TOSS_PORT15,

  CNT_MPGIG_BYTES_PORT16      = CNT_LCV_PORT8, // Eq side
  CNT_MPGIG_FRAMES_PORT16,
  CNT_MPGIG_BADFCS_PORT16,
  CNT_MPGIG_GOODRUNT_PORT16,
  CNT_MPGIG_BADRUNT_PORT16,
  CNT_MPGIG_GOODJABBER_PORT16,
  CNT_MPGIG_BADJABBER_PORT16,
  CNT_MPGIG_MULTICAST_PORT16,
  CNT_MPGIG_BROADCAST_PORT16,
  CNT_MPGIG_ANA_TOSS_PORT16,
  CNT_MPGIG_DIG_TOSS_PORT16

};

// 10 Gigabit - Counters below are remapped to existing port counters.
// These remaped counters cannot go beyond CNT_EXCESS_ZEROS_PORTx
// because the remaining counter are or will be used.
// That is there can be No more than 25 counters per port.
// In this case, only 12 counters are used.
enum
{
  CNT_10GIG_BYTES_PORT1      = CNT_LCV_PORT1,
  CNT_10GIG_FRAMES_PORT1,
  CNT_10GIG_BADFCS_PORT1,
  CNT_10GIG_GOODRUNT_PORT1,
  CNT_10GIG_BADRUNT_PORT1,
  CNT_10GIG_GOODJABBER_PORT1,
  CNT_10GIG_BADJABBER_PORT1,
  CNT_10GIG_MULTICAST_PORT1,
  CNT_10GIG_BROADCAST_PORT1,
  CNT_10GIG_TOSS_PORT1,            // All tossed/missed frames (at all stages of hw).
  CNT_10GIG_FILTERED_BYTES_PORT1, // Filtered frames, used to handle software stats projection.
  CNT_10GIG_FILTERED_FRAMES_PORT1,  // Filtered bytes, used to handle software stats projection.

  CNT_10GIG_BYTES_PORT2      = CNT_LCV_PORT2,
  CNT_10GIG_FRAMES_PORT2,
  CNT_10GIG_BADFCS_PORT2,
  CNT_10GIG_GOODRUNT_PORT2,
  CNT_10GIG_BADRUNT_PORT2,
  CNT_10GIG_GOODJABBER_PORT2,
  CNT_10GIG_BADJABBER_PORT2,
  CNT_10GIG_MULTICAST_PORT2,
  CNT_10GIG_BROADCAST_PORT2,
  CNT_10GIG_TOSS_PORT2,
  CNT_10GIG_FILTERED_BYTES_PORT2,
  CNT_10GIG_FILTERED_FRAMES_PORT2,

  CNT_10GIG_BYTES_PORT3      = CNT_LCV_PORT3,
  CNT_10GIG_FRAMES_PORT3,
  CNT_10GIG_BADFCS_PORT3,
  CNT_10GIG_GOODRUNT_PORT3,
  CNT_10GIG_BADRUNT_PORT3,
  CNT_10GIG_GOODJABBER_PORT3,
  CNT_10GIG_BADJABBER_PORT3,
  CNT_10GIG_MULTICAST_PORT3,
  CNT_10GIG_BROADCAST_PORT3,
  CNT_10GIG_TOSS_PORT3,
  CNT_10GIG_FILTERED_BYTES_PORT3,
  CNT_10GIG_FILTERED_FRAMES_PORT3,

  CNT_10GIG_BYTES_PORT4      = CNT_LCV_PORT4,
  CNT_10GIG_FRAMES_PORT4,
  CNT_10GIG_BADFCS_PORT4,
  CNT_10GIG_GOODRUNT_PORT4,
  CNT_10GIG_BADRUNT_PORT4,
  CNT_10GIG_GOODJABBER_PORT4,
  CNT_10GIG_BADJABBER_PORT4,
  CNT_10GIG_MULTICAST_PORT4,
  CNT_10GIG_BROADCAST_PORT4,
  CNT_10GIG_TOSS_PORT4,
  CNT_10GIG_FILTERED_BYTES_PORT4,
  CNT_10GIG_FILTERED_FRAMES_PORT4,

  CNT_10GIG_BYTES_PORT5      = CNT_LCV_PORT5,
  CNT_10GIG_FRAMES_PORT5,
  CNT_10GIG_BADFCS_PORT5,
  CNT_10GIG_GOODRUNT_PORT5,
  CNT_10GIG_BADRUNT_PORT5,
  CNT_10GIG_GOODJABBER_PORT5,
  CNT_10GIG_BADJABBER_PORT5,
  CNT_10GIG_MULTICAST_PORT5,
  CNT_10GIG_BROADCAST_PORT5,
  CNT_10GIG_TOSS_PORT5,
  CNT_10GIG_FILTERED_BYTES_PORT5,
  CNT_10GIG_FILTERED_FRAMES_PORT5
};


// LIM Layer 1 config constants

// limTypes supported (this enum is doomed)
enum
{
  LIM_NONE    =  0,
  LIM_UNKNOWN =  1,
  LIM_OC12    =  2, // J6810A dual rate
  LIM_OC3     =  3, // J6811A optical (not planned as A)
  LIM_EC3     =  4, // J6812A STM-1e  (not planned as A)
  LIM_T3E3    =  5, // J6813A BNCs, aka DS3
  LIM_J2      =  6, // J6814A
  LIM_T1E1B   =  7, // J6815A RJ48 with Bantam
  LIM_T1E1D   =  8, // J6816A RJ48 with DB9
  LIM_E1BNC   =  9, // J6817A BNCs
  LIM_ATM25   = 10, // J6818A UTP25 (not planned as A)
  LIM_UTP155  = 11, // J6819A RJ48  (not planned)
  LIM_VSERIES = 12, // J6820A X.21, v.24(RS-232), v.35, v.36(RS-449)
  LIM_ETHTX   = 13, // J6830A 10/100 base Tx, RJ48C + MII
  LIM_ETHFX   = 14, // J6831A 10/100 base Fx, 1300nm MM
  LIM_GIGABIT = 15, // J6832A 1000 base Fx with G-BICs

  LIM_OC12B   = 18, // J6810B dual rate
  LIM_OC3B    = 19, // J6811B optical
  LIM_EC3B    = 20, // J6812B STM-1e 
  LIM_T3E3B   = 21, // J6813B BNCs, aka DS3
  LIM_J2B     = 22, // J6814B
  LIM_T1E1BB  = 23, // J6815B RJ48 with Bantam
  LIM_T1E1DB  = 24, // J6816B RJ48 with DB9
  LIM_E1BNCB  = 25, // J6817B BNCs
  LIM_ATM25B  = 26, // J6818B UTP25 (Pony)
  LIM_UTP155B = 27, // J6819B RJ48 (will obsolete)
  LIM_VSERIEB = 28, // J6820B Vseries (not planned as B)
  LIM_HSSI    = 29, // J6821A HSSI
  LIM_8PORTT1E1= 30,// J6824 8-port T1/E1
  LIM_4PORTOC3 = 31,// J6828A / J6810A Forced Multiport mode
  LIM_UNSUPPORTED = 32,  // an unsupported LIM
  LIM_8PORTGIGABIT = 33,  // J6850  8-port Gigabit
  LIM_16PORTGIGABIT = 34,  // J6851 16-port Gigabit
  LIM_TENGIGABIT = 35,     // J6872A Ten Gigabit card
  LIM_PACKETPORTAL = 36    // Jxxxx PacketPortal system
};

// LAN interfaceTypes
enum
{
  LIM_LAN_LINE_MODE_NODE,
  LIM_LAN_LINE_MODE_MON,
  LIM_LAN_LINE_MODE_HUB,
  LIM_LAN_LINE_MODE_DUAL,
  LIM_LAN_LINE_MODE_MII,
  LIM_LAN_LINE_MODE_LBSM, // Loopback from same side ("to hub" side)
  LIM_LAN_LINE_MODE_LBOT  // Loopback from other side
};

// WAN interfaceTypes
enum LimWanInterfaceType
{
  LIM_LINE_T1 = 1, // DS1
  LIM_LINE_T1DSX,
  LIM_LINE_E1,
  LIM_LINE_T3,     // DS3
  LIM_LINE_E3,
  LIM_LINE_V11,    // not needed?
  LIM_LINE_X21,
  LIM_LINE_RS_232, // V.24
  LIM_LINE_V35,
  LIM_LINE_RS_449, // V.36
  LIM_LINE_EIA_530,
};

enum SignalLevelType {
     LIM_SL_DSX  = 1,
     LIM_SL_NETWORK 
};

// Ethernet BufFmtEthLinkStatusBits constants (originally located in NA code base, LimEthernet.h)

#define ETHER_TX       0
#define ETHER_FX       1
#define ETHER_NO_CARD  2
// modes
#define ETHER_MONITOR  0
#define ETHER_NODE     1
#define ETHER_HUB      2
#define ETHER_DUAL     3
#define ETHER_MII      4
#define ETHER_NO_MII   5
#define ETHER_LP_SAME  6
#define ETHER_LP_OTHER 7
// duplex
#define ETHER_HDX      0
#define ETHER_FDX      1
// speed
#define ETHER_10MBPS   0
#define ETHER_100MBPS  1
#define ETHER_1000MBPS 2
// T4
#define ETHER_NO_T4    0
#define ETHER_T4       1
// autonegotiation
#define ETHER_NO_AUTO           0
#define ETHER_AUTO              1
#define ETHER_AUTO_NOT_COMPLETE 0
#define ETHER_AUTO_COMPLETE     1
// link status
#define ETHER_LINK_GOOD 0
#define ETHER_LINK_BAD  1
// clock status
#define ETHER_CLOCK_OK   0
#define ETHER_LOST_CLOCK 1
// hardware status
#define ETHER_HW_OK    0
#define ETHER_HW_ERROR 1

// end of Ethernet BufFmtEthLinkStatusBits constants

// data rate. If you modify these remember to also modify the following
// support functions appropriately.
#define LIM_LAN_LINE_SPEED_UNKNOWN    0
#define LIM_LAN_LINE_SPEED_10Mb      10
#define LIM_LAN_LINE_SPEED_100Mb    100
#define LIM_LAN_LINE_SPEED_1Gb     1000
#define LIM_LAN_LINE_SPEED_10Gb   10000
#define LIM_LAN_LINE_SPEED_ERROR  0xffff

#ifdef __cplusplus
inline bool UnknownLIMLANSpeed(UINT32 LANSpeed)
{
   return ((!((LANSpeed == LIM_LAN_LINE_SPEED_10Mb) ||
              (LANSpeed == LIM_LAN_LINE_SPEED_100Mb) ||
              (LANSpeed == LIM_LAN_LINE_SPEED_1Gb) ||
              (LANSpeed == LIM_LAN_LINE_SPEED_10Gb))));
}

inline bool KnownLIMLANSpeed(UINT32 LANSpeed)
{
   return !UnknownLIMLANSpeed(LANSpeed);
}
#endif

// media Connections
#define LIM_LAN_MEDIA_INT_NORM       0
#define LIM_LAN_MEDIA_INT_AUTO       1

// Alternate constants for Media Connection
enum LIM_LAN_MEDIA_CONNECTION
{
   LIM_LAN_MEDIA_CONN_AUTO,
   LIM_LAN_MEDIA_CONN_FORCED,
   LIM_LAN_MEDIA_CONN_10BASET,
   LIM_LAN_MEDIA_CONN_FASTETHER,    // lineSpeed = 10 or 100 Mbps
   LIM_LAN_MEDIA_CONN_1000BASET
};

// Fast Ethernet & Gigabit hardware config info (bit mapped)
#define LIM_LAN_HW_TX           0x0001
#define LIM_LAN_HW_MII          0x0002
#define LIM_LAN_HW_FX           0x0004
#define LIM_LAN_HW_GBIC_MODE_A  0x0038
#define LIM_LAN_HW_GBIC_MODE_B  0x01C0


// GBIC HW types. If you modify this enum remember to also modify the following
// support functions appropriately.
enum LIM_LAN_GBICTypes
{
  LIM_LAN_GBIC_NOT_PRESENT = 1,
  LIM_LAN_GBIC_UNKNOWN,
  LIM_LAN_GBIC_SX,
  LIM_LAN_GBIC_LX,    
  LIM_LAN_GBIC_CX,
  LIM_LAN_GBIC_M,
  LIM_LAN_GBIC_T,
  LIM_LAN_SFPP_DA,
  LIM_LAN_SFPP_SR,
  LIM_LAN_SFPP_LR,
  LIM_LAN_SFPP_LRM,
  LIM_LAN_SFPP_0C3,
  LIM_LAN_SFPP_0C12,
  LIM_LAN_SFPP_0C48,
  LIM_LAN_SFPP_0C192,
  LIM_LAN_GBIC_UNSUPPORTED,
  LIM_LAN_GBIC_ERROR = 0xff,
};

#ifdef __cplusplus
inline bool InvalidLIMLANGBIC(UINT32 GBIC)
{
   return ((GBIC < LIM_LAN_GBIC_NOT_PRESENT) ||
           (GBIC == LIM_LAN_GBIC_NOT_PRESENT) ||
           (GBIC == LIM_LAN_GBIC_UNKNOWN) ||
           (GBIC == LIM_LAN_GBIC_UNSUPPORTED) ||
           ((GBIC > LIM_LAN_GBIC_UNSUPPORTED) && (GBIC < LIM_LAN_GBIC_ERROR)) ||
           (GBIC == LIM_LAN_GBIC_ERROR));
}

inline bool ValidLIMLANGBIC(UINT32 GBIC)
{
   return !InvalidLIMLANGBIC(GBIC);
}

inline bool OneGigELIMLANGBIC(UINT32 GBIC)
{
   return ((GBIC == LIM_LAN_GBIC_T) ||
           (GBIC == LIM_LAN_GBIC_SX) ||
           (GBIC == LIM_LAN_GBIC_LX));
}

inline bool OneGigECopperLIMLANGBIC(UINT32 GBIC)
{
   return (GBIC == LIM_LAN_GBIC_T);
}

inline bool OneGigEOpticalLIMLANGBIC(UINT32 GBIC)
{
   return ((GBIC == LIM_LAN_GBIC_SX) ||
           (GBIC == LIM_LAN_GBIC_LX));
}

inline bool TenGigELIMLANGBIC(UINT32 GBIC)
{
   return ((GBIC == LIM_LAN_SFPP_DA) ||
           (GBIC == LIM_LAN_SFPP_SR) ||
           (GBIC == LIM_LAN_SFPP_LR) ||
           (GBIC == LIM_LAN_SFPP_LRM));
}

inline bool OpticalSonetLIMLANGBIC(UINT32 GBIC)
{
   return ((GBIC == LIM_LAN_SFPP_0C3) ||
           (GBIC == LIM_LAN_SFPP_0C12) ||
           (GBIC == LIM_LAN_SFPP_0C48) ||
           (GBIC == LIM_LAN_SFPP_0C192));
}
#endif
   
// capabilities in the MII detection register
#define LIM_LAN_MII_100BASE_T4_Capable            0x8000
#define LIM_LAN_MII_100BASE_T_FullDuplexCapable   0x4000
#define LIM_LAN_MII_100BASE_T_HalfDuplexCapable   0x2000
#define LIM_LAN_MII_10BASE_T_FullDuplexCapable    0x1000
#define LIM_LAN_MII_10BASE_T_HalfDuplexCapable    0x0800
#define LIM_LAN_MII_AutoNegotiateCapable          0x0008


// WAN dataChanRate values
enum LimChannelRateType
{
  LIM_CHANNEL_FULL_24x64 = 1, // T1
  LIM_CHANNEL_FULL_31x64,     // E1 
  LIM_CHANNEL_FRAC_Nx64,      // T1, E1, T3, DDS 
  LIM_CHANNEL_FULL_24x56,     // T1 
  LIM_CHANNEL_FRAC_Nx56,      // T1, T3
  LIM_CHANNEL_D_4x16k,        // T1, E1
  LIM_CHANNEL_FULL_T3,        // T3
  LIM_CHANNEL_FRAC_T3,        // T3
  LIM_CHANNEL_FULL_E3         // E3 
};

// Receiver Mode types
enum LimReceiverModeType
{
  LIM_RM_BRIDGED = 1,  // T1, E1, T3, E3, ST, U, DDS 
  LIM_RM_MONJACK_T1,   // T1 
  LIM_RM_MONJACK_20DB, // E1 
  LIM_RM_MONJACK_30DB, // E1
  LIM_RM_TERMINATED,   // T1, E1, T3, E3, ST, U, DDS 
  LIM_RM_THRU_DI,      // T1, E1 "thru drop&insert"
  LIM_RM_SYNC,         // V Series (with all Layer 2 types)
  LIM_RM_ASYNC,        // V Series (only with PPP)
  LIM_RM_SIMULATE,
  LIM_RM_4PORT_THRU_BRIDGED,
  LIM_RM_1PORT_THRU_BRIDGED,
  LIM_RM_MONJACK_26DB, // E1
  LIM_RM_MONJACK_32DB, // E1
  LIM_RM_UNKNOWN
};

// Line Code types
enum LimLineCodeType
{
  LIM_LC_AMI = 1, // T1, E1, DDS 
  LIM_LC_B8ZS,    // T1 
  LIM_LC_HDB3,    // E1, E3 
  LIM_LC_B3ZS     // T3 
};

// Line Channel Types for the xx_Chan_Type entries 
enum LimChannelType 
{
  LIM_CHANNEL_TYPE_OFF = 1, // T1, E1, T3, E3, ST, U 
  LIM_CHANNEL_TYPE_DATA,    // T1, E1, T3, E3, ST, U, DDS 
  LIM_CHANNEL_TYPE_BERT,    // T1, E1, T3, E3, ST, U 
  LIM_CHANNEL_TYPE_MLPPP,   // T1, E1, ST, U 
  LIM_CHANNEL_TYPE_DFOLLOW  // T1, E1, ST, U 
};

// Transmit line Build Outs
enum LimTransmitBuildOutType
{
  LIM_LBO_0_133 = 1, // T1, E1 
  LIM_LBO_133_266,   // 
  LIM_LBO_266_399,   // 
  LIM_LBO_399_533,   // 
  LIM_LBO_533_655,   // 
  LIM_LBO_0DB,       // 
  LIM_LBO_7_5DB,     // 
  LIM_LBO_15_0DB,    // 
  LIM_LBO_22_5DB,    //
  LIM_LBO_HIGH,      // T3, E3 
  LIM_LBO_DSX,       // T3, E3 
  LIM_LBO_LOW,       // T3 
  LIM_LBO_900FT      // T3, E3 
};


//  Voice PCM Code types
enum LimVoicePCMCodeType
{
  LIM_PCMCODE_ULAW = 1, // T1, E1, S/T, U 
  LIM_PCMCODE_ALAW      // T1, E1, S/T, U 
};

// Transmit Clock Sources 
enum LimTransmitClockSourceType 
{
  LIM_TC_INTERNAL_CLOCK = 1, // T1, E1, T3, E3, ST, U, DDS 
  LIM_TC_RECOVERED_LINE,     // T1, E1, T3, E3, ST, U, DDS 
  LIM_TC_RECOVERED_EQPT,     // T1, E1, T3, E3, ST, U 
  LIM_TC_EXTERNAL_CLOCK,     // T1, E1 
  LIM_TC_DCE,                // V Series, Hssi
  LIM_TC_DTE                 // V Series, Hssi
};

// T1/E1 Framing parameters
// Notice that this enumeration is used by UINT32 variables and also by
//   bit field variables which require 5 bits (0-31 range) for SOME LIMs only.
// WEI NOTE: The types here must be kept in sync with the framing types on the
//        PC side in WanInterface.h unless specifically noted how the mappings
//        occur so that the 2 sides do NOT get out of sync again.  Be very
//        aware of this when adding new types -- and do so at the end of the list
//        -- and remember that these values are stored in files so serialization 
//        issues MAY arrise. Not following these simple guidelines has broken 
//        the system multiple times now.
enum LimFramingType
{
  LIM_FT_NONE = 0,           // No Framing used or necessary
  LIM_FT_CLEAR_CHAN_64K = 1, // DDS
  LIM_FT_ESF_W_CRC,          // T1, T3, Octopus Trib Mode
  LIM_FT_DDS_SEC_CHANNEL,    // DDS
  LIM_FT_DDS_NO_SEC_CHANNEL, // DDS
  LIM_FT_D4_FS,              // T1, Octopus Trib Mode
  LIM_FT_UNFRAMED,           // T1, E1, Octopus Trib Mode
  LIM_FT_T1DM,               // T1
  LIM_FT_E1_W_CRC4,          // E1 (G704), Octopus Trib Mode
  LIM_FT_E1_NO_CRC4,         // E1 (G704), Octopus Trib Mode
  LIM_FT_SLIC_96,            // T1
  LIM_FT_CBIT,               // T3
  LIM_FT_M13,                // T3
  LIM_FT_SF,                 // T3
  LIM_FT_G832,               // E3
  LIM_FT_G751,               // E3
  LIM_FT_E1_CAS_W_CRC4,      // E1 CAS Only
  LIM_FT_E1_CAS_NO_CRC4,     // E1 CAS Only
  LIM_FT_STM1,               // OC3
  LIM_FT_STS3C,              // OC3
  LIM_FT_4B5B,               // ATM25
  LIM_FT_AUTO,

  //The following OC12 framing types are NOT used on the LIM side, but are used
  //  on the PC side to map to the more complex OC12 framing setup.
  LIM_FT_AU4_4C,             // OC12
  LIM_FT_STS12C,             // OC12
  LIM_FT_STS1,               // OC12
  LIM_FT_AU3,                // OC12
  LIM_FT_AU4,                // OC12

  LIM_FT_J1_D4,              // 8 Port T1/E1
  LIM_FT_J1_ESF              // 8 Port T1/E1

  // WAIT WAIT WAIT !!! BEFORE ADDING ANYTHING HERE, PLEASE READ COMMENT ABOVE!
};

// Data Sense Parameters
enum LimDataSenseType 
{
  LIM_DS_NORMAL = 1, // T1, E1, T3, E3, ST, U, DDS, V-Series, HSSI 
  LIM_DS_INVERTED,   // T1, E1, T3, E3, ST, U, DDS, V-Series, HSSI 
  LIM_DS_NRZI,       // T1, E1, ST, U, DDS, V-Series 
  LIM_DS_RCVD_NRZI   // V-Series 
};

// special T3 simModes
enum LimT3SimModeTypes
{
  LIM_DS3_IDLES = 1,
  LIM_DS3_SIMTYPE_REPEAT,
  LIM_DS3_SIMTYPE_DROP_INS
};

// simulation sides
enum LimSimType
{
  LIM_NO_SIM = 1,
  LIM_NET_SIM,
  LIM_EQP_SIM
};

enum LimCRCType
{
  LIM_CRC_16 = 0,
  LIM_CRC_32 
};

// BERT parms
enum LimBertType
{
  LIM_NO_BERT = 1,
  LIM_NET_BERT,
  LIM_EQP_BERT
};

//Test mode for Hssi

enum LimHssiTestMode
{
  HSSI_TM_OFF = 0,
  HSSI_TM_ON,
  HSSI_TM_auto

};

// Connector types for T1/E1
enum LimT1E1ConnectorType 
{
  LIM_CT_RJ45 = 1,
  LIM_CT_RJ48C,
  LIM_E1_CONNECTOR_75OHM,
  LIM_E1_CONNECTOR_120OHM,
  LIM_LAST,
};

// Cable types for Vseries
enum LimVseriesCableType
{
  LIM_VSER_CABLE_V35    = 0x1C,
  LIM_VSER_CABLE_EIA530 = 0x19,
  LIM_VSER_CABLE_RS232  = 0x1E,
  LIM_VSER_CABLE_X21    = 0x1B,
  LIM_VSER_CABLE_RS449  = 0x1D,
  LIM_VSER_CABLE_NONE   = 0x1F
};


enum LimOC12PortMode
{
  LIM_OC12_1PORT_MODE,
  LIM_OC12_4PORT_MODE,
  LIM_OC12_FORCED_4PORT_MODE
};

// OC12 operating modes
enum LimOC12mode
{
  LIM_OC12_MONITOR_ONLY = 1,  // laser off, no passthru
  LIM_OC12_MONITOR_PASSTHRU,  // with passthru
  LIM_OC12_SIM_NET,
  LIM_OC12_SIM_EQP,
  LIM_OC12_INDEP_ONLY,        // laser off, no passthru
  LIM_OC12_INDEP_CHANNELS     // with passthru
};

enum LimOC12fibre
{
   LIM_OC12_FIB_AUTO = 1,
   LIM_OC12_FIB_OC3,   /* OC3  or STM-1 */
   LIM_OC12_FIB_OC12   /* OC12 or STM-4 */
};

// constants for the OC12 routing type
enum LimOC12MappingType
{
   LIM_OC12_STS12C = 1,
   LIM_OC12_VC_4_4C,
   LIM_OC12_STS1,
   LIM_OC12_VC3,
   LIM_OC12_STS3C,
   LIM_OC12_VC4 
};

enum LimOC12PDHtype
{
   // PDH Framing types when STS
   LIM_OC12_STS_BULK = 0,
   LIM_OC12_STS_VT15_DS1,
   LIM_OC12_STS_DS3_DS1,
   LIM_OC12_STS_DS3_UNCHAN,
   LIM_OC12_STS_VT2_E1,
   LIM_OC12_STS_E3_E1,
   LIM_OC12_STS_E3_UNCHAN,
   // PDH Framing types when SDH (VC)
   LIM_OC12_SDH_C11_DS1,
   LIM_OC12_SDH_C3_DS3_DS1,
   LIM_OC12_SDH_C3_DS3_UNCHAN,
   LIM_OC12_SDH_C4_BULK,
   LIM_OC12_SDH_C3_BULK,
   LIM_OC12_SDH_C12_E1,
   LIM_OC12_SDH_C3_E3_E1,
   LIM_OC12_SDH_C3_E3_UNCHAN
};

enum LimOC12SyncMode
{
   LIM_OC12_ASYNC = 0,
   LIM_OC12_BYTESYNC
};

enum LimOC12E3Framing
{
   LIM_OC12_E3_12 = 0,
   LIM_OC12_E3_16
};

enum LimOC12DS3Framing
{
   LIM_OC12_DS3_M13 = 0,
   LIM_OC12_DS3_CBIT
};

enum LimOC12DS1Framing
{
   LIM_OC12_DS1_SF = 0,
   LIM_OC12_DS1_ESF
};

enum LimOC12DS0Speed
{
   LIM_OC12_DS0_56K = 0,
   LIM_OC12_DS0_64K
};

enum LimOC12TADMode
{
   LIM_OC12_NORMAL = 0,
   LIM_OC12_TAD
};

// See Struct LimOctopCfg.mode: 1 bit
enum LimOctopMode
{
  LIM_OCTOP_MODE_SDH   = 0,
  LIM_OCTOP_MODE_SONET
};

// See Struct LimOctopCfg.path: 2 bits
enum LimOctopPath
{
  LIM_OCTOP_PATH_BULK   = 0,
  LIM_OCTOP_PATH_T1,
  LIM_OCTOP_PATH_E1,
  LIM_OCTOP_PATH_UNUSED
};



// CTID SDH when path includes AU4
typedef struct
{
  UINT32   TUID        :2;
  UINT32   TUG2ID      :3;
  UINT32   VC4ID       :2;
  UINT32   TUG3ID      :2;
  UINT32   unused      :23;
} CTID_SDH_AU4 ;

// CTID SDH when path includes AU3
typedef struct
{
  UINT32   TUID        :2;
  UINT32   TUG2ID      :3;
  UINT32   VCID        :4;
  UINT32   unused      :23;
} CTID_SDH_AU3 ;

// CTID Sonet
typedef struct
{
  UINT32   TUID        :2;
  UINT32   VTGID       :3;
  UINT32   STS1ID      :4;
  UINT32   unused      :23;
} CTID_SONET ;

// CTID Bits - used to extract raw 9 bits CTID
typedef struct
{
  UINT32   CTID        :9;
  UINT32   unused      :23;
} CTID_BITS ;


// Demux Tributary ID Config structure
typedef union
{
  UINT32         word;
  CTID_BITS      bits;
  CTID_SDH_AU4   sdh_au4;
  CTID_SDH_AU3   sdh_au3;
  CTID_SONET     sonet;

} LimOctoTribID;

// ATM IMA link status 2 bit field values.(used for IMA hardware status).
enum LimImaLinkStatus
{
   IMA_LINK_STATUS_NOT_IN_GROUP,
   IMA_LINK_STATUS_UNUSABLE,
   IMA_LINK_STATUS_USABLE,
   IMA_LINK_STATUS_ACTIVE
} ;

// LIM Layer1 configurations

typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_ETHER or LIM_FAMILY_GIGAETHER
  UINT32 limAnalogType;       // LIM_AN_TYPE_ETHTX or LIM_AN_TYPE_ETHFX or LIM_AN_TYPE_GIGABIT
  UINT32 limDigType;          // LIM_DIG_TYPE_A
  UINT32 interfaceType;       // fast ethernet mode
  UINT32 dataRate;            // lineSpeed -- 10MB or 100MB or 1GB
  UINT8  hubMACaddr[6];
  UINT8  nodMACaddr[6];
  UINT32 hardwareConnection;
  UINT32 fullDuplex;          // FALSE for half duplex, TRUE for full duplex
  UINT32 mediaConnection;
  UINT32 jabberSize;          // Minimum size for a Jabber frame in bytes (non-VLAN)

} LimL1CfgLAN;


typedef struct
{
  UINT32 limType;             // no type for WLAN--> set to LIM_NONE
  UINT32 limFamily;           // LIM_FAMILY_WLAN
  UINT8  driverConfigSize;    // number of bytes following in the driver config structure.
  UINT8  driverConfig[1];     // place holder for variable size driver structure.
} LimL1CfgWLAN;


// General Config bits for the 10/100/1000 Multiport Lim
typedef struct
{
  UINT32 reassembly:         1;    // 1=reassembly enabled. 0=disabled.
  UINT32 smartSlicing:       1;    // 1=smart Slicing enabled. 0=disabled
  UINT32 smartSlicingLength: 11;   // smart user plane slicing length (0 - 2047)
  UINT32 tunCtPlaneSlicing:  1;    // 1=tunnelled Ctrl Plane Slicing enabled. 0=disabled
  UINT32 ntunCtPlaneSlicing: 1;    // 1=non tunnelled Ctrl Plane Slicing enabled. 0=disabled

  UINT32 unused:             17;   // reserved for future use

} LimPortCfgGeneral;


// Status bits per port for the 10/100/1000 Multiport Lim
typedef struct
{
  UINT32 port:          4;  // port number 0-15
  UINT32 unused1:       2;  // reserved for future use (larger number of ports perhaps ?)
  UINT32 autoNeg:       2;  // 1=autonegotation status:  
                            //   0=disabled 1=ongoing 2=success 3=failed
  UINT32 interfaceType: 8;  // SFP Gbic Type, see LIM_LAN_GBICTypes (Must be 8 bits)
  UINT32 lineSpeed:    16;  // trying to reuse LIM_LAN_LINE_SPEED_XXXX above
                            // The speed constants may have to be redefined into a enumeration
						           // so that we wont need so many bits.
} LimPortStatus;


// Status Structure for the 10/100/1000 Multiport Lim
// Variable Sized structured depending on member portCount.
typedef struct
{
  UINT32 portCount;
  LimPortStatus status[1];

} LimMPortStatus;


// Port Config Structure for the 10/100/1000 Multiport Lim
// It must be multiple of 32-bit words
typedef struct
{
  UINT32 port;               // Which port this config applies to.
  UINT32 interfaceType;      // SFP Gbic Type, see LIM_LAN_GBICTypes
  UINT32 mediaConnection;    // see LIM_LAN_MEDIA_INT_NORM / _AUTO.
  UINT32 lineSpeed;          // see LIM_LAN_LINE_SPEED_XXXX above
  UINT32 jabberSize;         // Minimum size for a Jabber frame in bytes (non-VLAN)
  LimPortCfgGeneral genConfig; // General Config Bits
} LimPortLANCfg;

// Port Config Structure for the 10G Multiport Lim
// It must be multiple of 32-bit words
typedef struct
{
  UINT8  port;               // Which port this config applies to.
  UINT8  interfaceType;      // SFP Gbic Type, see LIM_LAN_GBICTypes
  UINT8  macAddr[6];         // Mac Address associated with port.
  UINT32 mediaConnection;    // see LIM_LAN_MEDIA_INT_NORM / _AUTO.
  UINT32 lineSpeed;          // see LIM_LAN_LINE_SPEED_XXXX above
  UINT32 jabberSize;         // Minimum size for a Jabber frame in bytes (non-VLAN)  
} LimPort10GLANCfg;



// Multiport 10/100/1000 Layer 1 Lim config
// It must be multiple of 32-bit words
// Steal some bits from aggregaton for IPM mode
// NOTE: If this structure is modified, the LimL1CfgMPortLANEx structure must
// also be modified correspondingly.
typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_MPORTGIGABIT
  UINT32 limAnalogType;       // LIM_AN_TYPE_8PORTGIGABIT or LIM_AN_TYPE_16PORTGIGABIT
  UINT32 limDigType;          // LIM_DIG_TYPE_A
  UINT32 aggregation :1;      // 0=disabled  1=enabled.
  UINT32 ipmMode     :2;      // 0=none 1=Fixed 2=Intelligent 3=Flexible
  UINT32 unused      :29;     // Unused
  UINT32 portEnable;		      // Bit mask: bit0 = port 1,  bit1=port2, ...
  UINT32 portCount;
  LimPortLANCfg  portCfg[1];  // First element of an array of 8 or 16 ports

} LimL1CfgMPortLAN;


// Multiport 10/100/1000 Layer 1 Lim config. This is the maximum size version
// of the LimL1CfgMPortLAN structure.
typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_MPORTGIGABIT
  UINT32 limAnalogType;       // LIM_AN_TYPE_8PORTGIGABIT or LIM_AN_TYPE_16PORTGIGABIT
  UINT32 limDigType;          // LIM_DIG_TYPE_A
  UINT32 aggregation :1;      // 0=disabled  1=enabled.
  UINT32 ipmMode     :2;      // 0=none 1=Fixed 2=Intelligent 3=Flexible
  UINT32 unused      :29;     // Unused
  UINT32 portEnable;		      // Bit mask: bit0 = port 1,  bit1=port2, ...
  UINT32 portCount;
  LimPortLANCfg  portCfg[16];

} LimL1CfgMPortLANEx;

enum LanTenGigaEthCardModeTypes
{
  CM_1_10_GIG = 0,            // 2x1 Gig + 2x10 Gig (Default)
  CM_1_GIG    = 1,            // 4x1 Gig
  CM_10_GIG   = 3             // 2x10 Gig, hi performance
};

// Base 10G Layer 1 Lim config. Layer 1 Lim config
// It must be multiple of 32-bit words
// NOTE: If this structure is modified, the LimL1Cfg5Port10GLANEx structure must
// also be modified correspondingly.
typedef struct
{
  UINT32 limType;             // LIM_TENGIGABIT
  UINT32 limFamily;           // LIM_FAMILY_TENGIGAETHER
  UINT32 limAnalogType;       // LIM_AN_TYPE_8PORTGIGABIT or LIM_AN_TYPE_16PORTGIGABIT
  UINT32 limDigType;          // LIM_DIG_TYPE_A
  UINT16 aggregation;         // 0=disabled  1=enabled.   
  UINT8  cardMode:3;          // 0 = 1/10G, 1 = 1G, 3 = 10G
  UINT8  pad:5;               // padding 
  UINT8  unused;              // balance of 16 bits stolen from aggregation for cardMode
  UINT32 portEnable;		  // Bit mask: bit0 = port 1,  bit1=port2, ...
  UINT32 portCount;
  LimPort10GLANCfg  portCfg[1];
} LimL1Cfg10GLAN;

// 5port 10G Layer 1 Lim config. This is the maximum size version
// of the LimL1Cfg5Port10GLAN structure.
typedef struct
{
  UINT32 limType;             // LIM_TENGIGABIT
  UINT32 limFamily;           // LIM_FAMILY_TENGIGAETHER
  UINT32 limAnalogType;       // LIM_AN_TYPE_8PORTGIGABIT or LIM_AN_TYPE_16PORTGIGABIT
  UINT32 limDigType;          // LIM_DIG_TYPE_A
  UINT16 aggregation;         // 0=disabled  1=enabled.   
  UINT8  cardMode:3;          // 0 = 1/10G, 1 = 1G, 3 = 10G
  UINT8  pad:5;               // padding 
  UINT8  unused;              // balance of 16 bits stolen from aggregation for cardMode
  UINT32 portEnable;		  // Bit mask: bit0 = port 1,  bit1=port2, ...
  UINT32 portCount;
  LimPort10GLANCfg  portCfg[5];
} LimL1Cfg5Port10GLANEx;


// Probe Config Structure for the Packet Portal system
// It must be multiple of 32-bit words
typedef struct
{
  UINT8  probeId[6];         // Which probe this config applies to.
  UINT16 probeHash;          // frame limInfo hash value used for this probe in data frames.
  UINT8  interfaceType;      // SFP Gbic Type, see LIM_LAN_GBICTypes
  UINT8  state;              // flags indicating various state info
                             //   Bit0: 1 or 0, probe is enabled or disabled by user.
                             //   Bit1: 1 or 0, probe is active or inactive
                             //   NOTE: this is internal app side only field (used by NA).
  UINT16 reserved;           // align to 32bits
  UINT32 lineSpeed;          // see LIM_LAN_LINE_SPEED_XXXX above  
} LimPacketPortalProbeCfg;

// Base PacketPortal Layer 1 Lim config.
// It must be multiple of 32-bit words
//  NOTE: In theory there may be 65535 probes, but in practice, currently at least, NA limits to 16.
typedef struct
{
  UINT32 limType;             // LIM_PACKETPORTAL
  UINT32 limFamily;           // LIM_FAMILY_PACKETPORTAL
  UINT32 limAnalogType;       // LIM_AN_TYPE_PACKETPORTAL
  UINT16 aggregation;         // Aggregate mode flag(0=disabled  1=enabled). 
                              //   NOTE: this is internal app side only field (used by NA)
  UINT16 probeCount;
  LimPacketPortalProbeCfg  probeCfg[1];
} LimL1CfgPacketPortal;

//NOTE: limit max probes in structure to 256 currently to keep max L1 config packet size down.
#define LIM_CFG_PP_MAX_PROBES 256
typedef struct
{
  UINT32 limType;             // LIM_PACKETPORTAL
  UINT32 limFamily;           // LIM_FAMILY_PACKETPORTAL
  UINT32 limAnalogType;       // LIM_AN_TYPE_PACKETPORTAL
  UINT16 aggregation;         // Aggregate mode flag(0=disabled  1=enabled). 
                              //   NOTE: this is internal app side only field (used by NA)
  UINT16 probeCount;
  LimPacketPortalProbeCfg  probeCfg[LIM_CFG_PP_MAX_PROBES]; 
} LimL1CfgPacketPortalEx;



typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_T1E1
  UINT32 limAnalogType;       // LIM_AN_TYPE_T1E1B or T1E1D or E1BNC
  UINT32 limDigType;          // LIM_DIG_TYPE_A or LIM_DIG_TYPE_B
  UINT32 interfaceType;       // T1_LINE or E1_LINE
  UINT32 recvMode;            // LIM_RM_xx value
  UINT32 lineCode;            // LIM_LC_xx
  // Primary data channel info
  UINT32 dataChanRate;        // see LimChannelRateType
  UINT32 dataChanType;        // see LimChannelType
  UINT32 recvChan[N_SIDES];   // data channel mask
  // Secondary data channel info (for multilink PPP, ISDN etc).
  UINT32 dataChanRateSec;     // see LimChannelRateType
  UINT32 dataChanTypeSec;     // see LimChannelType
  UINT32 recvChanSec[N_SIDES];// data channel mask
  // Voice channel info
  UINT32 voiceChan[N_SIDES];
  UINT32 pcmCode;             // see LimVoicePCMCodeType
  UINT32 buildOut[N_SIDES];
  UINT32 xmitClockSrc;        // see LimTransmitClockSourceType
  UINT32 dataSense;           // see LimDataSenseType
  UINT32 simulation;          // see LimSimType
  UINT32 framingType;         // see LimFramingType
  UINT32 connectorType;       // see LimT1E1ConnectorType

} LimL1CfgT1E1;

typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_T3E3
  UINT32 limAnalogType;       // LIM_AN_TYPE_T3E3
  UINT32 limDigType;          // LIM_DIG_TYPE_A or LIM_DIG_TYPE_B
  UINT32 interfaceType;       // T3_LINE or E3_LINE
  UINT32 recvMode;            // LIM_RM_xx value
  UINT32 dataChanRate;        // see LimChannelRateType
  UINT32 recvChan[N_SIDES];   // data channel masks
  UINT32 buildOut[N_SIDES];   // see LimTransmitBuildOutType
  UINT32 xmitClockSrc;        // see LimTransmitClockSourceType
  UINT32 dataSense;           // see LimDataSenseType
  UINT32 framingType;         // see LimFramingType
  UINT32 simulation;          // see LimSimType
  UINT32 ds1Framing;          // T3 only, see LimFramingType
  UINT32 ds1Channel;          // T3 only, channel number
  UINT32 t3SimMode;           // T3 only, see LimDS3SimModeType
  UINT32 e3FrameMask;         // E3 only, (12 or 16 bits)

} LimL1CfgT3E3;

typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_OC3 or LIM_FAMILY_OC12
  UINT32 limAnalogType;       // LIM_AN_TYPE_OC3 or LIM_AN_TYPE_OC12
  UINT32 limDigType;          // LIM_DIG_TYPE_A or LIM_DIG_TYPE_B
  UINT32 recvMode;            // LIM_RM_xx value
  UINT32 xmitClockSrc;        // see LimTransmitClockSourceType
  UINT32 framingType;         // see LimFramingType
  UINT32 simulation;          // see LimSimType 

} LimL1CfgOC3;


typedef struct
{
  UINT32 mode        :1;      // See LimOctopMode: SDH or SONET
  UINT32 path        :2;      // See LimOctopPath: Bulk, T1 or E1
  UINT32 framing     :5;      // See LimFramingType
  UINT32 ICPSuppress :1;      // ICP Cell Suppress - 1=enabled, 0=disabled.
  UINT32 AUMode      :1;      // 0 = AU3  1 = AU4
  UINT32 swap        :1;      // swap up/down link: 0=Normal  1=Swapp.
  UINT32 K1K2Enabled :1;      // K1K2 Bytes Enabled  0=disabled 1=enabled
  UINT32 K1K2Ring    :1;      // K1K2 Bytes Topology 0=Linear 1=Ring
  UINT32 unused      :19;     // Padding to make it 32 bit wide.
  UINT32 tribEnable[3];       // Tributary Enable Bit vector
} LimOctopCfg;


// structure for the OC12 framing type
typedef struct
{
  UINT32 fibre       :2;  // LimOC12fibre => AUTO OC3 OC12
  UINT32 MappingType :3;  // LimOC12MappingType => STS12C STM4C STS1 STS3C VC3 VC4
  UINT32 AugInst     :3;  //
  UINT32 Au3Inst     :2;  //
  UINT32 PDHtype     :4;  // LimOC12PDHtype STS or VC selections
  UINT32 TUG3        :4;  // 
  UINT32 Trib2       :4;  // 
  UINT32 Trib1       :1;  //
  UINT32 DS0Speed    :1;
  UINT32 DS1Framing  :1;  // LIM_OC12_DS1_SF or LIM_OC12_DS1_ESF
  UINT32 DS3Framing  :1;  // LIM_OC12_DS3_M13 or LIM_FT_CBIT, if we're using DS1 from T3/DS3
  UINT32 useMask     :1;  // 0 means use all ds0's, 1 means refer to the chanMask
  UINT32 TADMode     :1;  // LimOC12TADMode
  UINT32 SyncMode    :1;  // LimOC12SyncMode
  UINT32 E3Framing   :1;  // LimOC12E3Framing
  UINT32 notused     :2;

  UINT32 chanMask;      // bitmask for which of the 24 DS0s in DS1, or of the 31 DS0s in E1

} LimOC12framing;


// This structure allows the configuration of multiplexed tributaries per port.
// 
// The structure Array member tribEnable can enable/disable tributary per port.
// Up to 84 Tributaries can be enabled per port. One bit per tributary per port.
// So It is necessary 3 x UINT32 (4 bytes each) = 96 bits. 
// We only use the first 84 bits when tributaries are T1s.
// 1=enabled, 0=disabled
// If a port is bulkmapped, then the corresponding row in this array 
// is not used.

typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_OC12
  UINT32 limAnalogType;       // LIM_AN_TYPE_OC12, LIM_AN_TYPE_4PORTOC3
  UINT32 limDigType;          // LIM_DIG_TYPE_B, LIM_DIG_TYPE_C, LIM_DIG_TYPE_D
  UINT32 xmitClockSrc;        // LIM_TC_INTERNAL_CLOCK
  LimOC12framing framingType[N_SIDES];
  UINT32 mode;                // see LimOC12mode
  LimOctopCfg     portCfg[OCTOP_NUM_OF_PORTS];
} LimL1CfgOC12;

typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_VSERIES
  UINT32 limAnalogType;       // LIM_AN_TYPE_VSERIES
  UINT32 limDigType;          // LIM_DIG_TYPE_A
  UINT32 interfaceType;       // see LimWanInterfaceType (V Series specific ones)
  UINT32 dataSense;           // see LimDataSenseType
  UINT32 sync ;               // bool (TRUE=SYNC, FALSE=ASYNC)
  UINT32 xmitClockSrc;        // see LimTransmitClockSourceType
  UINT32 baud;                // bit per second
  UINT32 cableType;           // see LimVseriesCableType
  UINT32 simulation;          // see LimSimType 
  
} LimL1CfgVseries;

typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_UTP25
  UINT32 limAnalogType;       // LIM_AN_TYPE_UTP25
  UINT32 limDigType;          // LIM_DIG_TYPE_B
  UINT32 recvMode;            // LIM_RM_BRIDGED or LIM_RM_TERMINATED
  UINT32 xmitClockSrc;        // LIM_TC_INTERNAL
  UINT32 framingType;         // see LimFramingType
  UINT32 simulation;          // LIM_NO_SIM
  UINT32 loopControl;         // response to loopcontrol
  UINT32 simCtrlCA;           // CA on/off
  UINT32 simCtrlLC;           // LC on/off
  UINT32 testMode;            // on/off/autoreply
  UINT32 simCtrlTA;           // TA on/off
  UINT32 simCtrl;

} LimL1CfgATM25;

typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_HSSI
  UINT32 limAnalogType;       // LIM_AN_TYPE_HSSI
  UINT32 limDigType;          // LIM_DIG_TYPE_B
  UINT32 dataSense;           // see LimDataSenseType
  UINT32 baud;                // bit per second
  UINT32 crcMode;             // CRC 16 or CRC 32
  UINT32 simulation;          // see LimSimType
  UINT32 simCtrlCA;           // CA on/off
  UINT32 simCtrlLC;           // LC on/off
  UINT32 testLeadCtrl;        // TM on/off
  UINT32 simCtrlTA;           // TA on/off
  UINT32 simCtrlLA;           // LA on/off (Local DTE)
  UINT32 simCtrlLB;           // LB on/off (Local Line)
  UINT32 loopControl;   // response to loop control
  
} LimL1CfgHssi;

 
typedef enum
{
   MODE_IMA_RESEQUENCE = 0,
   MODE_IMA_NORESEQUENCE,
   MODE_CHANNELIZATION,
   MODE_NORMAL,
   MODE_MULTIPORT_ISDN,
   MODE_MULTIPORT_ATM,
   MODE_MULTILINK_WAN,
   MODE_MULTIPORT_WAN,
   MODE_AGGREGATE,
   MODE_NON_AGGREGATE
}  LimApplicationMode;


typedef struct
{
  UINT32 limType;
  UINT32 limFamily;           // LIM_FAMILY_8PORTT1E1
  UINT32 limAnalogType;       // LIM_AN_TYPE_8PORTT1E1
  UINT32 limDigType;          // LIM_DIG_TYPE_B
  UINT32 interfaceType;       // LIM_LINE_T1 or LIM_LINE_E1
  UINT32 recvMode[8];          // LIM_RM_xx value
  UINT16 swapUpDown[8];        // Swap uplink/downlink - 1=enabled, 0=disabled
  UINT16 lineCode[8];          // LIM_LC_xx
  UINT32 dataChanRate[8];      // see LimChannelRateType
  UINT32 recvChan[N_SIDES][8]; // data channel mask
  UINT32 buildOut[N_SIDES];    // see CTransmitBuildOutType
  UINT32 xmitClockSrc;         // see LimTransmitClockSourceType
  UINT32 dataSense[8];         // see LimDataSenseType
  UINT32 simulation;           // see LimSimType
  UINT32 framingType[8];       // see LimFramingType
  UINT32 connectorType;        // LIM_CT_RJ48C
  UINT32 signalLevel;          // LIM_LINE_T1 or LIM_LINE_T1DSX
  UINT32 mode;                 // MODE_IMA_RESEQUENCE, MODE_IMA_NORESEQUENCE or MODE_CHANNELIZATION or MODE_NORMAL, MODE_MULTIPORT_ISDN
  UINT32 ports;
  UINT32 limInfoConfigSize;
  UINT32 interfaceMode;
  UINT8  imaGroup[8];          // Ima Group per port.
} LimL1Cfg8PortT1E1;


// LIM Layer 2 config constants
enum 
{
   LIM_SINGLE_PORT_T1,
   LIM_SINGLE_PORT_E1,
   LIM_MULTI_PORT_T1,
   LIM_MULTI_PORT_E1
};

// the variety of layer2 protocols
enum // NOTE: These constants map to protocol Ids in globals.h
{
  LIM_L2_ETHER       = 108, // use LimL2CfgEther
  LIM_L2_FRAME_RELAY = 400, // use LimL2CfgFR
  LIM_L2_ATM         = 200, // use LimL2CfgATM
  LIM_L2_LAPB        = 202, // use LimL2CfgBOP
  LIM_L2_LAPD        = 203, // use LimL2CfgLAPD
  LIM_L2_HDLC        = 204, // use LimL2CfgBOP
  LIM_L2_SDLC        = 205, // use LimL2CfgBOP 
  LIM_L2_RFC1662     = 206, // use LimL2CfgPPP
  LIM_L2_RFC1663     = 207, // use LimL2CfgPPP
  LIM_L2_CISCOSLE    = 208, // use LimL2CfgBOP
  LIM_L2_RFC2615     = 210, // use LimL2CfgPPP  <- just guessing this protocolID
  LIM_L2_SS7_MTP2    = 250  // use LimL2CfgFR
};

enum // scramblingState for PoS
{
  LIM_SCRAMBLE_AUTO = 1,
  LIM_SCRAMBLE_OFF,
  LIM_SCRAMBLE_ON
};

enum // Fcs Type for POS and WAN 
{
  LIM_FCS_TYPE_AUTO = 1, // POS only
  LIM_FCS_TYPE_16,
  LIM_FCS_TYPE_32,
  LIM_FCS_TYPE_16_0, // WAN only
  LIM_FCS_TYPE_32_0  // WAN only
};


// LIM Layer2 configurations

typedef struct // Ethernet
{
  UINT32 layer2protocol; // Ether protocol ID
                         // There are currently no other configurations for Ethernet
} LimL2CfgEther;

typedef struct // Frame Relay
{
  UINT32 layer2protocol; // Frame Relay protocol ID.
  UINT32 fcsType;        // CRC16CCITT, CRC32CCITT, CRC16CCITT-0, CRC32CCITT-0
                         // There are currently no other configurations for FR.
} LimL2CfgFR;

typedef struct // LAPD
{
  UINT32 layer2protocol; // LAPD protocol ID.
  UINT32 fcsType;        // CRC16CCITT, CRC32CCITT, CRC16CCITT-0, CRC32CCITT-0
                         // There are currently no other configurations for LAPD.

} LimL2CfgLAPD;

typedef struct // ATM
{
  UINT32 layer2protocol;

  // HECerrorCorrX sets Hec error Correction per port.
  // HECerroCorr0 is used for single port lims.
  UINT32 HECerrorCorr0    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 HECerrorCorr1    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 HECerrorCorr2    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 HECerrorCorr3    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 HECerrorCorr4    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 HECerrorCorr5    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 HECerrorCorr6    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 HECerrorCorr7    :1;   // bool 1=Enabled  0=Disabled.
  UINT32 unused1          :24;  // UINT32 padding

  // CellTypeX sets UNI/NNI per port. CellType0 is used for single port lims
  UINT32 cellType0        :1;  // 0=UNI, 1=NNI
  UINT32 cellType1        :1;  // 0=UNI, 1=NNI
  UINT32 cellType2        :1;  // 0=UNI, 1=NNI
  UINT32 cellType3        :1;  // 0=UNI, 1=NNI
  UINT32 cellType4        :1;  // 0=UNI, 1=NNI
  UINT32 cellType5        :1;  // 0=UNI, 1=NNI
  UINT32 cellType6        :1;  // 0=UNI, 1=NNI
  UINT32 cellType7        :1;  // 0=UNI, 1=NNI
  UINT32 unused2          :24; // UINT32 padding

  UINT32 cellSyncType;   // 0=HEC, 1=PLCP (T3 only)

  // cellScramblerX sets scrabling per port. cellScrambler0 is used for single port lims
  UINT32 cellScrambler0   :1;  // bool 1=Enabled 0=disabled
  UINT32 cellScrambler1   :1;  // bool 1=Enabled 0=disabled
  UINT32 cellScrambler2   :1;  // bool 1=Enabled 0=disabled
  UINT32 cellScrambler3   :1;  // bool 1=Enabled 0=disabled
  UINT32 cellScrambler4   :1;  // bool 1=Enabled 0=disabled
  UINT32 cellScrambler5   :1;  // bool 1=Enabled 0=disabled
  UINT32 cellScrambler6   :1;  // bool 1=Enabled 0=disabled
  UINT32 cellScrambler7   :1;  // bool 1=Enabled 0=disabled
  UINT32 unused3          :24; // UINT32 padding

  // cellSquelchX sets scrabling per port, X=0-7. 
  // cellSquelch0 may be used for single port lims
  UINT32 cellSquelch0     :1;  // bool 1=Enabled 0=disabled
  UINT32 cellSquelch1     :1;  // bool 1=Enabled 0=disabled
  UINT32 cellSquelch2     :1;  // bool 1=Enabled 0=disabled
  UINT32 cellSquelch3     :1;  // bool 1=Enabled 0=disabled
  UINT32 cellSquelch4     :1;  // bool 1=Enabled 0=disabled
  UINT32 cellSquelch5     :1;  // bool 1=Enabled 0=disabled
  UINT32 cellSquelch6     :1;  // bool 1=Enabled 0=disabled
  UINT32 cellSquelch7     :1;  // bool 1=Enabled 0=disabled
  UINT32 unused4          :24; // UINT32 padding

  UINT32 bert;           // LimBertType

} LimL2CfgATM;


typedef struct // BOP (LAPB, HDLC, SDLC, CISCOSLE)
{
  UINT32 layer2protocol;
  UINT32 extendedAddress;
  UINT32 extendedControl;
  UINT32 fcsType;        // POS - 16, 32, auto CCITT
                         // WAN - CRC16CCITT, CRC32CCITT, CRC16CCITT-0, CRC32CCITT-0
  UINT32 scramblingState;

} LimL2CfgBOP;

typedef struct // PPP (RFC1662 and RFC1663)
{
  UINT32 layer2protocol;
  UINT32 extendedControl;  // RFC1663 only
  UINT32 ACFcompression;   // RFC1662 only
  UINT32 upperProtFieldCompression;
  UINT32 multiPPPshortSeqNum;
  UINT32 lowerProtFieldCompression;

  UINT32 fcsType;        // POS - 16, 32, auto CCITT
                         // WAN - CRC16CCITT, CRC32CCITT, CRC16CCITT-0, CRC32CCITT-0
  UINT32 scramblingState;

} LimL2CfgPPP;

#endif
