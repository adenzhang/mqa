#ifndef MQA_STREAMTASK_H_
#define MQA_STREAMTASK_H_

#include "StreamManage.h"
#include "LogUtility.h"
#include <ftl/FixedSizePool.h>

#ifdef _DEBUG
#include <RtpPacketParser.h>
#endif

// if MAX_ELEM==0, use default heap allocator.
// otherwise, use pool allocator which can allocate MAX_ELEM elements at most.
template <typename ELEM_T, size_t MAX_ELEM>
struct PooledAllocator
{
    static ftl::FixedSizePool _elemPool;

    void* operator new(size_t)
    {
        return _elemPool.allocate();
    }
    void operator delete(void* p, size_t)
    {
        _elemPool.deallocate(p);
    }
};
template <typename ELEM_T, size_t MAX_ELEM>
ftl::FixedSizePool PooledAllocator<ELEM_T, MAX_ELEM>::_elemPool(sizeof(ELEM_T), MAX_ELEM);

template <typename ELEM_T>
struct PooledAllocator<ELEM_T, 0>
{
};

template<size_t MAX_ELEM>
struct StreamTask
    : public PooledAllocator<StreamTask<MAX_ELEM>, MAX_ELEM>
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

    static void dowork_del(void *param){
        ((StreamTask*)param)->operator()();
        delete ((StreamTask*)param);
    }
    static void dowork(void *param){
        ((StreamTask*)param)->operator()();
    }

    void operator()(){
//    void StreamTask<MAX_ELEM>::operator()(){

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
                strmPair->res[nDir].nCalulation++;
                strmPair->res[nDir].sumRes.nDelay  += strmPair->res[nDir].res.nDelay;
                strmPair->res[nDir].sumRes.RFactor += strmPair->res[nDir].res.RFactor;
                strmPair->res[nDir].sumRes.MOS += strmPair->res[nDir].res.MOS;
                strmPair->res[nDir].sumRes.Jitter += strmPair->res[nDir].res.Jitter;
                strmPair->res[nDir].sumRes.fLossRate = strmPair->res[nDir].res.fLossRate;
                strmPair->res[nDir].sumRes.nPackets = strmPair->res[nDir].res.nPackets;

                SetMinMax(strmPair->res[nDir].res.nDelay, strmPair->res[nDir].minRes.nDelay, strmPair->res[nDir].maxRes.nDelay);
                SetMinMax(strmPair->res[nDir].res.RFactor, strmPair->res[nDir].minRes.RFactor, strmPair->res[nDir].maxRes.RFactor);
                SetMinMax(strmPair->res[nDir].res.MOS, strmPair->res[nDir].minRes.MOS, strmPair->res[nDir].maxRes.MOS);
                SetMinMax(strmPair->res[nDir].res.Jitter, strmPair->res[nDir].minRes.Jitter, strmPair->res[nDir].maxRes.Jitter);
                SetMinMax(strmPair->res[nDir].res.fLossRate, strmPair->res[nDir].minRes.fLossRate, strmPair->res[nDir].maxRes.fLossRate);

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
    } // end function

    //--- use object pool to speed up
//#if MAX_TASKS_IN_POOL > 0
//    void* operator new(size_t);
//    void operator delete(void*, size_t);
//#endif
};

//typedef PooledAllocator<sizeof(StreamTask), 1000> StreamTaskAllocator;
#endif // MQA_STREAMTASK_H_
