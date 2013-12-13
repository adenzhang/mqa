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

    mqa::StatsIpAddr ip;  // ip address
    int              port;
    bool operator==(const IpPort& a)const {
        return ip == a.ip && port == a.port;
    }
};
typedef boost::shared_ptr<IpPort> IpPortPtr;

// the one with smaller ip is src.
// return 0:src to dest; 1: dest to src
inline int FlowDirection(const IpPort& a, const IpPort& b){
    return a.ip < b.ip ? 0: 1;
}

typedef boost::shared_ptr<mqa::VQStatsConnVlan> VQStatsConnVlanPtr;
struct IpFlowPair {
    IpFlowPair(){}
    IpFlowPair(UINT8 proto, const IpPort& a, const IpPort& b, bool autoDirection=true)
        : proto(proto)
        , src((autoDirection&&FlowDirection(a,b))?b:a)
        , dest((autoDirection&&FlowDirection(a,b))?a:b)
    {}
    IpFlowPair(UINT8 proto, const IpPort& a, const IpPort& b, boost::shared_ptr<IpFlowPair>& aTunnel)
        : proto(proto)
        , src(FlowDirection(a,b)?b:a)
        , dest(FlowDirection(a,b)?a:b)
        , tunnel(aTunnel?(new IpFlowPair(aTunnel->proto, FlowDirection(a,b)?aTunnel->src:aTunnel->dest, FlowDirection(a,b)?aTunnel->dest:aTunnel->src, false))
                        :NULL)
    {}
    IpFlowPair(UINT8 proto, const IpPort& a, const IpPort& b, boost::shared_ptr<mqa::VQStatsConnVlan> VLan)
        : proto(proto)
        , src(FlowDirection(a,b)?b:a)
        , dest(FlowDirection(a,b)?a:b)
        , vlan(VLan)
    {}

    IpPort src;                          // the one with smaller ip is defined as source.
    IpPort dest;
    UINT8  proto;

    boost::shared_ptr<IpFlowPair> tunnel; // optional, note that tunnel is not of auto-direction.
    VQStatsConnVlanPtr            vlan;   // optional

    bool operator==(const IpFlowPair& a) const {
        int bTunnel = tunnel?1:0, aTunnel = a.tunnel? 1:0;
        int bVlan = vlan? 1:0, aVlan=a.vlan?1:0;
        if( bTunnel^aTunnel || bVlan^aVlan ) return false;
        if( bTunnel && *tunnel != *a.tunnel ) return false;
        if( bVlan && *vlan != *a.vlan ) return false;

        return src == a.src && dest == a.dest;
    }
    bool operator!=(const IpFlowPair& a) const {
        return !operator==(a);
    }
    int compare(const IpFlowPair& a) const { // only compare the upper level
        if( 0 == FlowDirection(src, a.src) ) return -1;
        else if( 0 == FlowDirection(a.src, src) ) return 1;
        // otherwise, src ip equals
        
        if( src.port < a.src.port ) return -1;
        if( src.port > a.src.port ) return 1;
        // otherwise, src port equals

        if( 0 == FlowDirection(dest, a.dest) ) return -1;
        else if( 0 == FlowDirection(a.dest, dest) ) return 1;
        // otherwise, dest ip equals

        if( dest.port < a.dest.port ) return -1;
        if( dest.port > a.dest.port ) return 1;
        // otherwise, dest port equals

        return 0;
    }
    bool operator<(const IpFlowPair& a) const {
        return compare(a)<0;
    }
    size_t hash() const {
        size_t h = src.ip.Hash() ^ src.port ^ dest.ip.Hash() ^ dest.port;
        if( tunnel ) h ^= tunnel->hash();
        if( vlan ) h ^= vlan->operator uintptr_t();
        return h;
    }

    std::string to_string()const{
        const IpFlowPair& flow = *this;
        
        std::stringstream  ss;
        //if(!src.ip.IsIpv4()) ss << "[IPv6]";
        ss << (int)flow.proto << ", " << flow.src.ip << ":" << flow.src.port 
           << " - " << flow.dest.ip << ":" << flow.dest.port;
        if(flow.tunnel){
            ss  <<", [" << (int)flow.proto << ", "<< flow.tunnel->src.ip << ":" << flow.tunnel->src.port
                <<" - " << flow.tunnel->dest.ip << ":" << flow.tunnel->dest.port << "(" << (int)flow.proto << ")"<< "]";
        }
        if( flow.vlan ) {
            ss << ", vlan:" << flow.vlan->aVLANMPLSIds[0];
        }
        
        return ss.str();
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
    // for HashMap
    static unsigned long Hash(const IpFlowPair& a){
        return a.hash();
    }
    // for HashMap
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
            nPackets[i] = 0;
        }
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
    ThreadPoolPtr         thread;

    UINT32                nPackets[2];

    RtpFlowResultQ        saved[2];

    void save(int nDir) {
        saved[nDir].push_back(res[nDir]);
        new((RtpFlowResult*)&res[nDir]) RtpFlowResult;
    }
    void IncPackets(int i){
        nPackets[i]++;
    }
};
typedef boost::shared_ptr<FlowStreamPair> FlowStreamPairPtr;
//typedef boost::unordered_map<IpFlowPair, FlowStreamPairPtr, FlowPairHash> FlowMap;
typedef ftl::Map<IpFlowPair, FlowStreamPairPtr, FlowPairHash> FlowMap;

typedef std::map<IpFlowPair, FlowStreamPairPtr> FlowSortedMap;
template< typename FromMap, typename ToMap > 
void CopyMap(FromMap& map, ToMap& tomap)
{
    for(FlowMap::iterator it=map.begin(); it!= map.end(); ++it) {
        tomap[it->first] = it->second;
    }
}

typedef std::list<mqa::StatsFrameParser*> FrameParserList;

bool LoadPcap(const std::string& sTrafficFile, FrameParserList& FrameList);


#endif //MQA_STREAMMANAGE_H_
