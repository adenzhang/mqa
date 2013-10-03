// mqatest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <list>
#include "LogUtility.h"
#include "PcapReader.h"
#include "StatsFrameParser.h"
#include "mqa_flowkeys.h"
#include "MQmonIf.h"
#include "timesec.h"

#include <boost/unordered_map.hpp>

using namespace std;

typedef mqa::StatsFrameParser FrameParser;
typedef std::list<mqa::StatsFrameParser*> FrameParserList;

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
        FrameParser* pFrameInfo = new FrameParser();
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

    cout << "Frames in " << sTrafficFile << ": " << FrameList.size() << endl;

    return true;
}
struct IpPort {
    IpPort(){}
    IpPort(bool isIPV4, const UINT8 *addr, int port)
        :ip(isIPV4, addr), port(port) {}

    mqa::StatsIpAddr ip;
    int              port;
    bool operator==(const IpPort& a)const {
        return ip == a.ip && port == a.port;
    }
};
// the one with smaller ip is src.
// return 0:src to dest; 1: dest to src
static int FlowDirection(const IpPort& a, const IpPort& b){
    return a.ip < b.ip ? 0: 1;
}

struct IpFlowPair {
    IpFlowPair(const IpPort& a, const IpPort& b){
        int dir = FlowDirection(a,b);
        src = dir ==0? a:b;
        dest = dir ==0? b:a;
    }

    IpPort src; // the one with smaller ip is defined as source.
    IpPort dest;
    bool operator==(const IpFlowPair& a) const {
        return src == a.src && dest == a.dest || src == a.dest && dest == a.src;
    }
};
struct FlowPairHash{
    size_t operator()(const IpFlowPair& a) const{
        return a.src.ip.Hash() ^ a.src.port ^ a.dest.ip.Hash() ^ a.dest.port;
    }
};
// one interface and one stream per flow
struct FlowStreamPair {
    FlowStreamPair(){
        for(int i=0;i<2;++i) {
            itf[i] = NULL;
            strm[i] = NULL;
        }
    }
    mqa::CMQmonInterface* itf[2];  // 0: src to dest; 1: dest to src;
    mqa::CMQmonStream*    strm[2];
    mqa::CMQmonMetrics    res[2];   // result
    mqa::CMQmonMetrics    sumRes[2];  // sum of result
    char                  strIP[2][32];
};
typedef boost::shared_ptr<FlowStreamPair> FlowStreamPairPtr;
typedef boost::unordered_map<IpFlowPair, FlowStreamPairPtr, FlowPairHash> FlowMap;

void run()
{
    const string pcapfn = "call-by-cf-001-whole.pcap";
    FlowMap     flows;
    mqa::MQmon *mon;
    FrameParserList  packetList;

    VqtSetLogLevel(VQT_LOG_DEBUG);
    if(!LoadPcap(pcapfn, packetList))
        return;

    mon = mqa::MQmon::Instance();

    int k = 0;
    for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
        k++;
        mqa::StatsFrameParser *parser = *it;

        IpPort src(parser->IsIpv4(), parser->SrcIp(), parser->IpTransInfo().TransInfo.nSrcPort);
        IpPort dest(parser->IsIpv4(), parser->DestIp(), parser->IpTransInfo().TransInfo.nDestPort);
        int nDir = FlowDirection(src, dest);
        IpFlowPair aFlow(src,dest);

        FlowMap::iterator     itFlow = flows.find(aFlow);
        mqa::CMQmonInterface *monif=NULL; 
        mqa::CMQmonStream    *pStream = NULL;
        FlowStreamPairPtr     strmPair;
        UINT32                rtpLen;
        const UINT8          *pRtp = parser->GetTransPayload(rtpLen);
        if( itFlow == flows.end() ) {  // new flow
            monif = mon->CreateInterface();
            monif->IndicateRtpPacket((uintptr_t)monif, pRtp, rtpLen, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000, &pStream);

            strmPair.reset(new FlowStreamPair);
            strmPair->itf[nDir] = monif;
            strmPair->strm[nDir] = pStream;
            stringstream  ss, ss1;
            ss << src.ip;
            ss1 << dest.ip;
            strncpy(strmPair->strIP[0], ss.str().c_str(),32);
            strncpy(strmPair->strIP[1], ss1.str().c_str(),32);

            flows.insert(std::make_pair(aFlow, strmPair));
        }else{  // old flow pair
            FlowStreamPairPtr& p = itFlow->second;
            if( p->strm[nDir] ) {
                pStream = p->strm[nDir];
                pStream->IndicateRtpPacket(pRtp, rtpLen, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000);
            }else{
                if(!p->itf[nDir]){  // if first feed in this direction
                    assert(p->itf[nDir?0:1]);  // the other direction should've interface.
                    p->itf[nDir] = mon->CreateInterface();
                    assert(p->strm[nDir] == NULL);  //only interface can detect new stream.
                }
                monif = p->itf[nDir];
                monif->IndicateRtpPacket((uintptr_t)monif, pRtp, rtpLen, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000, &pStream);
                if(pStream)
                    p->strm[nDir] = pStream;
            }
            if( pStream && pStream->IsValidStream() ) {
                if( pStream->GetMetrics(p->res[nDir]) ){
                    p->sumRes[nDir].MOS += p->res[nDir].MOS;
                    p->sumRes[nDir].Jitter += p->res[nDir].Jitter;
                    p->sumRes[nDir].fLossRate = p->res[nDir].fLossRate;
                    p->sumRes[nDir].nPackets = p->res[nDir].nPackets;

                    // output
                    VqtDebug("%s - %s %d: MOS=%3.1f Jitter=%dms, lossRate=%d%%\n, TotalPackets=%d\n"
                        , p->strIP[0], p->strIP[1], nDir, p->res[nDir].MOS, (int) p->res[nDir].Jitter
                        , (int) p->res[nDir].fLossRate*100, p->res[nDir].nPackets);
                    if( k%20 == 0 ) {
                        int npkt = p->res[nDir].nPackets;
                        VqtDebug("%s - %s %d [Avg]: MOS=%3.1f Jitter=%dms, lossRate=%d%%\n, TotalPackets=%d\n"
                            , p->strIP[0], p->strIP[1], nDir, p->sumRes[nDir].MOS/npkt, (int) p->res[nDir].Jitter/npkt
                            , (int) p->res[nDir].fLossRate*100, p->res[nDir].nPackets);
                    }
                }
            }  // if pStream
        }  // if old flow pair
    }

    for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
        delete *it;
    }
    packetList.clear();
}

int _tmain(int argc, _TCHAR* argv[])
{
    VqtSetLogLevel(VQT_LOG_DEBUG);
    run();
	return 0;
}

