#ifndef _SARSHRDEFN_H
#define _SARSHRDEFN_H
///////////////////////////////////////////////////////////////////////////////
//
// File       : SarSharedDefs.h
// Description: NA SAR Shared definitions. 
//
//              This is used by SarDriver, RTSDMeas and other system components
//              on the Xscale VxWorks system. It is also used on the
//              SART MU Server system for parsing information that is
//              sent on the RTSD data stream. For these reasons, these
//              "shared definitions" are in this separate H file so that
//              they are easily shared accross these and other apps.
//
//              NOTE: These were in SarDriver.h but were moved here.
//               
//              This is also used by the Elwood DLL system on the Network Analyzer
//              MU Client to decode the hardware counters and may in the future
//              be used by enhanced SART clients.
//
// Copyright(c) 2006 by Agilent Technologies, Inc.
//
// $Log: SarSharedDefs.h $
// Revision 1 2006/03/23 05:23:13 +0800 ive55253 deleted(id=11266)
// First pass breakout sharing of common defs between SART and NA.
// 
// Revision 1 2006/02/06 16:24:48 -0700 bi4723 /Platform/Nova/NA_Multiuser
// Moved shared definitions out of driver file so that SART MU server and NA MU
// client could use just the definitions. This shared definition files will be
// moved to another snapshot db that will be shared between SART and NA.
// 
//
///////////////////////////////////////////////////////////////////////////////

// Structure used to hold general configuration
typedef struct
{
    UINT8           NNImode;
    UINT8           discoveryEnable;
    UINT8           fixHECErrors;
    UINT8           spare1;
} SARDrvGenConfig;


// AAL re-assembly options
#define SARDRV_AAL_1                 (1)  // re-assemble AAL-1
#define SARDRV_AAL_2                 (2)  // re-assemble AAL-2
#define SARDRV_AAL_3_4               (3)  // re-assemble AAL-3/4
#define SARDRV_AAL_5                 (4)  // re-assemble AAL-5
#define SARDRV_AAL_AUTO              (5)  // discover AAL and re-assemble
#define SARDRV_AAL_CELL              (6)  // cellMode, don't re-assemble
#define SARDRV_AAL_NO_SAR           (10)  // discover AAL but don't re-assemble


// Structure used to hold a VC's definition
typedef struct
{
    UINT16          index;
    UINT16          vpi;
    UINT16          vci;
    UINT8           discovered;
    UINT8           aal;        // Limited to 6 bits (<=31) by SarDriver code.
                                //                defns are SARDRV_AAL_xx
    UINT16          id;         // Holds port number for 8-Port T1/E1 Lim
                                // or Tributary ID for 4-Port OC3 Lim
                                // = 0 for all other lims (limited to 9bits)
    UINT16          pad1;       // Make it a multiple of 32 bits.
} SARDrvVcDefinition;

// The zero sized array causes problems on the MS VC++ compilier...and
//  rather than risk changing this due to being shared with driver, we
//  just disable the warning here.
#ifdef WIN32 
#pragma warning( push )
#pragma warning( disable : 4200 ) 
#endif

// Structure used to hold a set of a VC definitions
typedef struct
{
    UINT16          count;
    UINT16          pad1;
    UINT32          pad2;
    SARDrvVcDefinition  defs[0];
} SARDrvVcDefinitionSet;

#ifdef WIN32 
// restore warning setup
#pragma warning( pop )
#endif


#endif
