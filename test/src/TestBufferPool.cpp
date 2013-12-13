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
int main(int argc, char* argv[])
{
//    printf("boost::function< void() > sizeof:%d\n", sizeof(boost::function< void() >));
//    return 0;
    //test_FifoBuffer();
    //test_MultiSizeBuffer();
    //test_FixedSizeBuffer();
    test_FixedSizeBufferSet();
    return 0;
}
