#ifndef LINUXBASETSD_H
#define LINUXBASETSD_H

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed long long    INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned long long  UINT64, *PUINT64;

//
// The following types are guaranteed to be signed and 32 bits wide.
//

typedef signed int LONG32, *PLONG32;

//
// The following types are guaranteed to be unsigned and 32 bits wide.
//

typedef unsigned int ULONG32, *PULONG32;
typedef unsigned int DWORD32, *PDWORD32;

//--- other types -------------------

typedef long        LONG;
typedef long       *LONG_PTR;
typedef unsigned int BOOL;

#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))

#ifndef NULL
#define NULL 0
#endif

#ifndef __FUNCTION__
#define __FUNCTION__ ""
#endif

#endif // LINUXBASETSD_H
