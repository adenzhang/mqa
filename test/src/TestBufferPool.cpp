//#include "stdafx.h"
#include <mqa/mqa_global.h>
#include <assert.h>
#include <ftl/FifoPool.h>
#include <ftl/FixedSizePool.h>
#include <ftl/MultiSizePool.h>
#include <stdio.h>
#include <boost/function.hpp>
#include "LogUtility.h"
#include <mqa/MqaAssert.h>
#include <ftl/AdaptivePoolSet.h>
#include "mqa/ThreadPool.h"
#include <vector>
#include <boost/shared_ptr.hpp>

extern long double GetTimeMicroSec();

void test_FixedSizeBufferSet()
{
    const int NSIZE = 1024*1024;
    const int NELEM = NSIZE + 1024;
    long double time0, time1;
    ftl::FixedSizePoolSet pool(sizeof(int), NSIZE);
    int **buf = new int*[NELEM];

    int i;
    time0 = GetTimeMicroSec();
    for(i=0; i < NELEM; ++i) {
        buf[i] = (int*)pool.allocate();
        ASSERTMSG2(buf[i], "%d", i);
    }
    printf("pools_count:%d capacity:%d size:%d\n", pool.pools_count(), pool.capacity(), pool.size());
    for(i=0; i < NELEM; ++i) {
        pool.deallocate(buf[i]);
    }
    time1 = GetTimeMicroSec();
    ASSERTMSG1(0==pool.size(), "size==0");
    printf("FixedSizePoolSet :%d timing:%d\n", i, int(time1-time0));

    time0 = GetTimeMicroSec();
    for(i=0; i < NELEM; ++i) {
        buf[i] = new int;;
        assert(buf[i]);
    }
    for(i=0; i < NELEM; ++i) {
        delete buf[i];
    }
    time1 = GetTimeMicroSec();
    printf("c++ new :%d timing:%d\n", i, int(time1-time0));

    ASSERTMSG0(pool.size()==0);
    delete[] buf;
}

void test_MultiSizeBuffer()
{
    const int NSIZE = 1024*10;
    long double time0, time1;
    ftl::MultiSizePool pool(sizeof(int), NSIZE);
    int **buf = new int*[NSIZE];

    int i;
    time0 = GetTimeMicroSec();
    for(i=0; i < NSIZE; ++i) {
        buf[i] = (int*)pool.allocate(1);
        //assert(buf[i]);
    }
    for(i=0; i < NSIZE; ++i) {
        pool.deallocate(buf[i]);
    }
    time1 = GetTimeMicroSec();
    printf("MultiSizePool :%d timing:%d\n", i, int(time1-time0));

    ASSERTMSG0(pool.size()==0);
    delete[] buf;
}

void test_FixedSizeBuffer()
{
    const int NSIZE = 1024*10;
    long double time0, time1;
    ftl::FixedSizePool pool(sizeof(int), NSIZE);
    int **buf = new int*[NSIZE];

    int i;
    time0 = GetTimeMicroSec();
    for(i=0; i < NSIZE; ++i) {
        buf[i] = (int*)pool.allocate();
        assert(buf[i]);
    }
    for(i=0; i < NSIZE; ++i) {
        pool.deallocate(buf[i]);
    }
    time1 = GetTimeMicroSec();
    printf("FixedSizePool :%d timing:%d\n", i, int(time1-time0));

    time0 = GetTimeMicroSec();
    for(i=0; i < NSIZE; ++i) {
        buf[i] = new int;;
        assert(buf[i]);
    }
    for(i=0; i < NSIZE; ++i) {
        delete buf[i];
    }
    time1 = GetTimeMicroSec();
    printf("c++ new :%d timing:%d\n", i, int(time1-time0));

    ASSERTMSG0(pool.size()==0);
    delete[] buf;
}
void test_FifoBuffer()
{
    const int NSIZE = 1024*100;
    const int  N = sizeof(int);
    long double time0, time1;
    ftl::FifoPool pool(NSIZE*(N+4));
    size_t n;
    int i=0;

    time0 = GetTimeMicroSec();
    for(i=0; pool.allocate(N); ++i)
        ;
    int j = 0;
    for(j=0; pool.peek(&n); ++j) {
        assert(n == (N));
        pool.deallocate();
    }
    assert(i==j);
    time1 = GetTimeMicroSec();
    printf("FifoPool :%d timing:%d\n", i, int(time1-time0));
    ASSERTMSG0(pool.size()==0);
}

typedef std::pair<int, void*> MemBlock;
typedef std::vector<MemBlock> MemBlockSet;

struct DeallocateTask {

    static void deallocate(MemBlockSet *s) {
        if(!s) return;
        MemBlock *v = &s->at(0);
        int n = s->size();
        for(int i=0; i<n; ++i) {
            ::free(v[i].second);
        }
        delete s;
    }

    static void dowork(void* p){
        deallocate((MemBlockSet *)p);
    }
};
typedef boost::shared_ptr<ThreadPool> ThreadPoolPtr;
struct AllocateTask {
    const int size, count, sizeCount;
    ThreadPoolPtr sink;

    AllocateTask(ThreadPoolPtr sink, int size, int count, int sizeCount): sink(sink), size(size), count(count), sizeCount(sizeCount) {}
    AllocateTask(int size, int count, int sizeCount): size(size), count(count), sizeCount(sizeCount) {}

    MemBlockSet *allocate() {
        MemBlockSet *p = new MemBlockSet(count*sizeCount);
        if(!p) return NULL;

        MemBlock *v = &p->at(0);
        int j = 0;
        for(int i=0; i<count; ++i) {
            int asize = size;
            for(int k=0; k<sizeCount; ++k, asize <<=1 ) {
                v[j].first = asize;
                v[j].second = ::malloc(asize);
                j++;
            }
        }
        return p;
    }

    void operator()() {
        if(!sink->post(allocate())) {
            printf("Failed to post to consumer\n");
        }
    }

    static void dowork(void* p){
        ((AllocateTask*)p)->operator()();
    }
    static void allocate_and_free(void *p) {
        DeallocateTask::deallocate(((AllocateTask*)p)->allocate());
    }
};
typedef boost::shared_ptr<AllocateTask> AllocateTaskPtr;
#define TEST_PROD_CONSUME
void test_malloc_speed()
{
    int Nprod = 1;
    int Ncons = 1;
    int Ntask = 128;
    int Npipelines = 20;
    int NconsQsize = 1024*1024;

    int sizeBegin = 1;
    int sizeRep = 1000*100;
    int sizeCount = 10;
    TimePoint t0 = Now(), t1;

    std::vector<AllocateTaskPtr> allocTasks(Ntask);
    for(int i=0; i<Ntask; ++i) {
        allocTasks[i].reset(new AllocateTask(sizeBegin, sizeRep, sizeCount));
    }

    //--- pipeline model
#ifdef TEST_PIPLELINE
    printf("pipelines:%d, tasks:%d, size:%d->%d, sizeRep:%d\n", Npipelines, Ntask, sizeBegin, sizeBegin<<sizeCount, sizeRep);
    {
        std::vector<ThreadPoolPtr> pipelines(Npipelines);
        for(int i=0; i<Npipelines; ++i) {
            pipelines[i].reset(new ThreadPool(AllocateTask::allocate_and_free, 1, NconsQsize));
        }
        t0 = Now();
        for(int i=0; i<Npipelines; ++i) {
            for(int j=0; j<Ntask; ++j)
                pipelines[i]->post(allocTasks[j].get());
        }
        for(;;) {  // wait for all tasks completion
            int n=0;
            for(int i=0; i<Npipelines; ++i) {
                n += pipelines[i]->task_count();     // pending tasks
                n += pipelines[i]->working_count();  // working tasks
            }
            if( n == 0 ) break; // no working threads or tasks.
            Sleep(10);
        }
        t1 = Now();
        printf("pipeline time:%dms\n", ToMilli(t1-t0));
    }
  #endif

    //--- produce-consume model
 #ifdef TEST_PROD_CONSUME
    printf("producers:%d, consumers:%d, tasks:%d, size:%d->%d, sizeRep:%d\n", Nprod, Ncons, Ntask, sizeBegin, sizeBegin<<sizeCount, sizeRep);

    {
        ThreadPoolPtr producers(new ThreadPool(AllocateTask::dowork, Nprod, Ntask));
        ThreadPoolPtr consumers(new ThreadPool(DeallocateTask::dowork, Ncons, NconsQsize));

        t0 = Now();
        for(int i=0; i<Ntask; ++i) {
            //allocTasks[i]->operator()();
            allocTasks[i]->sink = consumers;
            if(!producers->post(allocTasks[i].get())) {
                printf("Failed to post to producer");
            }
        }

        for(;;) {  // wait for all tasks completion
            int n=0;
                n += producers->task_count();     // pending tasks
                n += producers->working_count();  // working tasks
                n += consumers->task_count();     // pending tasks
                n += consumers->working_count();  // working tasks
            if( n == 0 ) break; // no working threads or tasks.
            Sleep(10);
        }
        t1 = Now();
        printf("produce-consume time:%dms\n", ToMilli(t1-t0));
    }
#endif
}

int main(int argc, char* argv[])
{
//    printf("boost::function< void() > sizeof:%d\n", sizeof(boost::function< void() >));
//    return 0;
    //test_FifoBuffer();
    //test_MultiSizeBuffer();
    //test_FixedSizeBuffer();
//    test_FixedSizeBufferSet();
    test_malloc_speed();
    return 0;
}
