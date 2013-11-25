#pragma once

#include <vector>
#include <boost/thread.hpp>

#include <MQmonIf.h>
#include "FifoBuffer.h"

#include <atlcoll.h>

class VQTask {
public:
    enum STATE_T{ S_INITED, S_WORKING, S_STOPPING, S_STOPPED};

    typedef void (*TProcessor)(void *);
    VQTask(int id, size_t  max_buf_size);

    // callable for threadproc
    void operator()(int threadidx);

    typedef boost::shared_ptr<boost::thread>        ThreadPtr;
    ThreadPtr                                _thread;
    FifoBuffer                               _fifoBuf;
    boost::mutex                             _bufMutex;

    struct ProcessData{
        TProcessor proc;
        char data[1];
    };

    bool sendProcess(TProcessor proc, int len, void *data);
    bool sendProcess(TProcessor proc, int len1, void *data1, int len2, void *data2);
    bool sendProcess(TProcessor proc, int len1, void *data1, int len2, void *data2, int len3, void *data3);
    bool sendProcess(TProcessor proc, int len1, void *data1, int len2, void *data2, int len3, void *data3, int len4, void *data4);

    //bool sendTask(CMQmonStream *pStream, CMQmonFrameInfo& info, int len, void *packet);
    //bool sendTask(CMQmonInterface *pInterface, CMQmonFrameInfo& info, int len, void *packet);

    // lock buffer
    void lock(){
        _bufMutex.lock();
    }
    void unlock(){
        _bufMutex.unlock();
    }
    int getLoad();
    void join() {
        while( state() != S_STOPPED ) {
            stop();
            // sleep some time
        }
        _thread->join();
    }
    void stop() {
        _state = S_STOPPING;
    }
    STATE_T state() {
        return _state;
    }
protected:
    int id;
    STATE_T _state;

    void doIdle();
};
typedef boost::shared_ptr<VQTask>  VQTaskPtr;

class VQTaskEngine
    //: public boost::enable_shared_from_this<VQTaskEngine>
{
public:
    VQTaskEngine(int nThread=0, int max_buf_size=1024*1024*100);

    bool sendTask(CMQmonStream *pStream, CMQmonFrameInfo& info, int len, void *packet, int* threadId, bool newFlow);
    bool sendTask(CMQmonInterface *pInterface, CMQmonFrameInfo& info, int len, void *packet, int* threadId, bool newFlow);

protected:

    struct TaskProcData {
        void                *pStream;  // interface or stream
        CMQmonFrameInfo      info;
        int                  lenPacket;
        TaskProcData(void *p, CMQmonFrameInfo& inf, int len):pStream(p), info(inf), lenPacket(len){}
    };
    template<typename STREM_TYPE>
    static void procIndicatePacket(void *param) {
        TaskProcData *ptr = (TaskProcData*) param;
        ((STREM_TYPE*)(ptr->pStream))->IndicatePacket((unsigned char*)param + sizeof(TaskProcData), ptr->lenPacket, ptr->info);
    }


    int idleThread();
    std::vector<VQTaskPtr>    _tasks;
};
typedef boost::shared_ptr<VQTaskEngine> VQTaskEnginePtr;
