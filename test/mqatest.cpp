// mqatest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MqaAssert.h"
#include <list>
#include "LogUtility.h"
#include "PcapReader.h"
#include "StatsFrameParser.h"
#include "mqa_flowkeys.h"
#include "MQmonIf.h"
#include "timesec.h"

#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>

#include "ThreadPool.h"

#include <ftl/FixedSizePool.h>

using namespace std;

typedef mqa::StatsFrameParser FrameParser;
typedef std::list<mqa::StatsFrameParser*> FrameParserList;

#define MULTI_THREAD

#define MAX_TASKS_PER_THREAD (1024*1024)
#define MAX_TASKS_IN_POOL (1024*1024*10)
#define THREAD_PER_STREAM 0

//#define MAX_TASKS_PER_THREAD 0
//#define MAX_TASKS_IN_POOL 0
//#define THREAD_PER_STREAM 0

const int MAX_TRY_ALLOC = 1000;  // pool alloc or post task to thread pool

#define SetMinMax(nSample, nMin, nMax) \
    if ((nSample < nMin)) nMin = nSample; \
    if (nSample > nMax) nMax = nSample;

long double GetTimeMicroSec()
{
    long double atime=0;
#ifdef WIN32
    static long long freq = 0;
    long long a;
    if( 0 == freq )
        QueryPerformanceFrequency((LARGE_INTEGER*) &freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&a);
    atime = (long double)a/(long double)freq * (long double)1.0E6;
#else
    timespec t;
    if( 0 == clock_gettime(CLOCK_MONOTONIC, &t) )
        atime = (long double)s.tv_sec * (long double)1.0E6 + (long double)s.tv_nsec / (long double)1.0E3;
#endif
    return atime;
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
    IpFlowPair(){}
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
typedef boost::shared_ptr<ThreadPool> ThreadPoolPtr;

// one interface and one stream per flow
struct FlowStreamPair {
    FlowStreamPair(IpFlowPair& aflow, ThreadPoolPtr th=ThreadPoolPtr())
        : flow(aflow)
        , thread(th)
        //, thread( new ThreadPool(1,MAX_TASKS_PER_THREAD))
    {
        for(int i=0;i<2;++i) {
            itf[i] = NULL;
            strm[i] = NULL;
            nCalulation[i] = 0;
        }
        bAddrConverted = false;
        addr2Str();
    }
    mqa::CMQmonInterface* itf[2];  // 0: src to dest; 1: dest to src;
    mqa::CMQmonStream*    strm[2];
    mqa::CMQmonMetrics    res[2];   // result
    mqa::CMQmonMetrics    sumRes[2], minRes[2], maxRes[2];  // sum of result
    int                   nCalulation[2];  // number of calculations
    IpFlowPair            flow;
    char                  strIP[2][32];
    bool                  bAddrConverted;
    ThreadPoolPtr         thread;

    string addr2Str(){
        if(!bAddrConverted){
            stringstream  ss, ss1;
            ss << flow.src.ip;
            ss1 << flow.dest.ip;
            strncpy(strIP[0], ss.str().c_str(),32);
            strncpy(strIP[1], ss1.str().c_str(),32);
            bAddrConverted = true;
        }
        string ret;
        ret += strIP[0];
        ret += " -> ";
        ret += strIP[1];
        return ret;
    }
};
typedef boost::shared_ptr<FlowStreamPair> FlowStreamPairPtr;
typedef boost::unordered_map<IpFlowPair, FlowStreamPairPtr, FlowPairHash> FlowMap;

template<typename LOCK_T>
struct WaitTask
{
    LOCK_T& lock;
    WaitTask(LOCK_T& alock):lock(alock) {}
    static void dowork(void *p){
        ((WaitTask*) p)->lock.wait();
    }
};

template<typename LOCK_T>
struct UnlockTask
{
    LOCK_T& lock;
    UnlockTask(LOCK_T& alock):lock(alock) {}
    static void dowork(void *p){
        ((UnlockTask*) p)->lock.unlock();
    }
};

template<typename LOCK_T>
struct NotifyTask
{
    LOCK_T& lock;
    NotifyTask(LOCK_T& alock):lock(alock) {}
    static void dowork(void *p){
        ((NotifyTask*) p)->lock.notify();
    }
};


struct StreamTask
{
    FlowStreamPair *strmPair;
    int             nDir;
    mqa::MQmon     *mon;

    const UINT8    *pRtp;
    UINT32          rtpLen;

    // following only for interface feeding
    mqa::StatsFrameParser *parser;

    StreamTask(FlowStreamPair *afs, int dir, const UINT8 *pRtp, UINT32 len, mqa::StatsFrameParser *parser)
        : strmPair(afs), nDir(dir), pRtp(pRtp), parser(parser), rtpLen(len){
            mon = mqa::MQmon::Instance();
    }

    static void dowork(void *param){
        ((StreamTask*)param)->operator()();
        delete ((StreamTask*)param);
    }

    void operator()(){

        mqa::CMQmonInterface *monif = strmPair->itf[nDir]; 
        mqa::CMQmonStream*   &pStream = strmPair->strm[nDir];

        if( pStream ) {
            pStream->IndicateRtpPacket(pRtp, rtpLen, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000);
        }else{
            if(!strmPair->itf[nDir]){  // if first feed in this direction
                strmPair->itf[nDir] = mon->CreateInterface();
            }
            monif = strmPair->itf[nDir];
            monif->IndicateRtpPacket((uintptr_t)monif, pRtp, rtpLen, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000, &pStream);
        }
        if( pStream && pStream->IsValidStream() ) {
            if( pStream->GetMetrics(strmPair->res[nDir]) ){
                strmPair->nCalulation[nDir]++;
                strmPair->sumRes[nDir].nDelay  += strmPair->res[nDir].nDelay;
                strmPair->sumRes[nDir].RFactor += strmPair->res[nDir].RFactor;
                strmPair->sumRes[nDir].MOS += strmPair->res[nDir].MOS;
                strmPair->sumRes[nDir].Jitter += strmPair->res[nDir].Jitter;
                strmPair->sumRes[nDir].fLossRate = strmPair->res[nDir].fLossRate;
                strmPair->sumRes[nDir].nPackets = strmPair->res[nDir].nPackets;
                SetMinMax(strmPair->res[nDir].nDelay, strmPair->minRes[nDir].nDelay, strmPair->maxRes[nDir].nDelay);
                SetMinMax(strmPair->res[nDir].RFactor, strmPair->minRes[nDir].RFactor, strmPair->maxRes[nDir].RFactor);
                SetMinMax(strmPair->res[nDir].MOS, strmPair->minRes[nDir].MOS, strmPair->maxRes[nDir].MOS);
                SetMinMax(strmPair->res[nDir].Jitter, strmPair->minRes[nDir].Jitter, strmPair->maxRes[nDir].Jitter);
                SetMinMax(strmPair->res[nDir].fLossRate, strmPair->minRes[nDir].fLossRate, strmPair->maxRes[nDir].fLossRate);

                /*
                VqtDebug("%s - %s %d: Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d%%\n, TotalPackets=%d\n"
                , strmPair->strIP[0], strmPair->strIP[1], nDir, strmPair->res[nDir].nDelay, (int)strmPair->res[nDir].RFactor, strmPair->res[nDir].MOS
                , (int) strmPair->res[nDir].Jitter, (int) strmPair->res[nDir].fLossRate*100, strmPair->res[nDir].nPackets);

                if( k%100 == 0 ) {
                int ndiv = strmPair->nCalulation[nDir];
                VqtDebug("%s - %s %d [Avg]: Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d%%\n, TotalPackets=%d\n"
                , strmPair->strIP[0], strmPair->strIP[1], nDir, strmPair->sumRes[nDir].nDelay/ndiv, (int)strmPair->sumRes[nDir].RFactor/ndiv, strmPair->sumRes[nDir].MOS/ndiv
                , (int) strmPair->sumRes[nDir].Jitter/ndiv, (int) strmPair->sumRes[nDir].fLossRate*100, strmPair->sumRes[nDir].nPackets);
                }
                */
            }else{  // GetMetrics failed
                VqtDebug("Failed to GetMetrics %s - %s %d\n", strmPair->strIP[0], strmPair->strIP[1], nDir);
            }
        }
    } // end function

    //======== use object pool to speed up
#if MAX_TASKS_IN_POOL > 0
    void* operator new(size_t);
    void operator delete(void*, size_t);
#endif
};
#if MAX_TASKS_IN_POOL > 0

static ftl::FixedSizePool poolStreamTask(sizeof(StreamTask), MAX_TASKS_IN_POOL);

void* StreamTask::operator new(size_t)
{
    return poolStreamTask.allocate();
}
void StreamTask::operator delete(void* p, size_t)
{
    poolStreamTask.deallocate(p);
}

#endif  // MAX_TASKS_IN_POOL

void run(const string pcapfn, int nThread)
{
    const bool  bMultiThread = THREAD_PER_STREAM;

    if(nThread < 1) nThread = 1;
    vector<ThreadPoolPtr> threadpools(nThread);
    FlowMap         flows;
    mqa::MQmon     *mon;
    FrameParserList packetList;

    ftl::FixedSizePool  taskPool(sizeof(StreamTask), 1024*1024);
    for(int i=0; i<nThread; ++i) {
        threadpools[i].reset( new ThreadPool(1, MAX_TASKS_IN_POOL) );
    }

    VqtSetLogLevel(VQT_LOG_DEBUG);
    if(!LoadPcap(pcapfn, packetList))
        return;

    mon = mqa::MQmon::Instance();

    int nCalc = 0;
    const int NLOOP = 1;
    long double time0 = GetTimeMicroSec(), time1;

    for(int k=0; k<NLOOP; ++k) {

        int nPacket = 0;
        int lastThreadIdx = 0;
        for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
            mqa::StatsFrameParser *parser = *it;
            nPacket ++;

            IpPort src(parser->IsIpv4(), parser->SrcIp(), parser->IpTransInfo().TransInfo.nSrcPort);
            IpPort dest(parser->IsIpv4(), parser->DestIp(), parser->IpTransInfo().TransInfo.nDestPort);
            int nDir = FlowDirection(src, dest);
            IpFlowPair aFlow(src,dest);

            FlowMap::iterator     itFlow = flows.find(aFlow);
            mqa::CMQmonInterface *monif=NULL; 
            mqa::CMQmonStream    *pStream = NULL;
            FlowStreamPair       *strmPair = NULL;
            UINT32                rtpLen;
            const UINT8          *pRtp = parser->GetTransPayload(rtpLen);
            bool                  bNewFlow;
            if( itFlow == flows.end() ) {  // new flow
                // insert new into flows
                // pick up a thread
                strmPair = new FlowStreamPair(aFlow, threadpools[lastThreadIdx++]);
                lastThreadIdx %= nThread;
                flows.insert(std::make_pair(aFlow, strmPair));
            }else{
                strmPair = itFlow->second.get();
            }
            StreamTask *pTask = NULL;
            for(int i=0;  i<MAX_TRY_ALLOC && !(pTask=new StreamTask(strmPair, nDir, pRtp, rtpLen, parser)); ++i ) {
                Sleep(50);
            }
            ASSERTMSG2(pTask, "Failed to new StreamTask due to small pool:%d", MAX_TASKS_IN_POOL);
            //if(bMultiThread) {
            bool bPosted = false;
            for(int i=0; i<MAX_TRY_ALLOC && !(bPosted=strmPair->thread->post(StreamTask::dowork, pTask) ); ++i) {
                Sleep(50);
            }
            //}else{
            //    StreamTask::dowork(new StreamTask(strmPair, nDir, pRtp, rtpLen, parser));
            //}
        }
    }

    //if(bMultiThread) 
    {
        // append last tasks to thread pools
        //boost::barrier bar(flows.size()+1);
        //WaitTask<boost::barrier>   lastTask(bar);
        //for(FlowMap::iterator it = flows. begin(); it!=flows.end(); ++it) {
        //    it->second->thread->post(WaitTask<boost::barrier>::dowork, &lastTask);
        //}
        //bar.wait();

        // join all threads
        for(;;) {
            int n=0;
            Sleep(20);
            for(FlowMap::iterator it= flows.begin(); it!=flows.end(); ++it) {
                n += it->second->thread->task_count();
            }
            if( n == 0 )
                break;
        }

        // potential issue: there may be some threads processing the 
    }

    time1 = GetTimeMicroSec();
    VqtDebug("--- streams:%d NLOOP:%d Npackets:%d, NCalc:%d time:%dmilli ---\n", flows.size(),NLOOP, packetList.size(), nCalc, int(time1-time0)/1000 );

    VqtDebug("---------- Streams Summaries -------\n");
    nCalc = 0;
    for(FlowMap::iterator it= flows.begin(); it!=flows.end(); ++it) {
        for(int nDir=0; nDir<2; ++nDir) {
            FlowStreamPairPtr& strmPair = it->second;
            if( strmPair->strm[nDir] ) {
                int ndiv = strmPair->nCalulation[nDir];
                nCalc += ndiv;
                if(ndiv==0) {
                    VqtDebug("%s - %s %d [Avg]: nCalc=%d lossRate=%d, TotalPackets=%d\n"
                        , strmPair->strIP[0], strmPair->strIP[1], nDir, ndiv, (int) strmPair->sumRes[nDir].fLossRate*100, strmPair->sumRes[nDir].nPackets);
                }else{
                    VqtDebug("%s - %s %d [Avg]: nCalc=%d Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d, TotalPackets=%d\n"
                        , strmPair->strIP[0], strmPair->strIP[1], nDir, ndiv, strmPair->sumRes[nDir].nDelay/ndiv, (int)strmPair->sumRes[nDir].RFactor/ndiv, strmPair->sumRes[nDir].MOS/ndiv
                        , (int) strmPair->sumRes[nDir].Jitter/ndiv, (int) strmPair->sumRes[nDir].fLossRate*100, strmPair->sumRes[nDir].nPackets);
                }
            }
        }
    }
    VqtDebug("--- streams:%d NLOOP:%d Npackets:%d, NCalc:%d time:%dmilli ---\n", flows.size(),NLOOP, packetList.size(), nCalc, int(time1-time0)/1000 );
    for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
        delete *it;
    }
    packetList.clear();
}

void run0(const string pcapfn = "call-rodney-002-whole.pcap")
{
    FlowMap     flows;
    mqa::MQmon *mon;
    FrameParserList  packetList;

    VqtSetLogLevel(VQT_LOG_DEBUG);
    if(!LoadPcap(pcapfn, packetList))
        return;

    mon = mqa::MQmon::Instance();

    int nCalc = 0;
    const int NLOOP = 1;
    long double time0 = GetTimeMicroSec(), time1;

    for(int k=0; k<NLOOP; ++k) {

    for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
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

            strmPair.reset(new FlowStreamPair(aFlow));
            strmPair->itf[nDir] = monif;
            strmPair->strm[nDir] = pStream;

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
                    nCalc ++;
                    p->nCalulation[nDir]++;
                    p->sumRes[nDir].nDelay  += p->res[nDir].nDelay;
                    p->sumRes[nDir].RFactor += p->res[nDir].RFactor;
                    p->sumRes[nDir].MOS += p->res[nDir].MOS;
                    p->sumRes[nDir].Jitter += p->res[nDir].Jitter;
                    p->sumRes[nDir].fLossRate = p->res[nDir].fLossRate;
                    p->sumRes[nDir].nPackets = p->res[nDir].nPackets;

                    /*
                    VqtDebug("%s - %s %d: Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d%%\n, TotalPackets=%d\n"
                        , p->strIP[0], p->strIP[1], nDir, p->res[nDir].nDelay, (int)p->res[nDir].RFactor, p->res[nDir].MOS
                        , (int) p->res[nDir].Jitter, (int) p->res[nDir].fLossRate*100, p->res[nDir].nPackets);
                    
                    if( k%100 == 0 ) {
                        int ndiv = p->nCalulation[nDir];
                        VqtDebug("%s - %s %d [Avg]: Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d%%\n, TotalPackets=%d\n"
                            , p->strIP[0], p->strIP[1], nDir, p->sumRes[nDir].nDelay/ndiv, (int)p->sumRes[nDir].RFactor/ndiv, p->sumRes[nDir].MOS/ndiv
                            , (int) p->sumRes[nDir].Jitter/ndiv, (int) p->sumRes[nDir].fLossRate*100, p->sumRes[nDir].nPackets);
                    }
                    */
                }  // GetMetrics
            }  // if pStream
        }  // if old flow pair
    }
    }
    time1 = GetTimeMicroSec();
    VqtDebug("--- NLOOP:%d Npackets:%d, NCalc:%d time:%dmilli ---\n", NLOOP, packetList.size(), nCalc, int(time1-time0)/1000 );

    for(FlowMap::iterator it= flows.begin(); it!=flows.end(); ++it) {
        for(int nDir=0; nDir<2; ++nDir) {
            FlowStreamPairPtr& p = it->second;
            if( p->strm[nDir] ) {
                int ndiv = p->nCalulation[nDir];
                if(ndiv==0) {
                    VqtDebug("%s - %s %d [Avg]: lossRate=%d%%, TotalPackets=%d\n"
                        , p->strIP[0], p->strIP[1], nDir, (int) p->sumRes[nDir].fLossRate*100, p->sumRes[nDir].nPackets);
                }else{
                    VqtDebug("%s - %s %d [Avg]: Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d%%, TotalPackets=%d\n"
                        , p->strIP[0], p->strIP[1], nDir, p->sumRes[nDir].nDelay/ndiv, (int)p->sumRes[nDir].RFactor/ndiv, p->sumRes[nDir].MOS/ndiv
                        , (int) p->sumRes[nDir].Jitter/ndiv, (int) p->sumRes[nDir].fLossRate*100, p->sumRes[nDir].nPackets);
                }
            }
        }
    }
    for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
        delete *it;
    }
    packetList.clear();
}
int _tmain(int argc, _TCHAR* argv[])
{
    string fn;
    int  nThread = -1;
    using namespace boost::program_options;

    boost::program_options::options_description ops;
    ops.add_options()
        ("help,h", "print help message")
        ("traffic,f", value<string>(), "traffic detail, can have multiple with format <traffic_file>%<stream_number_per_thread>%<throughput_byte_per_second>")
        ("thread,t", value<int>(), "set thread number.")
        ;

    variables_map Vm;
    store(parse_command_line(argc, argv, ops), Vm);
    notify(Vm);    

    if (Vm.count("traffic"))
    {
        fn = Vm["traffic"].as<string>();
    }
    if (Vm.count("thread"))
    {
        nThread = Vm["thread"].as<int>();
    }
    
    VqtSetLogLevel(VQT_LOG_DEBUG);
    if(nThread<0) {
        run0(fn);
    }else{
        run(fn, nThread);
    }
    return 0;
}

