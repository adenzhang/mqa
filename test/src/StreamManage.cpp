#include "StreamManage.h"

#include "LogUtility.h"
#include "mqa/mqa_flowkeys.h"

using namespace mqa;

VQStatsConnVlan CreateConnVlan(const StatsFrameParser& Parser)
{
    return VQStatsConnVlan(
        Parser.nVLAN + Parser.nMPLS,
        Parser.aVLANMPLSIds,
        Parser.nLimPort
        );
}
VQStatsConnIp CreateConnIp(const StatsFrameParser& Parser)
{
    return VQStatsConnIp(
        Parser.IsIpv4(),
        Parser.SrcIp(),
        Parser.DestIp()
        );
}
VQStatsConnPort CreateConnPort(const StatsFrameParser& Parser, bool* pSrcToDest)
{
    bool bSrcToDest;
    if (!pSrcToDest)
        bSrcToDest = *pSrcToDest;
    else
    {
        StatsIpAddr IpSrc(Parser.IsIpv4(), Parser.SrcIp());
        StatsIpAddr IpDest(Parser.IsIpv4(), Parser.DestIp());
        bSrcToDest = !(IpSrc > IpDest);
    }
    if (bSrcToDest)
        return VQStatsConnPort(
        Parser.LowerInfo.TransInfo.nSrcPort,
        Parser.LowerInfo.TransInfo.nDestPort,
        Parser.LowerInfo.TransInfo.nTransProto
        );
    else
        return VQStatsConnPort(
        Parser.LowerInfo.TransInfo.nDestPort,
        Parser.LowerInfo.TransInfo.nSrcPort,
        Parser.LowerInfo.TransInfo.nTransProto
        );
}
//----------- tunneling key ----------------------------------

VQStatsTunnelLowerAddress CreateTunnelLowerAddress(const StatsFrameParser& Parser)
{
    return VQStatsTunnelLowerAddress(
        Parser.IsIpv4(false),
        Parser.SrcIp(false),
        Parser.DestIp(false),
        Parser.nTEI,
        Parser.nVLAN + Parser.nMPLS,
        Parser.aVLANMPLSIds,
        Parser.nLimPort
        );
}
VQStatsTunnelUpperAddress CreateTunnelUpperAddress(const StatsFrameParser& Parser)
{
    return VQStatsTunnelUpperAddress(
        Parser.IsIpv4(true),
        Parser.SrcIp(true),
        Parser.DestIp(true),
        Parser.UpperInfo.TransInfo.nSrcPort,
        Parser.UpperInfo.TransInfo.nDestPort,
        Parser.UpperInfo.TransInfo.nTransProto
        );
}




bool LoadPcap(const std::string& sTrafficFile, FrameParserList& FrameList)
{
    CPcapReader PcapReader;

    if (!PcapReader.Open(sTrafficFile))
        return false;

    PcapFileHeader& PcapHeader = PcapReader.PcapHeader();
    for (int i=0;; ++i)
    {
        PcapPktHeader* pPktHeader;
        uint8_t* pPkt;

        if (!PcapReader.ReadNext(pPkt, pPktHeader))
            break;

        // Allocate frame info and parse each packet
        mqa::StatsFrameParser* pFrameInfo = new mqa::StatsFrameParser(128);  // RTP header size
        if (!pFrameInfo)
            return false;
        if(!pFrameInfo->ParseFrame(pPkt, pPktHeader->incl_len, 0))
        {
            delete pFrameInfo;
            continue;
        }
        pFrameInfo->nTimestamp.tv_sec = pPktHeader->ts_sec;
        pFrameInfo->nTimestamp.tv_usec = pPktHeader->ts_usec;

        // Insert into queue
        FrameList.push_back(pFrameInfo);
    }

    //VqtInfo("Frames Parsed in %s : %d\n", sTrafficFile.c_str(), FrameList.size() );

    return true;
}
