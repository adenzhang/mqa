#ifndef _NA_BASETYPES_H
#define _NA_BASETYPES_H
/////////////////////////////////////////////////////////////////////////////
// 
// File: NABaseTypes.h 
// Description: Header file for Network Analyzer basic types
// NOTE: This file was called BaseTypes.h in Analyzer, but to avoid
//       conflict with file of same name in SART, the name was changed.
//
// $Log: NABaseTypes.h $
// Revision 1 2006/07/21 04:40:32 +0800 ive55253 /AnalyzerCommon/5.20_Release/NA_Multiuser2
// Moved initial basetypes.h from Analyzer for sharing.
// 
// Revision 3 2005/09/01 10:34:26 -0600 ah0108 /Analyzer/Northstar/DotNet
// All typesdefs need to be explicitly signed or unsigned
// 
// Revision 2 2002/06/03 10:52:41 -0600 bv4736 deleted(id=1151)
// 1.2 Migration
// 
// 2     4/18/01 10:43p Sb4450
// Changed singed types to not use "signed" keyword so that the typedefs
// will not clash with babble typedefs.
// 
// 1     4/13/01 1:53p Sb4450
/////////////////////////////////////////////////////////////////////////////

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;

// NOTE: signed keyword required for .NET compile now
typedef   signed char  INT8;
typedef   signed short INT16;
typedef   signed int   INT32;

#ifdef WIN32
typedef unsigned __int64 UINT64;
typedef   signed __int64 INT64;
#endif

#endif //_NA_BASETYPES_H