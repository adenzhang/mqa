#ifndef __BUFFERFORMAT_H
#define __BUFFERFORMAT_H
// ****************************************************************************
//
// File:        BufferFormat.h
//
// Description: Software buffer format for Network Analyzer
//
// Project:     Used in all portions of the Network Analyzer project
// 
// Copyright 2010-2011 by JDSU.
//
// $Log: BufferFormat.h $
// Revision 33 2011/08/26 21:01:15 +0800 ive55253 /AnalyzerCommon/7.20.000_Tip/ANT_SASE_RELEASE_7.20.000.008/7.20.000.008.PacketPortal-Dev-BillIves
// Minor change to mp gig status structure for Packet Portal.
// 
// Revision 30 2010/08/18 10:34:11 -0600 mah55258 /AnalyzerCommon/6.65_Extensions/6.65.102/RecFilePlayback
// Fix comment for gbicType field in BufFmtTenGigLinkStatusBits structure.
// 
// Revision 28 2010/05/17 19:15:26 -0600 zijiyuan /AnalyzerCommon/6.70.100_Tip/ANT_SASE_RELEASE_6.70.100.082/6.70.100.082-SartRocket-GbicType-1-Zijing
// backout TenGig changes
// 
// Revision 27 2010/05/17 14:36:47 +0800 zijiyuan /AnalyzerCommon/6.70.100_Tip/ANT_SASE_RELEASE_6.70.100.082/6.70.100.082-SartRocket-GbicType-1-Zijing
// Add GBIC type definition for blade rocket
// 
// Revision 25 2010/04/21 01:34:41 +0800 sb4450 /AnalyzerCommon/6.70.100_Tip/10_Merge_6.65.100.028
// Automatic merge of parallel changes into revision [23]
// 
// Revision 24 2010/01/22 09:47:35 -0700 sb4450 /AnalyzerCommon/6.70.100_Tip/SmartSlicing/Scott
// HW Smart Slicing Changes
// 
// Revision 23 2009/11/03 14:03:29 -0700 bi4723 /AnalyzerCommon/6.65.100_Tip/BladeRocket
// Added unsupport gbic type to macro UnknownBuffFmtGBIC to be
// consistent with what we do for lim driver gbic checks.
// 
// Revision 22 2009/07/30 14:32:40 -0600 tm4457 /AnalyzerCommon/6.60.100_Tip/BladeRocket/tm4457
// Update BuffFmtLANGBicTypes for Ten Gig.
// 
// Revision 21 2009/07/14 13:47:50 -0600 tm4457 /AnalyzerCommon/6.50.100_Tip/BladeRocket/tm4457
// Add new BuffFmtLANGBicTypes for Ten Gig.
// 
// Revision 20 2009/04/23 12:15:04 -0600 bi4723 /AnalyzerCommon/6.50.100_Tip/BladeRocket
// Change comment on gbic type value for 10G to be clear about
// what constants are used (at least for now).
// 
// Revision 19 2009/04/09 09:37:09 -0600 bi4723 /AnalyzerCommon/6.50.100_Tip/BladeRocket
// Removed buffer size constants that terry m put in since we don't want NA specific --
// DAT file oriented and NDIS limited values -- in the system.  These values were only
// intended for use by NA, independent of hardware lim.  The idea was to setup the UI
// by detecting which value is the "max" and to have flexibility in adding or removing
// actual capture buffer size options.
// 
// Revision 18 2009/02/19 10:55:23 -0700 tm4457 /AnalyzerCommon/6.50.100_Tip/BladeRocket/tm4457
// Move buffer size definitions to this file so that they can be accessed by the TenGigDataSouceDll.
// 
// Revision 17 2008/11/24 10:28:38 -0700 tm4457 /AnalyzerCommon/6.50.100_Tip/BladeRocket/tm4457
// Add support for Ten Gigabit Ethernet interface
// 
// Revision 16 2008/11/07 10:25:25 -0700 thohenry /AnalyzerCommon/6.50.100_Tip/BladeRocket/TH5605
// Update to include BladRocket card.
// 
// Revision 15 2008/03/13 11:21:04 -0600 thohenry /AnalyzerCommon/6.00.100_Tip/HDDNA
// Add orange to list of possible line status colors.
// 
// Revision 14 2008/01/17 13:25:47 -0700 thohenry /AnalyzerCommon/6.00.100_Tip/VLan/HDDNA/MobileDNA/TH5605
// Changes to support APS K Bytes Status.
// 
// Revision 13 2008/01/14 14:22:25 -0700 jkronba /AnalyzerCommon/6.00.100_Tip/MobileDNA
// Redeclare OC3 status bits structure due to compiler errors, a new OC3 status bits structure is now
// used.
// 
// Revision 11 2007/12/20 12:41:35 -0700 tm4457 /AnalyzerCommon/6.00.100_Tip/MobileDNA/tm4457
// Changed Amplitude in BufEthVitals back to UINT32 as the previous change to INT16 was causing an
// increase in the size of the structure and subsequently breaking a lot of other code.
// 
// Revision 10 2007/12/06 08:54:33 -0700 thohenry /AnalyzerCommon/6.00.100_Tip/MobileDNA/TH5605
// Amplitude in BufEthVitals changed to INT16 to preserve sign.
// 
// Revision 9 2007/11/16 11:14:36 -0700 thohenry /AnalyzerCommon/5.60.100_Tip/MobileDNA/TH5605
// Protected inline bool functions in BufferFormat.h from begin compiled in extern "C" blocks.
// 
// Revision 8 2007/10/22 13:33:52 -0600 jkronba /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Automatic merge of parallel changes into revision [7]
// 
// Revision 7 2007/10/19 11:56:42 -0600 jkronba /AnalyzerCommon/5.60.100_Tip/MobileDNA
// Merge fixes
// 
// Revision 6 2007/09/25 16:40:22 -0600 tm4457 /AnalyzerCommon/5.60.100_Tip/MobileDNA/tm4457
// Add support for 8/16 port Gig LIM.
// Revision 5 2007/07/09 14:34:56 -0600 jkronba /AnalyzerCommon/5.50.100_Tip/DNA_Measurement_Merge/DNA_Measurement
// Fix typo on structure BufFmtMPGigLinkStatusBits
// 
// Revision 3 2007/05/30 13:25:49 -0600 thohenry /AnalyzerCommon/5.40.100_Tip/MobileDNA/TH5605
// Add Support for 8/16 port GLIM from Platform version of this file.
// 
// Revision 2 2007/02/22 09:17:55 -0700 bi4723 /AnalyzerCommon/5.40.400_Tip/5.40.100_5.40.300Merge
// Merge in fix from original BufferFormat.h in Platform for SCR 4322. Added new SWBITS constant for RTSM.
// 
// Revision 1 2006/06/21 08:41:40 -0600 bi4723 /AnalyzerCommon/5.20_Release/NA_Multiuser2
// Moved this shared file from Platform database to AnalyzerCommon.
// 
// Revision 61 2006/02/01 07:45:35 -0700 sc4495 /Platform/GoldenGate/MultiportWan
// Automatic merge of parallel changes into revision [60]
// 
// Revision 60 2005/11/30 11:25:10 -0700 jm4399 /Platform/4.80_Extensions/4.80_Releases/4.80.110/Patches/4.80.111/jm4399/a
// Changes for Multi-Link FR & new multi-pattern encaps
// 
// Revision 59 2005/08/29 08:12:12 -0600 mg4364 /Platform/GoldenGate/Hemi
// Added new buffer types for ScramJet
// 
// Revision 58 2005/06/22 13:39:31 -0600 jx4600 /Platform/4.80_Extensions/4.80.110/Octopus1.5
// accomodate lim info bytes change per Jerry M.'s request
// 
// Revision 57 2005/06/02 09:52:03 -0600 jx4600 /Platform/4.80_Extensions/Patches/4.80.110/Octopus1.5
// add one constant for octopus 1.5 to fit the lim header info format change
// 
// Revision 56 2004/11/10 09:37:25 -0700 jm4399 /Platform/4.60_Release/ClearChannel/Development/jm4399
// Started adding ATM 4 PORT OC3
// 
// Revision 55 2004/02/26 15:17:23 -0700 jx4600 /Platform/4.00_Release/HighDensityT1
// Automatic merge of parallel changes into revision [54]
// 
// Revision 54 2004/02/25 12:21:11 -0700 jm4399 /Platform/GeorgeTown/HighDensityT1
// Added Channalization LIM_INFO length.
// Revision 53 2004/01/22 10:00:48 -0700 sc4495 /Platform/GeorgeTown/HighDensityT1
// Removed Pos LEDs and renamed ATMLCD
// 
// Revision 52 2003/12/08 09:36:28 -0700 jm4399 /Platform/GeorgeTown/HighDensityT1/IMA/jm4399
// Added some LIM INFO values for IMA.
// Revision 51 2003/11/25 07:22:51 -0700 sc4495 /Platform/Cumbres/OC12
// Fixed the comment for REI
// 
// Revision 49 2003/11/10 15:26:22 -0700 sc4495 /Platform/GeorgeTown/HighDensityT1/IMA/jm4399
// Back to original version
// Revision 50 2003/11/24 18:49:13 -0700 sc4495 /Platform/Cumbres/OC12
// Changed RDI to REI to be correct
// 
// Revision 45 2003/11/03 09:28:45 -0700 jx4600 /Platform/GeorgeTown/HighDensityT1/IMA/sc4495
// Revision 44 2003/10/09 14:58:04 -0600 jkronba /Platform/Cumbres/OC12
// Automatic merge of parallel changes into revision [40]
// Check in after merge, fix revision comments.
// 
// Revision 43 2003/10/01 12:48:34 -0600 jx4600 /Platform/GeorgeTown/HighDensityT1/IMA
// Revision 42 2003/08/22 08:04:50 -0600 jq4394 /Platform/Cumbres/OC12
// Automatic merge of parallel changes into revision [41]
//
// Revision 41 2003/07/08 09:28:32 -0600 jq4394 /Platform/Analyzer_Tip/Gig Benchmarking
//
// Revision 40 2003/06/02 16:51:41 -0600 af4768 /Platform/Lim_Tip/OC_12
// changed BUFFMT_LS 2 from GRAY to TOGL
// 
// Revision 39 2003/05/28 15:20:17 -0600 af4768 /Platform/OC12_WANL2_Dev/OC_12
// define the TOGL status bit
// 
// Revision 42 2003/08/22 08:04:50 -0600 jq4394 /Platform/GeorgeTown/HighDensityT1/IMA
// Automatic merge of parallel changes into revision [41]
// Revision 27 2003/02/05 09:57:46 -0700 sc4495 /Platform/OC12_WANL2_Dev/OC_12
// Automatic merge of parallel changes into revision [25]
// 
// Revision 26 2003/01/31 10:30:53 -0700 sc4495 /Platform/OC12_WANL2_Dev
// Automatic merge of parallel changes into revision [24]
//
// Revision 25 2003/01/17 14:24:07 -0700 af4768 /Platform/OC12_WANL2_Dev
// update the OC12 alarm bits
// 
// Revision 41 2003/07/08 09:28:32 -0600 jq4394 /Platform/Analyzer_Tip/Gig Benchmarking
// Revision 24 2003/01/16 08:13:04 -0700 st4126 /Platform/1.2_RTSD_Merge
// Merge of 1.2_RTSD_Dev, 2.0_Release, and NA_RTSD_Dev.
//
// Revision 23 2002/12/13 11:24:37 -0700 af4768 /Platform/OC12_WANL2_Dev/OC_12
// status bits for OC12
// 
// Revision 22 2002/12/12 12:36:28 -0700 st4126 /Platform/1.2_RTSD_Dev
// Keystone SCR #2509: end of runtime indication in RT frame data
// 
// Revision 19 2002/10/22 10:51:20 -0600 sc4495 /Platform/1.2_RTSD_Dev
// Merge from RTSD DEv Snapshot
// 
// Revision 18 2002/10/15 10:20:39 -0600 sc4495 /Platform/NA_RTSD_Dev
// Fixed comment order after 1.2 to NTC merge
// 
// Revision 17 2002/10/14 15:45:30 -0600 bv4736 /Platform/1.2_Temp_Merge
// Automatic merge of parallel changes into revision [12]
// 1.2 to NTC merge
// 
// Revision 16 2002/10/02 15:05:28 -0600 tm4457 /Platform/1.2_RTSD_Dev/RTSD
// 
// Revision 13 2002/09/25 11:07:33 -0600 af4768 /Platform/1.2_Temp_Merge
// gigabit baseT subtype
// 
// Revision 12 2002/09/19 17:54:54 -0600 jkronba /Platform/1.2_Temp_Merge
// Check in with the companion files: The optimization has been turned off as it
// was causing the BufferDriver Init to fail
// 
// Revision 10 2002/09/13 10:40:03 -0600 bi4723 /Platform/1.2_Dev
// Added first pass WLAN constants
// 
// Revision 4 2002/07/17 15:55:42 -0600 af4768 /Platform/1.2_Dev
// 
// Revision 3 2002/07/15 09:13:23 -0600 sc4495 /Platform/1.2_Dev
// Moved BufFmtUTP25LineStatusBits from limAtm25.cpp
// 
// Revision 2 2002/05/31 10:33:14 -0600 st4126 /Platform/1.2_Dev
// from Patch2
// 
// 22    3/25/02 10:36a Jm4399
// Check in from 1.1.4
// 
// 20    2/21/02 8:22a Sb4450
// Added AAL2 PF frame type for use in PIF.
// 
// 19    1/28/02 10:43a Sb4450
// Added limInfo values for AAL 1,2,3/4
// 
// 18    02/01/24 12:30 Af4768
// added a monitor capable bit to the ethernet status
// 
// 16    01/12/12 12:11 Af4768
// 
// 14    01/10/23 15:32 Af4768
// more Vseries status bits
// 
// 13    10/09/01 5:28p Bk4415
// added atm error mask
// 
// 12    8/20/01 11:12p Sb4450
// Added ATM limInfo constants.
// 
// 11    8/09/01 11:02a Bi4723
// Renamed ethernetStatusReg to proper format for this header file.
// 
// 10    8/08/01 17:51 Af4768
// moved the ethernet StatusBits struct here
// 
// 9     7/26/01 11:47 Af4768
// corrected a length error in a statusBits struct
// 
// 8     6/15/01 10:03a Bv4736
// Added support for limInfo fields.
// 
// 7     6/14/01 4:36p Bi4723
// Fixed spelling error in vseries line status structure.  Also added
// status bit mask definitions for Eth, TR, WAN and ATM.
// 
// 6     6/08/01 2:25p Bi4723
// Added line status bit field structures and comments.
// 
// 5     6/05/01 4:03p Bv4736
// Added limInfo fields to BufFmtFrameHeader and eventInfo6 to
// BufFmtEventHeader so the records are the same length.
// 
// 4     5/24/01 11:00a St4126
// Changed event header to have 5 info fields and no length field.
// 
// 3     4/04/01 10:55a St4126
// Changed BufFmtFrameHeader.recordType to be type number only (no status
// bits)
// Changed BufFmtFrameHeader.reserved to .softwareBits
// Moved the status bits which were previously in .recordType to
// .softwareBits
// Changed BufFmtFrameHeader.otherBits to .hardwareBits
// Changed the names of constants for the various status bits
// 
// 2     4/02/01 3:27p St4126
// changed BUFDRV... to BUFFMT...
// removed *data fields from structures
// changed structure names to "...Header" rather than "...Record"
// 
// 1     3/27/01 12:29p St4126
// Shared between PC code and embedded code.
// First version of this file extracted from BufferDriver.h.
// 
// 
// 
// ****************************************************************************

// ****************************************************************************
//
// Implementation Notes:
//
// We have defined a new buffer data format for Keystone.  The intent of this
// is to provide a format which can remain unchanged for all LIMs and even when
// the underlying acquisition hardware changes.  This format is used for data
// captured from the line, for nonruntime buffer data, and for data loaded into
// the buffer from a file.
//
// This include file is shared between the PC side and the embedded side.  Any
// time a change is made to this file, a message should be sent to all software
// developers to alert them to get the latest copy from SourceSafe.
//
// PREREQUISITES:
// ???????.h:               Standard type definitions.
//
// OTHER DOCUMENTS:
//
// ****************************************************************************



// ****************************************************************************
//                              CONSTANTS
// ****************************************************************************

// BufFmtRecord.frame.recordType and BufFmtRecord.event.recordType: this tells
// what record type we have.  These two fields map to the same physical byte in
// the structure, so that you can use either access method to distinguish between
// frame and event records.
#define BUFFMT_TYPE_EMPTY           (0)     /* Used when the buffer is empty */
#define BUFFMT_TYPE_DATA            (1)     /* Created by BufferDriver */
#define BUFFMT_TYPE_LINE_STATUS     (2)     /* Created by LIM Driver */
#define BUFFMT_TYPE_TIME            (3)     /* Created by BufferDriver */
#define BUFFMT_TYPE_DATA_BLOCK      (4)     /* Created by BufferDriver */
#define BUFFMT_TYPE_PTR_LIST_BLOCK  (5)     /* Created by BufferDriver */

// Possible values for BufFmtRecord.frame.limSide and BufFmtRecord.event.limSide:
// This is a partial list to cover the two physical data streams in the lines under
// test for our first wave of Keystone products.  In the future there may well be
// LIMs which decode a large number of data sources, such as time slices in a T1
// link.  BufferDriver will continue to work properly in these situations, as follows:
// -The LIM will deliver the data to the acquisition system on the two physical
//  receive busses into two hardware buffers.
// -The LIM will deliver data such that the frames on a given physical recieve bus
//  are in strict time order.
// -The LIM may assign any side/channel/stream/etc number it wishes to a given frame,
//  as long as it fits within the bits reserved for it in the receive channel data
//  format.
// -Buffer Driver will time correlate the data from the two hardware buffers.
// -Buffer Driver will move the side/channel/stream/etc number into the .limSide
//  field in the records given to its clients.  Buffer Driver does not look at
//  this field; it just passes it on.
// -Buffer Driver's clients will get data records with the .limSide field set
//  with whatever number the LIM sent.  Thus the application code and the LIM
//  hardware designer can agree on any coding they wish for this field.
//  In general I would expect additional channels to just use numbers starting
//  after the last symbol defined below.
//
// Now to define some symbols for the two commonly used channels.
// Note that for the two common channels there are several different names
// for each channel (eg DCE, LINE, HUB, NET), reflecting the historical
// names used in various protocols.  You should pick one pair of names and
// use them consistently within one module.
#define BUFFMT_SIDE_EVENT           (0)
#define BUFFMT_SIDE_DCE             (1)
#define BUFFMT_SIDE_LINE            (1)
#define BUFFMT_SIDE_NET             (1)
#define BUFFMT_SIDE_HUB             (1)
#define BUFFMT_SIDE_DTE             (2)
#define BUFFMT_SIDE_EQPT            (2)
#define BUFFMT_SIDE_NODE            (2)

// Bit definitions in BufFmtRecord.frame.softwareBits
// and BufFmtRecord.event.softwareBits.  If one of the bits below is set,
// then the corresponding condition exists.
// Currently, these bits are only set in data sent to the runtime APIs;
// Elwood nonruntime buffer data will have them clear.
#define BUFFMT_SWBITS_FRAME_DISCONT (0x80)  /* If set, we skipped one or more frames */
#define BUFFMT_SWBITS_EVENT_DISCONT (0x40)  /* If set, we skipped one or more events */
#define BUFFMT_SWBITS_LAST_RTRECORD (0x20)  /* If set, this is the last record of the run */
#define BUFFMT_SWBITS_STOP_RUN_PENDING (0x10)  /* If set, a stop run is in progress */

// Bit Masks for BufFmtRecord.frame.hardwareBits.  If one of the bits below is set,
// then the corresponding condition exists.  See your hardware documents for what
// these bits actually mean.
#define BUFFMT_HWBITS_TAGGED        (0x80)
#define BUFFMT_HWBITS_HALT          (0x40)
#define BUFFMT_HWBITS_CENTER        (0x20)
#define BUFFMT_HWBITS_START         (0x10)
#define BUFFMT_HWBITS_SHORT_DISCARD (0x08)
#define BUFFMT_HWBITS_SPARE3        (0x04)
#define BUFFMT_HWBITS_SPARE2        (0x02)
#define BUFFMT_HWBITS_SPARE1        (0x01)

// Common/shared status bit masks 
#define BUFFMT_END_STATUS_MASK             0x80

// Ethernet status bit masks 
#define BUFFMT_ETH_RUNT_MASK               0x40
#define BUFFMT_ETH_JABBER_MASK             0x20
#define BUFFMT_ETH_BAD_FCS_MASK            0x10
#define BUFFMT_ETH_DRIBBLE_MASK            0x08
#define BUFFMT_ETH_CHANNEL_MASK            0x04
#define BUFFMT_ETH_COLLISION_MASK          0x02
#define BUFFMT_ETH_REMOTE_COLLISION_MASK   0x01
#define BUFFMT_DNAHD_SLICED_MASK           0x01 // smart sliced bit for DNA HD

// TokenRing status bit masks
#define BUFFMT_TR_ED_MASK                  0x80
#define BUFFMT_TR_CODEV_MASK               0x40
#define BUFFMT_TR_ABORT_MASK               0x20
#define BUFFMT_TR_BAD_FCS_MASK             0x10
#define BUFFMT_TR_AC_MASK                  0x0C // 2BIT field
#define BUFFMT_TR_E_MASK                   0x02
#define BUFFMT_TR_I_MASK                   0x01

// WLAN status bit masks
#define BUFFMT_WLAN_FRAG_COUNT_MASK         0x000F0000
#define BUFFMT_WLAN_CHANNEL_MASK            0x0000F000
#define BUFFMT_WLAN_WEP_ICV_MASK            0x00000200
#define BUFFMT_WLAN_PLCP_SHORT_MASK         0x00000100
#define BUFFMT_WLAN_PLCP_HEC_MASK           0x00000080
#define BUFFMT_WLAN_RUNT_MASK               0x00000040
#define BUFFMT_WLAN_JABBER_MASK             0x00000020
#define BUFFMT_WLAN_BAD_FCS_MASK            0x00000010
//#define BUFFMT_X_DRIBBLE_MASK             0x00000008 // NOT USED ON WLAN 
#define BUFFMT_WLAN_DIRECTION_MASK          0x00000004
//#define BUFFMT_X_COLLISION_MASK           0x00000002 // NOT USED ON WLAN 
//#define BUFFMT_X_REMOTE_COLLISION_MASK    0x00000001 // NOT USED ON WLAN 

// Multi-port Gig status bit masks 
#define BUFFMT_MPGIG_SHORT_MASK     0x40
#define BUFFMT_MPGIG_LONG_MASK      0x20
#define BUFFMT_MPGIG_CRC_MASK       0x10
#define BUFFMT_MPGIG_TRUNCATED_MASK 0x08
#define BUFFMT_MPGIG_BROADCAST_MASK 0x04
#define BUFFMT_MPGIG_MULTICAST_MASK 0x02

// Definitions for the LIM Info Array for WLAN frames.
#define BUFFMT_WLAN_LIMINFO_LEN                      3
#define BUFFMT_WLAN_LIMINFO_DATA_RATE                0
#define BUFFMT_WLAN_LIMINFO_SIGNAL_LEVEL             1
#define BUFFMT_WLAN_LIMINFO_SIGNAL_TO_NOISE_LEVEL    2


// ATM status bit masks (with SAR mode on).
#define BUFFMT_ATM_FRAME_MASK              0x40
#define BUFFMT_ATM_DCRD_MASK               0x20
#define BUFFMT_ATM_CONG_MASK               0x10
#define BUFFMT_ATM_BADCRC_MASK             0x08
#define BUFFMT_ATM_BADHEC_MASK             0x04
#define BUFFMT_ATM_CORHEC_MASK             0x02
#define BUFFMT_ATM_TRUNC_MASK              0x01

// WAN status bit masks
#define BUFFMT_WAN_SHORT_MASK              0x08
#define BUFFMT_WAN_ABORT_MASK              0x04
#define BUFFMT_WAN_BADFCS_MASK             0x02
#define BUFFMT_WAN_GOODFCS_MASK            0x01

// Maximum number of limInfo bytes
#define BUFFMT_LIMINFO_MAX_LENGTH          (3)

// LimInfo values
//
// ATM LimInfo is always one byte
//
#define BUFFMT_ATM_LIMINFO_LEN                  (1)
#define BUFFMT_ATM_LIMINFO_SARINFO_MASK         0x03
#define BUFFMT_ATM_LIMINFO_SARINFO_MASK_4PORT   0x0f
#define BUFFMT_ATM_LIMINFO_CELL                 (0)
#define BUFFMT_ATM_LIMINFO_AAL1FRAME            (1)
#define BUFFMT_ATM_LIMINFO_AAL2FRAME            (2)
#define BUFFMT_ATM_LIMINFO_AAL34FRAME           (3)
#define BUFFMT_ATM_LIMINFO_AAL5FRAME            (4)
// NOTE: not in Charles's SAR document - used for PIF parsing of
// software reassembled data.
#define BUFFMT_ATM_LIMINFO_AAL2PFFRAME     (6)
//
// IMA LimInof can be configured to be one or three bytes long.
//
#define BUFFMT_IMA_LIMINFO_SHORT_LEN       (1)
#define BUFFMT_IMA_LIMINFO_LONG_LEN        (3)
// The first byte contains the ATM SAR info in addition to the following
#define BUFFMT_IMA_LIMINFO_1_IMAINFO_MASK  0xF0
//
// Channalization LimInof can be configured to be one or three bytes long.
//
#define BUFFMT_CHANALIZED_LIMINFO_LONG_LEN (3)
//
// OC3 ATM 4-port LIM
//
#define BUFFMT_ATM_FOUR_PORT_LIMINFO_LEN (3)
//
// Multi-port WAN
//
#define BUFFMT_MULTI_PORT_WAN_LIMINFO_LEN (3)


// ****************************************************************************
//                         LINE STATUS EVENT CONSTANTS
// ****************************************************************************

// NOTE: Bit fields have been tested for portability between MS VC++ (on the PC)
//       and Tornado 2.0 (XScale).  Must be re-tested for other environments.

typedef struct
{
  UINT32 LCD      :2;
  UINT32 LCV      :2; // BPV
  UINT32 LOF      :2;
  UINT32 LOS      :2;
  UINT32 AIS      :2;
  UINT32 RAI      :2; // yellow
  UINT32 FAS      :2; // frame bit error
  UINT32 CRC      :2; // CRC6 or CRC4
  UINT32 onesDen  :2;
  UINT32 excess0  :2;
  UINT32 unused   :11;
  UINT32 disabled :1;
} BufFmtT1E1LineStatusBits;

typedef struct
{
  UINT32 LCD      :2;
  UINT32 LCV      :2;
  UINT32 LOF      :2; // OOF
  UINT32 LOS      :2;
  UINT32 AIS      :2;
  UINT32 RDI      :2; // yellow alarm, remote alarm
  UINT32 REI      :2; // FEBE
  UINT32 BIP      :2;
  UINT32 mismatch :2;
  UINT32 P1P2par  :2;
  UINT32 CbitPar  :2;
  UINT32 plcpOOF  :2;
  UINT32 plcpBIP  :2;
  UINT32 plcpFEBE :2;
  UINT32 plcpRAI  :2;
  UINT32 unused   :1;
  UINT32 disabled :1;
} BufFmtT3E3LineStatusBits;


typedef struct
{
  UINT32 LCD      :2;
  UINT32 LCV      :2;
  UINT32 LOF      :2;
  UINT32 LOP      :2;
  UINT32 LOS      :2;
  UINT32 AISline  :2;
  UINT32 AISpath  :2;
  UINT32 RDIline  :2; // yellow
  UINT32 RDIpath  :2; // yellow
  UINT32 REIline  :2; // line FEBE
  UINT32 REIpath  :2; // path FEBE
  UINT32 B1Bip    :2;
  UINT32 B2Bip    :2;
  UINT32 B3Bip    :2;
  UINT32 unused   :3;
  UINT32 disabled :1;
} BufFmtOC3LineStatusBits;


// Used for 4-port OC3 to support K1K2 Bytes
typedef struct
{
  UINT32 LCD      :2;
  UINT32 LCV      :2;
  UINT32 LOF      :2;
  UINT32 LOP      :2;
  UINT32 LOS      :2;
  UINT32 AISline  :2;
  UINT32 AISpath  :2;
  UINT32 RDIline  :2; // yellow
  UINT32 RDIpath  :2; // yellow
  UINT32 REIline  :2; // line FEBE
  UINT32 REIpath  :2; // path FEBE
  UINT32 B1Bip    :2;
  UINT32 unused   :3; // Previously used for B3BIP
  UINT32 K1K2All  :1; // Previously used for B3BIP - 1=all K1K2 bits, 0=partial K1K2
  UINT32 K1K2On   :1; // K1K2 are being collected
  UINT32 K1K2Ring :1; // 1=Ring Topology, 0=Linear topology
  UINT32 newFormat:1; // 1=New Line Status Record Format, 0=Old format
  UINT32 disabled :1;
} BufFmtOC3LineStatusBits2;

typedef struct
{
  UINT32 LOF      :2; // SONET loss of frame
  UINT32 LOP      :2; // SONET loss of pointer
  UINT32 LOS      :2; // SONET loss of signal
  UINT32 AISline  :2; // SONET line alarm indication signal
  UINT32 AISpath  :2; // SONET path alarm indication signal
  UINT32 REIline  :2; // SONET line FEBE
  UINT32 REIpath  :2; // SONET path FEBE
  UINT32 LOFds3   :2; // DS3 loss of frame
  UINT32 AISds3   :2; // DS3 alarm
  UINT32 RDIds3   :2; // DS3 yellow
  UINT32 LOFds1   :2; // DS1 loss of frame
  UINT32 AISds1   :2; // DS1 alarm
  UINT32 RDIds1   :2; // DS1 yellow
  UINT32 ATMLCD   :2; // ATM LCD
  UINT32 unused1  :3;
  UINT32 disabled :1;
} BufFmtOC12LineStatusBits;

typedef struct
{
  UINT32 LCV      :2; // invalid symbol
  UINT32 LCD      :2; // short cell
  UINT32 LOS      :2; // bad signal
  UINT32 LOF      :2; // frame count not incrementing
  UINT32 FIF      :2; // FIFO overrun
  UINT32 HEC      :2; // HEC error
  UINT32 unused   :19;
  UINT32 disabled :1;
} BufFmtUTP25LineStatusBits;

typedef struct
{
  UINT32 DTE_SD   :2; // TXD
  UINT32 DCE_RD   :2; // RXD
  UINT32 TC_ST    :2; // TXC
  UINT32 RC_RT    :2; // RXC
  UINT32 EXT_CLK  :2; // SCTE
  UINT32 RTS      :2;
  UINT32 CTS      :2;
  UINT32 DTR      :2;
  UINT32 DSR_DM   :2; // DSR
  UINT32 CD_RR    :2; // DCD
  UINT32 RI       :2;
  UINT32 LL       :2;
  UINT32 RL       :2;
  UINT32 TM       :2;
  UINT32 unused   :3;
  UINT32 disabled :1;
} BufFmtVSeriesLineStatusBits;

typedef struct
{

  UINT32 RD       :2; 
  UINT32 SD       :2; 
  UINT32 ST       :2; 
  UINT32 RT       :2; 
  UINT32 TT       :2;
  UINT32 CA       :2;
  UINT32 TA       :2;
  UINT32 LA       :2;
  UINT32 LB       :2;
  UINT32 LC       :2;
  UINT32 TM       :2; 
  UINT32 unused   :9;
  UINT32 disabled :1;
} BufFmtHssiLineStatusBits;

// LED structure
typedef struct
{
  UINT32 LCD      :2;
  UINT32 LCV      :2; // BPV
  UINT32 LOF      :2;
  UINT32 LOS      :2;
  UINT32 AIS      :2;
  UINT32 RAI      :2; // yellow
  UINT32 FAS      :2; // frame bit error
  UINT32 CRC      :2;
  UINT32 onesDen  :2;
  UINT32 excess0  :2;
  UINT32 unused  :11;
  UINT32 disabled :1;
} BufFmt8PortT1E1LineStatusBits;

//because of the size limitation of line event data structrue,
//we have to change the size of fields to 1 bit
typedef struct
{
  UINT16 LCD         :1; //1: on, 0: off
  UINT16 LCV         :1; // BPV
  UINT16 LOF         :1;
  UINT16 LOS         :1;
  UINT16 AIS         :1;
  UINT16 RAI         :1; // yellow
  UINT16 FAS         :1; // frame bit error
  UINT16 CRC         :1; // CRC6 or CRC4
  UINT16 onesDen     :1;
  UINT16 excess0     :1;
  UINT16 amp         :4;
  UINT16 unused      :1;
  UINT16 disabled    :1;
} BufFmt8PortT1E1LineStatusBits2;

// status bit values for the above structures
enum BufFmtLineStatusType
{
  BUFFMT_LS_UNKN  = 0, // white, for uninitialized unknown or invalid
  BUFFMT_LS_WHITE = 0,
  BUFFMT_LS_OFF   = 1, // green
  BUFFMT_LS_GREEN = 1,
  BUFFMT_LS_TOGL  = 2, // sometimes yellow, but usually gray for not applicable or reserved
  BUFFMT_LS_GRAY  = 2,
  BUFFMT_LS_ON    = 3, // red
  BUFFMT_LS_RED   = 3,
  BUFFMT_LS_YELLOW= 4,
  BUFFMT_LS_BLUE  = 5,
  BUFFMT_LS_TEAL  = 6,
  BUFFMT_LS_ORANGE= 7
};

// Auto negotiated and link detected findings, for Ethernet and Gigabit
typedef struct
{
  UINT32   cardTXFX    : 2; // ETHER_TX or ETHER_FX or ETHER_NO_CARD
  UINT32   autoneg     : 1; // ETHER_NO_AUTO or ETHER_AUTO
  UINT32   negotiated  : 1; // ETHER_AUTO_NOT_COMPLETE or ETHER_AUTO_COMPLETE
  UINT32   speed       : 3; // ETHER_10MBPS or ETHER_100MBPS or ETHER_1000MBPS
  UINT32   duplex      : 1; // ETHER_HDX or ETHER_FDX 
  UINT32   T4100       : 1; // 100 Base T4 capable
  UINT32   TX100FDX    : 1; // 100BaseTX full duplex capable
  UINT32   TX100HDX    : 1; // 100BaseTX half duplex capable
  UINT32   T10FDX      : 1; // 10BaseT full duplex capable
  UINT32   T10HDX      : 1; // 10BaseT half duplex capable
  UINT32   autoConfig  : 1; // Autonegotiation capability 
  UINT32   MII         : 1; // TRUE if MII present
  UINT32   monitor     : 1; // Monitor capability

  UINT32   linkStatus  : 1; // ETHER_LINK_GOOD or ETHER_LINK_BAD
  UINT32   rcvClock    : 1; // ETHER_CLOCK_OK or ETHER_LOST_CLOCK 
  UINT32   xmtClock    : 1; // ETHER_CLOCK_OK or ETHER_LOST_CLOCK 
  UINT32   hwError     : 1; // ETHER_HW_OK or ETHER_HW_ERROR
  UINT32   phyComm     : 1; // ETHER_HW_OK or ETHER_HW_ERROR
  UINT32   phyTX       : 1; // GBIT ONLY -- PHY TX LED
  UINT32   phyRX       : 1; // GBIT ONLY -- PHY RX LED
  UINT32   phyfdx      : 1; // GBIT ONLY -- PHY duplex LED
  UINT32   gbicLOS     : 1; // GBIT ONLY -- GBIC loss of signal
  UINT32   gbicTXF     : 1; // GBIT ONLY -- GBIC TX Fault
  UINT32   gbicMode    : 3; // GBIT ONLY -- GBIC mode pins
  UINT32   gbicSubT    : 2; // GBIT GBICbaseT 1=Finisar, 2=Molex
  UINT32   disabled    : 1;
} BufFmtEthLinkStatusBits ;


enum BufFmtLANAutoNegType
{
  BUFFMT_LAN_AUTO_NEG_ONGOING = 0,
  BUFFMT_LAN_AUTO_NEG_FINISHED,
  BUFFMT_LAN_AUTO_NEG_NOTUSED,
  BUFFMT_LAN_AUTO_NEG_FAILED,
};

// GBIC / SFPs Lan Speeds
enum BufFmtLANSpeed
{
  BUFFMT_LAN_SPEED_UNKN  = 0,
  BUFFMT_LAN_SPEED_10MB,
  BUFFMT_LAN_SPEED_100MB,
  BUFFMT_LAN_SPEED_1000MB,
  BUFFMT_LAN_SPEED_10000MB,
  BUFFMT_LAN_SPEED_ERROR = 0xf
};

#ifdef __cplusplus
inline bool UnknownLANSpeed(BufFmtLANSpeed LANSpeed)
{
   return ((LANSpeed == BUFFMT_LAN_SPEED_UNKN) ||
           (LANSpeed == BUFFMT_LAN_SPEED_ERROR));
}

inline bool KnownLANSpeed(BufFmtLANSpeed LANSpeed)
{
   return !UnknownLANSpeed(LANSpeed);
}
#endif

// GBIC / SFPs HW types 
enum BuffFmtLANGBicTypes
{
  BUFFMT_LAN_GBIC_UNKNOWN = 0,
  BUFFMT_LAN_GBIC_NOT_PRESENT,
  BUFFMT_LAN_GBIC_SX,
  BUFFMT_LAN_GBIC_LX,    
  BUFFMT_LAN_GBIC_TX,
  BUFFMT_LAN_SFPP_DA,
  BUFFMT_LAN_SFPP_SR,
  BUFFMT_LAN_SFPP_LR,
  BUFFMT_LAN_SFPP_LRM,
  BUFFMT_LAN_SFPP_0C3,
  BUFFMT_LAN_SFPP_0C12,
  BUFFMT_LAN_SFPP_0C48,
  BUFFMT_LAN_SFPP_0C192,
  BUFFMT_LAN_GBIC_UNSUPPORTED,
  BUFFMT_LAN_GBIC_ERROR = 0xf
};

#ifdef __cplusplus
inline bool UnknownBuffFmtGBIC(BuffFmtLANGBicTypes GBIC)
{
   return ((GBIC == BUFFMT_LAN_GBIC_UNKNOWN) ||
           (GBIC == BUFFMT_LAN_GBIC_NOT_PRESENT) ||
           (GBIC == BUFFMT_LAN_GBIC_UNSUPPORTED) ||
           (GBIC == BUFFMT_LAN_GBIC_ERROR));
}

inline bool KnownBuffFmtGBIC(BuffFmtLANGBicTypes GBIC)
{
   return !UnknownBuffFmtGBIC(GBIC);
}

inline bool OneGigEBuffFmtGBIC(UINT32 GBIC)
{
   return ((GBIC == BUFFMT_LAN_GBIC_SX) ||
           (GBIC == BUFFMT_LAN_GBIC_LX) ||
           (GBIC == BUFFMT_LAN_GBIC_TX));
}

inline bool OneGigECopperBuffFmtGBIC(UINT32 GBIC)
{
   return (GBIC == BUFFMT_LAN_GBIC_TX);
}

inline bool OneGigEOpticalBuffFmtGBIC(UINT32 GBIC)
{
   return ((GBIC == BUFFMT_LAN_GBIC_SX) ||
           (GBIC == BUFFMT_LAN_GBIC_LX));
}

inline bool TenGigEBuffFmtGBIC(UINT32 GBIC)
{
   return ((GBIC == BUFFMT_LAN_SFPP_DA) ||
           (GBIC == BUFFMT_LAN_SFPP_SR) ||
           (GBIC == BUFFMT_LAN_SFPP_LR) ||
           (GBIC == BUFFMT_LAN_SFPP_LRM));
}

inline bool OpticalSonetBuffFmtGBIC(UINT32 GBIC)
{
   return ((GBIC == BUFFMT_LAN_SFPP_0C3) ||
           (GBIC == BUFFMT_LAN_SFPP_0C12) ||
           (GBIC == BUFFMT_LAN_SFPP_0C48) ||
           (GBIC == BUFFMT_LAN_SFPP_0C192));
}
#endif

enum BufFmtLANDuplexType
{
  BUFFMT_LAN_DUPLEX_HALF = 0,
  BUFFMT_LAN_DUPLEX_FULL,
  BUFFMT_LAN_DUPLEX_UNKNOWN,
  BUFFMT_LAN_DUPLEX_ERROR,
};

// Multiport 10/100/1000 LIM, also used for Packet Portal.
typedef struct
{
  UINT32   linkStatus  : 2;  // see BufFmtLineStatusType: BUFFMT_LS_UNKN/OFF/TOGL/ON
  UINT32   autoNeg     : 2;  // 0=ongoing 1=finished 2=notused 3=failed
  UINT32   speed       : 4;  // see BufFmtLANSpeed
  UINT32   gbicType    : 4;  // see BuffFmtLANGBicTypes
  UINT32   duplex      : 1;  // 0=HDX 1=FDX
  UINT32   amplitude   : 16; // power meter reading (X10 value INT16)
  UINT32   ampNotKnown : 1;  // 0=power reading is valid, 1=power is unknown,n/a or not implemented.
  UINT32   unused      : 1;
  UINT32   disabled    : 1;
} BufFmtMPGigLinkStatusBits ;

typedef union
{
  BufFmtMPGigLinkStatusBits bits;
  UINT32                    value;
} BufFmtMPGigLinkStatusBitsUnion;

// 10/100/1000/10000 LIM @TH@
typedef struct
{
  UINT32   linkStatus  : 2;  // see BufFmtLineStatusType: BUFFMT_LS_UNKN/OFF/TOGL/ON
  UINT32   autoNeg     : 2;  // 0=ongoing 1=finished 2=notused 3=failed
  UINT32   speed       : 4;  // see BufFmtLANSpeed
  UINT32   gbicType    : 4;  // see BuffFmtLANGBicTypes
  UINT32   duplex      : 1;  // 0=HDX 1=FDX
  UINT32   amplitude   : 16; // power meter reading (X10 value INT16)
  UINT32   unused      : 2;
  UINT32   disabled    : 1;
} BufFmtTenGigLinkStatusBits ;

typedef union
{
  BufFmtTenGigLinkStatusBits bits;
  UINT32                    value;
} BufFmtTenGigLinkStatusBitsUnion;


// ****************************************************************************
//                             TYPES
// ****************************************************************************
// Structure for frames or cells.  Note that this structure has the same size
// and alignment as BufFmtEventHeader, and that the recordType field is
// in the same place.  There is usually a block of data bytes associated with
// this header, and the length of that block is in .storedLength.
typedef struct
{
    UINT8   recordType;     // record type (frame data only)
    UINT8   limSide;        // which side of the line-under-test this is from,
                            // or what channel of a T1 line, etc.
    UINT8   softwareBits;   // status bits created by BufferDriver software
    UINT8   hardwareBits;   // status bits set by filter hardware: start, stop, center, etc.
    UINT32  frameNumber;    // counts frame and event records since start of run
    UINT32  timeSecs;       // high bits of timestamp: seconds since 1970
    UINT32  timeNSecs;      // low bits of time stamp: 0 to 999999999 nanoseconds
    UINT32  storedLength;   // number of bytes actually in the buffer            
    UINT32  rcvLength;      // number of bytes in frame (may be more than stored)
    UINT32  statusBits;     // value set by the LIM hardware; varies from LIM to LIM
    UINT32  filtersMatched; // bits indicating which filters this record matched
    UINT32  encapsulation;  // bits set by the filter program to give protocol ID info
    UINT8   limInfoLength;  // number of bytes used in limInfo (right aligned)
    UINT8   limInfo[BUFFMT_LIMINFO_MAX_LENGTH]; // extra info bytes generated by LIM
} BufFmtFrameHeader;                            // (called header data in LIM doc)

// Structure for line status events, time events, and any other non-frame data records
// which we dream up.  Note that this structure has the same size
// and alignment as BufFmtFrameHeader, and that the recordType, limSide, and
// softwareBits fields are in the same place.  The event records consist of a
// header only with no data, so we don't have a data length field.  All known
// events fit into this structure. See LIM hardware and LIM Driver software documentation
// for the meanings of the eventInfo fields.  For other event
// types see the documentation on the software which created them.
typedef struct
{
    UINT8   recordType;     // record type (line status event, timestamp, etc)
    UINT8   limSide;        // which side of the line-under-test this is from, or N/A
    UINT8   softwareBits;   // status bits created by BufferDriver software
    UINT8   reserved;
    UINT32  eventNumber;    // counts frame and event records since start of run
    UINT32  timeSecs;       // high bits of timestamp: seconds since 1970
    UINT32  timeNSecs;      // low bits of time stamp: 0 to 999,999,999 nanoseconds
    UINT32  eventInfo1;     // Extra event info will fit
    UINT32  eventInfo2;     // in these 5 fields.
    UINT32  eventInfo3;
    UINT32  eventInfo4;
    UINT32  eventInfo5;
    UINT32  eventInfo6;
} BufFmtEventHeader;

///////////////////////////////////////////////////////////////////////////////
// For recordType=BUFFMT_TYPE_LINE_STATUS (Line Status Events)
//      limSide = BUFFMT_SIDE_EVENT
//      eventInfo1 is BUFFMT_SIDE_LINE line status word
//      eventInfo2 is BUFFMT_SIDE_LINE line status ( (lineStatus counter << 16) | signalAmplitude)
//      eventInfo3 is BUFFMT_SIDE_EQPT line status word
//      eventInfo4 is BUFFMT_SIDE_EQPT line status ( (lineStatus counter << 16) | signalAmplitude)
//      eventInfo5 is BUFFMT_SIDE_LINE frequency
//      eventInfo6 is BUFFMT_SIDE_EQPT frequency
//  Each set of line status bits is accompanied by a 16-bit lineStatus counter,
//  which is the sum of the other bits, and by signalAmplitude, which is a 16-bit signed 
//  integer, representing mV or dBm, scaled by 10. 

// Structure for buffer record headers.  Use this typedef for headers within Keystone.
typedef union
{
    BufFmtFrameHeader   frame;
    BufFmtEventHeader   event;
} BufFmtHeader;

#endif   /* __BUFFERFORMAT_H */
