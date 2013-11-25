#ifndef MQA_STREAMMANAGE_H_
#define MQA_STREAMMANAGE_H_

#include "mqa/StatsFrameParser.h"
#include "mqa/mqa_flowkeys.h"
#include "mqa/MQmonIf.h"
#include "mqa/ThreadPool.h"
#include "PcapReader.h"

#include <list>
#include <deque>
#include <string>
#include <sstream>

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
typedef boost::shared_ptr<IpPort> IpPortPtr;

// the one with smaller ip is src.
// return 0:src to dest; 1: dest to src
static int FlowDirection(const IpPort& a, const IpPort& b){
    return a.ip < b.ip ? 0: 1;
}

struct IpFlowPair {
    IpFlowPair(){}
    IpFlowPair(const IpPort& a, const IpPort& b){
        int dir = FlowDirection(a,b);
        src = dir ==0? a:b;
        dest = dir ==0? b:a;
    }
    IpFlowPair(const IpPort& a, const IpPort& b, boost::shared_ptr<IpFlowPair> tunnelPair)
        : tunnel(tunnelPair)
    {
        int dir = FlowDirection(a,b);
        src = dir ==0? a:b;
        dest = dir ==0? b:a;
    }

    IpPort src; // the one with smaller ip is defined as source.
    IpPort dest;

    boost::shared_ptr<IpFlowPair> tunnel;
    //VQStatsConnVlan vlan;

    bool operator==(const IpFlowPair& a) const {
        if(tunnel) return a.tunnel && *a.tunnel == *tunnel && src == a.src && dest == a.dest;
        else return !a.tunnel && src == a.src && dest == a.dest;
    }
    size_t hash() const {
        size_t h = src.ip.Hash() ^ src.port ^ dest.ip.Hash() ^ dest.port;
        if( tunnel ) h ^= tunnel->hash();
        return h;
    }
};
typedef boost::shared_ptr<IpFlowPair> IpFlowPairPtr;

struct FlowPairHash
    //: public ftl::ElementTraitsBase<IpFlowPair>
{
    // for boost::unordered_map
    size_t operator()(const IpFlowPair& a) const{
        return a.hash();
    }
    // for ftl::Map
    static unsigned long Hash(const IpFlowPair& a){
        return a.hash();
    }
    // for ftl::Map
    static bool CompareElements(const IpFlowPair& a, const IpFlowPair& b) {
        return a == b;
    }
};
typedef boost::shared_ptr<ThreadPool> ThreadPoolPtr;

struct RtpFlowResult {
    enum STREAM_STATE{S_INITED, S_VALID, S_INVALID};

    mqa::CMQmonMetrics    res;   // result
    mqa::CMQmonMetrics    sumRes, minRes, maxRes;  // sum of result
    int                   nCalulation;  // number of calculations
    size_t                startID, currID;
    STREAM_STATE          currState;

    RtpFlowResult(): nCalulation(0), startID(0), currID(0), currState(S_INVALID)
    {}
};
typedef std::deque<RtpFlowResult> RtpFlowResultQ;

// one interface and one stream per flow
struct FlowStreamPair
{
    FlowStreamPair(IpFlowPair& aflow, ThreadPoolPtr th=ThreadPoolPtr())
        : flow(aflow)
        , thread(th)
        //, thread( new ThreadPool(1,MAX_TASKS_PER_THREAD))
    {
        for(int i=0;i<2;++i) {
            itf[i] = NULL;
            strm[i] = NULL;
        }

        bAddrConverted = false;
        addr2Str();
    }
    RtpFlowResult         res[2];
    //mqa::CMQmonMetrics    res[2];   // result
    //mqa::CMQmonMetrics    sumRes[2], minRes[2], maxRes[2];  // sum of result
    //int                   nCalulation[2];  // number of calculations
    //size_t                startID[2], currID[2];
    //STREAM_STATE          currState[2];

    mqa::CMQmonInterface* itf[2];  // 0: src to dest; 1: dest to src;
    mqa::CMQmonStream*    strm[2];
    IpFlowPair            flow;
    char                  strIP[2][128];
    bool                  bAddrConverted;
    ThreadPoolPtr         thread;

    RtpFlowResultQ        saved[2];

    void save(int nDir) {
        saved[nDir].push_back(res[nDir]);
        new((RtpFlowResult*)&res[nDir]) RtpFlowResult;
    }
    std::string addr2Str(){
        if(!bAddrConverted){
            std::stringstream  ss, ss1;
            if(flow.tunnel){
                ss  <<"[" << flow.tunnel->src.ip << ":" << flow.tunnel->src.port << "]";
                ss1 <<"[" << flow.tunnel->dest.ip << ":" << flow.tunnel->dest.port << "]";
            }
            ss << flow.src.ip << ":" << flow.src.port;
            ss1 << flow.dest.ip << ":" << flow.dest.port;
            strncpy(strIP[0], ss.str().c_str(),128);
            strncpy(strIP[1], ss1.str().c_str(),128);
            bAddrConverted = true;
        }
        std::string ret;
        ret += strIP[0];
        ret += " -> ";
        ret += strIP[1];
        return ret;
    }
};
typedef boost::shared_ptr<FlowStreamPair> FlowStreamPairPtr;
//typedef boost::unordered_map<IpFlowPair, FlowStreamPairPtr, FlowPairHash> FlowMap;
typedef ftl::Map<IpFlowPair, FlowStreamPairPtr, FlowPairHash> FlowMap;

typedef std::list<mqa::StatsFrameParser*> FrameParserList;

bool LoadPcap(const std::string& sTrafficFile, FrameParserList& FrameList);


#endif //MQA_STREAMMANAGE_H_
