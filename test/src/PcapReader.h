#pragma once

#ifndef _PCAP_READER_H_
#define _PCAP_READER_H_

#include <stdio.h>
#include <string>
#include "stdint.h"
#include <boost/thread.hpp>

typedef struct _Pcap_File_Hdr
{
    uint32_t uMagicNumber;        //Magic number (0xa1b2c3d4)
    uint16_t uVerMajor;           //Major version number
    uint16_t uVerMinor;           //Minor version number
    uint32_t uThisZone;           //GMT to local correction
    uint32_t uSigfigs;            //Accuracy of timestamps
    uint32_t uSnaplen;            //Max length of captured packets (byte)
    uint32_t uNetwork;            //Data link type  
}PcapFileHeader;

typedef struct _Pcap_Pkt_Hdr {
    uint32_t    ts_sec;             /* timestamp seconds */                         //This corresponds to timeSecs in RTSM header
    uint32_t	ts_usec;            /* timestamp microseconds */                    //This corresponds to timeNSecs/1000 in RTSM header 
    uint32_t	incl_len;           /* number of octets of packet saved in file */  //This corresponds to storedLength in RTSM DATA header only
    uint32_t	orig_len;           /* actual length of packet */                   //This corresponds to rcvLength in RTSM DATA header only
}PcapPktHeader;

class CFileReader;
class CPcapReader
{
public:
    CPcapReader();
    ~CPcapReader();

    bool Open(std::string sFilePath);
    void Close();

    inline PcapFileHeader& PcapHeader(void) { return m_PcapHeader; }

    bool ReadNextPktHeader(PcapPktHeader*& pPktHeader);
    bool ReadNextPkt(uint8_t*& pPkt, uint32_t nPktLength);
    bool ReadNext(uint8_t*& pPkt, PcapPktHeader*& pPktHeader);

private:
    CFileReader* m_pFileReader;
    PcapFileHeader m_PcapHeader;
    bool m_bIsBe;
};

#endif //_PCAP_READER_H_