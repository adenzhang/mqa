// mqatest.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
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
#include "StreamManage.h"
#include "StreamTask.h"

#ifndef WIN32  // linux
#include <unistd.h>
#define Sleep(milli) usleep(milli*1000)
#endif

// 16byte swap
#define Swap16(swap_nValue) \
    ((UINT16)((((swap_nValue) >> 8) & 0x00FF) | (((swap_nValue) << 8) & 0xFF00)))

// 32byte swap
#define Swap32(swap_nValue) \
    ((UINT32)((((swap_nValue) >> 24) & 0x000000FF) |  \
    (((swap_nValue) >>  8) & 0x0000FF00) |   \
    (((swap_nValue) <<  8) & 0x00FF0000) |  \
    (((swap_nValue) << 24) & 0xFF000000)))

#define SwapAdd16(v, a) \
    v = Swap16(Swap16(v)+a)

#define SwapAdd32(v, a) \
    v = Swap32(Swap32(v)+a)

//#define TIME_PROCESS

using namespace std;

//#define MAX_TASKS_PER_THREAD (1024*1024)
#define MAX_TASKS_IN_POOL (1024*100)

//#define MAX_TASKS_PER_THREAD 0
//#define MAX_TASKS_IN_POOL 0

const int MAX_TRY_ALLOC = 1000;  // pool alloc or post task to thread pool
static bool g_bTime = false;

typedef StreamTask<MAX_TASKS_IN_POOL> PooledStreamTask;
typedef StreamTask<0>                 MyStreamTask;

void output_stats(int time, FlowMap& flows, size_t nPackets)
{
    int nCalc = 0;
    int nRtpStreams = 0;
    for(FlowMap::iterator it= flows.begin(); it!=flows.end(); ++it) {
        for(int nDir=0; nDir<2; ++nDir) {
            FlowStreamPairPtr strmPair = it->second;
            if( strmPair->strm[nDir] ) {
                for(int i=0; i< strmPair->saved[nDir].size(); ++i) {
                    RtpFlowResult* res = &strmPair->saved[nDir][i];
#ifdef _DEBUG
                    if( res->currState != RtpFlowResult::S_VALID) 
                        continue;
#endif
                    nRtpStreams++;
                    int ndiv = res->nCalulation;
                    nCalc += ndiv;
                    if(ndiv==0) {
                        VqtDebug("%s - %s %d [Avg]: TotalPackets=%d\n"
                            , strmPair->strIP[0], strmPair->strIP[1], nDir, res->sumRes.nPackets);
                    }else{
#ifdef _DEBUG
                        VqtDebug("%s - %s %d [Avg]: nCalc=%d Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d, TotalPackets=%d, SeqNO:%d-%d\n"
                            , strmPair->strIP[0], strmPair->strIP[1], nDir, ndiv, res->sumRes.nDelay/ndiv, (int)res->sumRes.RFactor/ndiv, res->sumRes.MOS/ndiv
                            , (int) res->sumRes.Jitter/ndiv, (int)res->sumRes.fLossRate*100, res->sumRes.nPackets, res->startID, res->currID);
#else
                        VqtDebug("%s - %s %d [Avg]: nCalc=%d Delay=%d RFactor=%d MOS=%3.1f Jitter=%dms, lossRate=%d, TotalPackets=%d\n"
                            , strmPair->strIP[0], strmPair->strIP[1], nDir, ndiv, res->sumRes.nDelay/ndiv, (int)res->sumRes.RFactor/ndiv, res->sumRes.MOS/ndiv
                            , (int) res->sumRes.Jitter/ndiv, (int)res->sumRes.fLossRate*100, res->sumRes.nPackets);
#endif
                    }
                } // for saved results
            }  // if has flow in nDir
        }  // for nDir
    }  // for FlowMap
    if(g_bTime)
        VqtDebug("--- flows:%d, Npackets:%d, RTPStreams:%d, NCalc:%d, time:%dms---\n", flows.size(), nPackets, nRtpStreams, nCalc, int(time)/1000 );
    else
        VqtDebug("--- flows:%d, Npackets:%d, RTPStreams:%d, NCalc:%d ---\n", flows.size(), nPackets, nRtpStreams, nCalc);

}

const int RTP_PACKET_SIZE  = 180-8; // bytes
const int TIMESTAMP_INC = 160;

struct RtpPacket {
    UINT8 Ver:2;
    UINT8 Pad:1;
    UINT8 Ext:1;
    UINT8 CC:2;
    UINT8 Mark:1;
    UINT8 PT;
    UINT16 Seq;
    UINT32 TimeStamp;
    UINT32 SSRC;
};
void init_RtpPacket(UINT8 *buf) {
    RtpPacket& P = *((RtpPacket*)buf);
    P.Ver = 2;
    P.Pad = 0;
    P.Ext = 0;
    P.CC = 0;
    P.Mark = 1;
    P.PT = 0;
    P.Seq = 1;
    P.TimeStamp = 0;
    P.SSRC = 0x0000b012;
}
struct RtpPacketInfo {
    UINT8 buf[RTP_PACKET_SIZE];
    int   len;

    RtpPacketInfo():len(RTP_PACKET_SIZE){}
};
void next_RtpPacket(UINT8 *buf) {
    SwapAdd16(*((UINT16*)(buf+2)), 1);
    SwapAdd32(*((UINT32*)(buf+4)), TIMESTAMP_INC);
    //RtpPacket& P = *((RtpPacket*)buf);
    //P.Seq +=1;
    //P.TimeStamp += TIMESTAMP_INC;
}
// extreme load test
void performance_test(int nflows)
{
    const UINT64 NFLOWS                  = nflows;    // flows

    const UINT64 NPACKETS_TIME           = (10000*10000/NFLOWS/10);   // number of packets per timing

    std::vector<RtpPacketInfo>           packets(NFLOWS);
    std::vector<mqa::CMQmonInterface*>   intf(NFLOWS);
    std::vector<mqa::CMQmonStream*>      strm(NFLOWS);
    mqa::MQmon                          *mon = mqa::MQmon::Instance();
    ftl::timenano                        capTime = 0;
    mqa::CMQmonMetrics                   res, maxRes, minRes, avgRes, sumRes;

    // init RTP packets
    for(int i=0;i<NFLOWS; ++i) {
        UINT8 *p = &packets[i].buf[0];
        //init_RtpPacket(p);
        *((UINT16*)p) = 0x8080; //Version, P, X, CC, M, PT
        p += 2;
        *((UINT16*)p) = Swap16(1);   // sequence number
        p += 2;
        *((UINT32*)p) = 0;  // timestamp
        p += 4;
        *((UINT32*)p) = 0x0000b012;  // SSRC

        intf[i] = mon->CreateInterface();
        strm[i] = NULL;
    }

    printf("NFlows:%d\n", NFLOWS);

    long double t0 = GetTimeMicroSec(),  t1;
    UINT32 nCalc = 0;
    for(UINT64 seq=0; ; seq++) {
        UINT16 packetLen = packets[0].len;
        UINT8 *pPacket = &packets[0].buf[0];
        for(int i=0; i<NFLOWS; ++i) {
            if(strm[i])
                strm[i]->IndicateRtpPacket(pPacket, packetLen, capTime.sec, capTime.nsec);
            else
                intf[i]->IndicateRtpPacket(i, pPacket, packetLen, capTime.sec, capTime.nsec, &strm[i]);
            if( strm[i] && strm[i]->IsValidStream() ) {
                if( strm[i]->GetMetrics(res) ) {
                    sumRes.nDelay  += res.nDelay;
                    sumRes.RFactor += res.RFactor;
                    sumRes.MOS += res.MOS;
                    sumRes.Jitter += res.Jitter;
                    sumRes.fLossRate = res.fLossRate;
                    sumRes.nPackets = res.nPackets;

                    SetMinMax(res.nDelay, minRes.nDelay, maxRes.nDelay);
                    SetMinMax(res.RFactor, minRes.RFactor, maxRes.RFactor);
                    SetMinMax(res.MOS, minRes.MOS, maxRes.MOS);
                    SetMinMax(res.Jitter, minRes.Jitter, maxRes.Jitter);
                    SetMinMax(res.fLossRate, minRes.fLossRate, maxRes.fLossRate);
                    nCalc++;
                }
            }
        } // for flows
        capTime += ftl::timemilli(20);
        next_RtpPacket(pPacket);

        if(seq%NPACKETS_TIME == 0) 
        {
            t1 = GetTimeMicroSec();
            printf("nPackets:%dM time:%dms nCalc:%d\n", 
                (UINT32)((seq/NPACKETS_TIME)*NFLOWS*NPACKETS_TIME/1000000), int(t1-t0)/1000, 
                (UINT32)(nCalc));
            t0 = t1;
            nCalc = 0;
        }
    } // for sequence number
    t1 = GetTimeMicroSec();


}
void multi_run(const string pcapfn, int nThread)
{
    const bool  bMultiThread = nThread>0;

    if(nThread < 1) nThread = 1;
    vector<ThreadPoolPtr> threadpools(nThread);
    FlowMap         flows;
    mqa::MQmon     *mon;
    FrameParserList packetList;

    for(int i=0; i<nThread; ++i)
        threadpools[i].reset( new ThreadPool(1, MAX_TASKS_IN_POOL) );  // throws if unable to allocate memory.

    if(!LoadPcap(pcapfn, packetList)) {
        VqtError("Failed to load pcap file: %s\n", pcapfn.c_str());
        return;
    }

    mon = mqa::MQmon::Instance();

    int nCalc = 0;
    const int NLOOP = 1;
    long double time0 = GetTimeMicroSec(), time1, time2, timePostTask = 0;

    {

        int nPacket = 0;
        int lastThreadIdx = 0;
        time2 = GetTimeMicroSec();
        for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
            nPacket ++;
#ifdef TIME_PROCESS
            if( nPacket % 1000 == 0 ) {
                double t = GetTimeMicroSec();
                printf("%d time:%d flows:%d timePostTaks:%d\n", nPacket, int(t-time2), flows.size(),int(timePostTask));
                //time2 = t;
                //timePostTask = 0;
            }
#endif

            mqa::StatsFrameParser *parser = *it;
            IpFlowPairPtr          tunnel;

            if( parser->nTunnelType ) {
                IpPort tsrc(parser->IsIpv4(false), parser->SrcIp(false), parser->IpTransInfo(false).TransInfo.nSrcPort);
                IpPort tdest(parser->IsIpv4(false), parser->DestIp(false), parser->IpTransInfo(false).TransInfo.nDestPort);
                tunnel.reset(new IpFlowPair(tsrc, tdest));
            }

            IpPort                src(parser->IsIpv4(), parser->SrcIp(), parser->IpTransInfo().TransInfo.nSrcPort);
            IpPort                dest(parser->IsIpv4(), parser->DestIp(), parser->IpTransInfo().TransInfo.nDestPort);
            int                   nDir = FlowDirection(src, dest);
            IpFlowPair            aFlow(src,dest, tunnel);

            FlowMap::iterator     itFlow = flows.find(aFlow);
            mqa::CMQmonInterface *monif=NULL; 
            mqa::CMQmonStream    *pStream = NULL;
            FlowStreamPairPtr     strmPair;
            UINT32                rtpLen;
            const UINT8          *pRtp = parser->GetTransPayload(rtpLen);

            if( itFlow == flows.end() ) {  // new flow
                // insert new into flows
                strmPair.reset( new FlowStreamPair(aFlow, threadpools[lastThreadIdx++]) ); // pick up a thread
                lastThreadIdx %= nThread;
                flows[aFlow] = strmPair;
            } else {
                strmPair = itFlow->second;
            }
            if(bMultiThread) {
                int i;
#ifdef TIME_PROCESS
                long double t=GetTimeMicroSec();
#endif
                PooledStreamTask *pTask = NULL;
                for(i=0;  i<MAX_TRY_ALLOC && !(pTask=new PooledStreamTask(strmPair.get(), nDir, pRtp, rtpLen, parser)); ++i ) {
                    Sleep(10);
                }
                if(i>0) VqtInfo("%d tried %d alloc\n", nPacket, i);
                ASSERTMSG2(pTask, "Failed to new StreamTask due to small pool:%d", MAX_TASKS_IN_POOL);
                bool bPosted = false;
                for(i=0; i<MAX_TRY_ALLOC && !(bPosted=strmPair->thread->post(PooledStreamTask::dowork_del, pTask) ); ++i) {
                    Sleep(10);
                }
                if(i>0) VqtInfo("%d tried %d post\n", nPacket, i);
#ifdef TIME_PROCESS
                timePostTask += (GetTimeMicroSec()-t);
#endif
            }else{
#ifdef TIME_PROCESS
                long double t=GetTimeMicroSec();
#endif
                MyStreamTask::dowork(&MyStreamTask(strmPair.get(), nDir, pRtp, rtpLen, parser));
#ifdef TIME_PROCESS
                timePostTask += (GetTimeMicroSec()-t);
#endif
            }
        }
    }
#ifdef TIME_PROCESS
    time2 = GetTimeMicroSec();
    printf("\nFeeding time:%d flows:%d\n", int(time2-time0), flows.size());
#endif

    if(bMultiThread){  // join all threads
        for(;;) {
            volatile int n=0, m = 0;
            for(FlowMap::iterator it= flows.begin(); n==0 && it!=flows.end(); ++it) {
            	int k =0;
                if( k = it->second->thread->task_count() ) {    // pending tasks
                    n += k;
//                	printf("thread %d pending tasks %d\n", m, k);
//                	break;
                }
                if( k = it->second->thread->working_count() ) {  // working tasks
                    n += k;
//                	printf("thread %d working tasks %d\n", m, k);
//                	break;
                }

                m++;
            }
            if( n == 0 ) break;
            Sleep(10);
        }
    }
#ifdef TIME_PROCESS
    printf("Joining time:%d\n", int(GetTimeMicroSec()-time2));
#endif

    time1 = GetTimeMicroSec();
    for(FlowMap::iterator it= flows.begin(); it!=flows.end(); ++it) {
        for(int nDir=0; nDir<2; ++nDir) {
            FlowStreamPairPtr strmPair = it->second;
            if( strmPair->strm[nDir] ) {
#ifdef _DEBUG
                if( strmPair->res[nDir].currState == RtpFlowResult::S_VALID )
#endif
                    strmPair->save(nDir);
            }
        }
    }
    output_stats(int(time1-time0), flows, packetList.size());

    for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
        delete *it;
    }
    packetList.clear();
}

void single_run(const string pcapfn = "call10.pcap")
{
    FlowMap     flows;
    mqa::MQmon *mon;
    FrameParserList  packetList;

    VqtSetLogLevel(VQT_LOG_DEBUG);
    if(!LoadPcap(pcapfn, packetList)) {
        VqtDebug("Error Failed to load file <%s>\n", pcapfn.c_str());
        return;
    }

    mon = mqa::MQmon::Instance();

    int nCalc = 0;
    const int NLOOP = 1;
    long double time0 = GetTimeMicroSec(), time1, time2;

    for(int k=0; k<NLOOP; ++k) {

        int nPacket = 0;
        time2 = GetTimeMicroSec();
        for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
            ++nPacket;
#ifdef TIME_PROCESS
            if( nPacket % 1000 == 0 ) {
                double t = GetTimeMicroSec();
                printf("%d time:%d flows:%d calcs:%d\n", nPacket, int(t-time2), flows.size(), nCalc);
                time2 = t;
            }
#endif
            mqa::StatsFrameParser *parser = *it;
            IpFlowPairPtr          tunnel;

            if( parser->nTunnelType ) {
                IpPort tsrc(parser->IsIpv4(false), parser->SrcIp(false), parser->IpTransInfo(false).TransInfo.nSrcPort);
                IpPort tdest(parser->IsIpv4(false), parser->DestIp(false), parser->IpTransInfo(false).TransInfo.nDestPort);
                tunnel.reset(new IpFlowPair(tsrc, tdest));
            }
            IpPort src(parser->IsIpv4(), parser->SrcIp(), parser->IpTransInfo().TransInfo.nSrcPort);
            IpPort dest(parser->IsIpv4(), parser->DestIp(), parser->IpTransInfo().TransInfo.nDestPort);
            int nDir = FlowDirection(src, dest);
            IpFlowPair aFlow(src,dest, tunnel);

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

                ////flows.insert(std::make_pair(aFlow, strmPair));
                //flows.insert(FlowMap::Pair(aFlow, strmPair));
                flows[aFlow] = strmPair;
            }else{  // old flow pair
                strmPair = itFlow->second;
                if( strmPair->strm[nDir] ) {
                    pStream = strmPair->strm[nDir];
                    pStream->IndicateRtpPacket(pRtp, rtpLen, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000);
                }else{
                    if(!strmPair->itf[nDir]){  // if first time to feed in this direction
                        assert(strmPair->itf[nDir?0:1]);  // the other direction should've interface.
                        strmPair->itf[nDir] = mon->CreateInterface();
                        assert(strmPair->strm[nDir] == NULL);  //only interface can detect new stream.
                    }
                    monif = strmPair->itf[nDir];
                    monif->IndicateRtpPacket((uintptr_t)monif, pRtp, rtpLen, parser->nTimestamp.tv_sec, parser->nTimestamp.tv_usec*1000, &pStream);
                    if(pStream)
                        strmPair->strm[nDir] = pStream;
                }
            }
            if( pStream && pStream->IsValidStream() ) {
#ifdef _DEBUG
                mqa::RtpPacketParser rtpParser((const char*)pRtp, rtpLen);
                rtpParser.Parse();
                if(strmPair->res[nDir].currState != RtpFlowResult::S_VALID) {
                    strmPair->res[nDir].currState = RtpFlowResult::S_VALID;
                    strmPair->res[nDir].startID = rtpParser.sequenceNum;
                }
                strmPair->res[nDir].currID = rtpParser.sequenceNum;
#endif

                if( pStream->GetMetrics(strmPair->res[nDir].res) ){
                    nCalc ++;
                    strmPair->res[nDir].nCalulation++;
                    strmPair->res[nDir].sumRes.nDelay  += strmPair->res[nDir].res.nDelay;
                    strmPair->res[nDir].sumRes.RFactor += strmPair->res[nDir].res.RFactor;
                    strmPair->res[nDir].sumRes.MOS += strmPair->res[nDir].res.MOS;
                    strmPair->res[nDir].sumRes.Jitter += strmPair->res[nDir].res.Jitter;
                    strmPair->res[nDir].sumRes.fLossRate = strmPair->res[nDir].res.fLossRate;
                    strmPair->res[nDir].sumRes.nPackets = strmPair->res[nDir].res.nPackets;

                    if(strmPair->res[nDir].nCalulation == 1){
                        InitMinMax(strmPair->res[nDir].res.nDelay, strmPair->res[nDir].minRes.nDelay, strmPair->res[nDir].maxRes.nDelay);
                        InitMinMax(strmPair->res[nDir].res.RFactor, strmPair->res[nDir].minRes.RFactor, strmPair->res[nDir].maxRes.RFactor);
                        InitMinMax(strmPair->res[nDir].res.MOS, strmPair->res[nDir].minRes.MOS, strmPair->res[nDir].maxRes.MOS);
                        InitMinMax(strmPair->res[nDir].res.Jitter, strmPair->res[nDir].minRes.Jitter, strmPair->res[nDir].maxRes.Jitter);
                        InitMinMax(strmPair->res[nDir].res.fLossRate, strmPair->res[nDir].minRes.fLossRate, strmPair->res[nDir].maxRes.fLossRate);
                    }else{
                        SetMinMax(strmPair->res[nDir].res.nDelay, strmPair->res[nDir].minRes.nDelay, strmPair->res[nDir].maxRes.nDelay);
                        SetMinMax(strmPair->res[nDir].res.RFactor, strmPair->res[nDir].minRes.RFactor, strmPair->res[nDir].maxRes.RFactor);
                        SetMinMax(strmPair->res[nDir].res.MOS, strmPair->res[nDir].minRes.MOS, strmPair->res[nDir].maxRes.MOS);
                        SetMinMax(strmPair->res[nDir].res.Jitter, strmPair->res[nDir].minRes.Jitter, strmPair->res[nDir].maxRes.Jitter);
                        SetMinMax(strmPair->res[nDir].res.fLossRate, strmPair->res[nDir].minRes.fLossRate, strmPair->res[nDir].maxRes.fLossRate);
                    }

                }  // GetMetrics
            }else{  // invalid RTP stream
#ifdef _DEBUG
                if(strmPair->res[nDir].currState != RtpFlowResult::S_INVALID) {
                    if(strmPair->res[nDir].currState == RtpFlowResult::S_VALID) {
                        strmPair->save(nDir);
                    }
                    strmPair->res[nDir].currState = RtpFlowResult::S_INVALID;
                    strmPair->res[nDir].startID = 0;
                    strmPair->res[nDir].currID = 0;
                }
#endif
            }
        }
    }
    time1 = GetTimeMicroSec();
    for(FlowMap::iterator it= flows.begin(); it!=flows.end(); ++it) {
        for(int nDir=0; nDir<2; ++nDir) {
            FlowStreamPairPtr strmPair = it->second;
            if( strmPair->strm[nDir] ) {
#ifdef _DEBUG
                if( strmPair->res[nDir].currState == RtpFlowResult::S_VALID )
#endif
                    strmPair->save(nDir);
            }
        }
    }
    output_stats(int(time1-time0), flows, packetList.size());

    for(FrameParserList::iterator it = packetList.begin(); it!= packetList.end(); ++it) {
        delete *it;
    }
    packetList.clear();
}

struct NullTask{
	volatile int id;
    NullTask(int id=0): id(id) {}
    void operator()() {
//        printf("NullTask::() %d\n", id);
    }
    static void dowork(void* p){
    	((NullTask*)p)->operator()();
    }
};

void test_threadpool_speed()
{
    int NTASKS = MAX_TASKS_IN_POOL*100;
    int NTHREADS = 4;
    std::vector<NullTask> tasks(NTASKS);
    list<int>             iList;
    boost::condition_variable cond;
    for(int i=0; i<NTASKS; ++i)
        tasks[i].id = i;

    std::vector<ThreadPoolPtr> threads(NTHREADS);
    for(int i=0; i<NTHREADS; ++i)
        threads[i].reset(new ThreadPool(1, 0));

    long double t0 = GetTimeMicroSec(), t1, t2;
    for(int i=0;i<NTASKS; ++i){
//        threads[i%NTHREADS]->post(NullTask::dowork, &tasks[i]);
//        threads[i%NTHREADS]->post(tasks[i]);
        if(!threads[i%NTHREADS]->post(NullTask::dowork, &tasks[i]))
            printf("failed to post %d\n", i);

        //boost::unique_lock< boost::mutex > lock(mtx);
        //n += i;
    }
    t1 = GetTimeMicroSec();
    printf("%d tasks, time post:%dms\n", NTASKS, int(t1-t0)/1000 );

    for(;;) {
        int n=0;
        for(int i=0; n==0 && i<NTHREADS; ++i) 
        {
            if( n = threads[i%NTHREADS]->task_count()) {
                //printf("pending tasks for %d:%d\n", i, n);
                break;
            }
            if( n = threads[i%NTHREADS]->working_count()) {
                //printf("working tasks for %d:%d\n", i, n);
                break;
            }
            n += threads[i%NTHREADS]->task_count();     // pending tasks
            n += threads[i%NTHREADS]->working_count();  // working tasks
        }
        if( n == 0 ) break; // no working threads or tasks.
        printf("tasks %d\n", n);
        Sleep(10);
    }
    t2 = GetTimeMicroSec();
    printf("time joined:%dms\n", int(t2-t1)/1000 );

}
int main(int argc, char* argv[])
{
    //test_threadpool_speed();
    //performance_test();
    //return 0;
    string fn;
    int    nThread = -1;
    int    bPerf = false;
    int    nStreams = 100;
    using namespace boost::program_options;

    boost::program_options::options_description ops;
    ops.add_options()
        ("help,h", "print help message")
        ("traffic,f", value<string>(), "traffic detail, can have multiple with format <traffic_file>%<stream_number_per_thread>%<throughput_byte_per_second>")
        ("threads,t", value<int>(), "set thread number.")
        ("time,T", "time the program")
        ("perf,P", "performance test")
        ("streams,S", value<int>(), "number of streams for perf test")
        ;

    variables_map Vm;
    store(parse_command_line(argc, argv, ops), Vm);
    notify(Vm);    

    if(Vm.count("help")) {
        ops.print(std::cout);
        return 0;
    }
    if( Vm.count("perf") )
    {
        bPerf =  true;
        if( Vm.count("streams") )
        {
            nStreams =  Vm["streams"].as<int>();
        }
    }else if (Vm.count("traffic"))
    {
        fn = Vm["traffic"].as<string>();
    }
	else{
        ops.print(std::cout);
        return 1;
    }
    if (Vm.count("threads"))
    {
        nThread = Vm["threads"].as<int>();
    }
    if( Vm.count("time") )
    {
        g_bTime = true;
    }
    
    VqtSetLogLevel(VQT_LOG_DEBUG);

    long double time0 = GetTimeMicroSec(), time1;
    if(bPerf) {
        performance_test(nStreams);
    }else if(nThread<0) {
        single_run(fn);
    }else{
        multi_run(fn, nThread);
    }
    time1 = GetTimeMicroSec();
    //    printf("Timing App:%dmilli\n", int(time1-time0)/1000);
    return 0;
}

