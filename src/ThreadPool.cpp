#include "ThreadPool.h"
#include <queue>
#include <boost/thread.hpp>
//#include <boost/atomic.hpp>

#include "ftl/FifoPool.h"

using namespace std;

//#define _TBB

struct TASK_T
{
    TASK_T(ThreadPool::FUNCTION_T f=NULL, void *p=NULL)
        : pFun(f), pParam(p) 
    {}
    ThreadPool::FUNCTION_T pFun;
    void *pParam;

    void operator()(){
        pFun(pParam);
    }
};
//typedef boost::function< void() > TASK_T;
const size_t TASK_SIZE = sizeof(TASK_T);

//typedef std::queue< TASK_T > StdQueue;

template< typename ELEM_TYPE >
struct StdQueue
{
    std::queue< TASK_T >  pool;

    StdQueue(size_t size){}

    bool empty(){return pool.empty();}
    bool push(const ELEM_TYPE& e){ pool.push(e); return true;}
    ELEM_TYPE front(){ return pool.front();}
    void pop(){ pool. pop();}
    size_t size(){ return pool.size();}
};
typedef StdQueue<TASK_T> StdTaskQ;

template< typename ELEM_TYPE >
struct FiFoQueue
{
    ftl::FifoPool pool;

    FiFoQueue(size_t size)
        : pool(size*(sizeof(ELEM_TYPE)+pool.HEADER_SIZE)) 
    {}

    bool empty(){return pool.empty();}

    bool push(const ELEM_TYPE& e) {
        void *pElem = pool.allocate(sizeof(ELEM_TYPE));
        if(!pElem) return false;
        ::new(pElem) ELEM_TYPE(e);
        return true;
    }
    ELEM_TYPE front() {
        size_t n;
        ELEM_TYPE *p = (ELEM_TYPE*)pool.peek(&n);
        assert(n == sizeof(ELEM_TYPE));
        return *p;
    }

    void pop() { pool.deallocate(); }

    size_t size() { return pool.size(); }
};
struct thread_pool_if
{
    virtual bool run_task(ThreadPool::FUNCTION_T pFun, void *pParam=NULL) = 0;

    virtual size_t task_count() = 0;
    virtual size_t working_count() = 0;

    virtual ~thread_pool_if(){}
};
#ifdef _TBB
#include <tbb/concurrent_queue.h>


struct thread_concurrentQ
    : public thread_pool_if
{
protected:
    typedef tbb::concurrent_bounded_queue<TASK_T> BoundedConcurrentTaskQ_T;
    BoundedConcurrentTaskQ_T  tasks_;
    boost::thread_group       threads_;
    volatile bool             running_;
    const size_t              maxTasks_;
    const size_t              NTHREADS;
    int                       workingThreads_;
public:
    thread_concurrentQ(size_t pool_size,size_t task_size) 
        : maxTasks_(task_size)
          ,running_( true )
          ,workingThreads_(0)
          ,NTHREADS(pool_size?pool_size:1)

    {
        for ( std::size_t i = 0; i < NTHREADS; ++i )
            threads_.create_thread( boost::bind( &thread_concurrentQ::pool_main, this ) ) ;
        if(maxTasks_>0) tasks_.set_capacity(maxTasks_);
    }
    virtual bool run_task(ThreadPool::FUNCTION_T f, void* param) {
        tasks_.push(TASK_T(f,param));
        return true;
    }
    virtual size_t task_count(){
        return tasks_.size() + (NTHREADS - workingThreads_);
    }
    virtual size_t working_count(){
        return workingThreads_;
    }
    virtual ~thread_concurrentQ() {
        running_ = false;
        tasks_.abort();
        try{
            threads_.join_all();
        }catch ( ... ) {}
    }
protected:
    virtual void pool_main(){
        while(running_) {
            try {
                TASK_T task;
                tasks_.pop(task);
                workingThreads_++;
                task();
                workingThreads_--;
            }catch(tbb::user_abort& ){
                break;
            }catch(...) {
                break;
            }
        }
    }
};

#endif // _TBB

typedef FiFoQueue<TASK_T> FiFoTaskQ;

template<typename QUEUE_TYPE>
class thread_pool
    : public thread_pool_if
{
protected:
    QUEUE_TYPE                tasks_;

    boost::thread_group       threads_;
    boost::mutex              mutex_;
    boost::condition_variable condition_;
    volatile bool             running_;
    const size_t              maxTasks_;
    const size_t              NTHREADS;
//    boost::atomic<int>        workingThreads_;
    int                       workingThreads_;
public:

    /// @brief Constructor.
    thread_pool( std::size_t pool_size, size_t task_size )
        : 
        maxTasks_(task_size)
        ,running_( true )
        ,tasks_(task_size)
        ,workingThreads_(0)
        ,NTHREADS(pool_size?pool_size:1)
    {
        for ( std::size_t i = 0; i < NTHREADS; ++i )
            threads_.create_thread( boost::bind( &thread_pool::pool_main, this ) ) ;
    }

    /// @brief Destructor.
    ~thread_pool()
    {
        // Set running flag to false then notify all threads.
        {
            boost::unique_lock< boost::mutex > lock( mutex_ );
            running_ = false;
            condition_.notify_all();
        }

        try
        {
            threads_.join_all();
        }
        // Suppress all exceptions.
        catch ( ... ) {}
    }

    bool run_task(ThreadPool::FUNCTION_T f, void* param)
    {
        boost::unique_lock< boost::mutex > lock( mutex_ );

        // If no threads are available, then return.
        //if ( 0 == available_ ) return false;

        // Decrement count, indicating thread is no longer available.
        //--available_;
        if( !tasks_.push( TASK_T(f, param) ) )
            return false;

        // Set task and signal condition variable so that a worker thread will
        // wake up andl use the task.
        condition_.notify_one();
        return true;
    }
    size_t task_count() {
        boost::unique_lock< boost::mutex > lock( mutex_ );
        size_t n = tasks_.size();
        return n;
    }
    size_t working_count() {
        return workingThreads_;//.load(boost::memory_order_relaxed);
    }
protected:
    virtual void pool_main()
    {
        while( running_ )
        {
            // Wait on condition variable while the task is empty and the pool is
            // still running.
            boost::unique_lock< boost::mutex > lock( mutex_ );
            while ( tasks_.empty() && running_ )
            {
                condition_.wait( lock );
            }
            // If pool is no longer running, break out.
            if ( !running_ ) break;

            // Copy task locally and remove from the queue.  This is done within
            // its own scope so that the task object is destructed immediately
            // after running the task.  This is useful in the event that the
            // function contains shared_ptr arguments bound via bind.
            {
                // Run the task.

                TASK_T task = tasks_.front();
                tasks_.pop();

                workingThreads_++; //.fetch_add(1, boost::memory_order_relaxed);

                lock.unlock();

                try
                {
                    task();
                }
                // Suppress all exceptions.
                catch ( ... ) {}
                workingThreads_--; //.fetch_sub(1, boost::memory_order_relaxed);
            }

            // Task has finished, so increment count of available threads.
            //lock.lock();
            //++available_;
        } // while running_
    }
};

// optimize for one thread model.
template<typename QUEUE_TYPE>
struct thread_one
    : public thread_pool<QUEUE_TYPE>
{
    thread_one(size_t task_size)
        : thread_pool<QUEUE_TYPE>(1, task_size)
    {}
    virtual bool run_task(ThreadPool::FUNCTION_T f, void* param) {
        boost::unique_lock< boost::mutex > lock( thread_pool<QUEUE_TYPE>::mutex_ );
        return thread_pool<QUEUE_TYPE>::tasks_.push( TASK_T(f, param) );
    }
private:
    virtual void pool_main()
    {
        while( thread_pool<QUEUE_TYPE>::running_ ) {
			if(!thread_pool<QUEUE_TYPE>::tasks_.empty() ) {
                boost::unique_lock< boost::mutex > lock( thread_pool<QUEUE_TYPE>::mutex_ );
                try{
                    TASK_T task = thread_pool<QUEUE_TYPE>::tasks_.front();
                    thread_pool<QUEUE_TYPE>::tasks_.pop();
                    thread_pool<QUEUE_TYPE>::workingThreads_++; //.fetch_add(1, boost::memory_order_relaxed);
                    lock.unlock();
                    task();
                }
                catch ( ... ) {}
                thread_pool<QUEUE_TYPE>::workingThreads_--; //.fetch_sub(1, boost::memory_order_relaxed);
			}// if
        } // while
    }
};

ThreadPool::ThreadPool( std::size_t pool_size, size_t task_size )
 //: priv((void*)(new thread_one<StdTaskQ>(task_size)))
    //: priv((void*)(new thread_concurrentQ(pool_size, task_size)))
    : priv( pool_size>1? 
#ifdef _TBB
                         ((void*)(new thread_concurrentQ(pool_size, task_size)) )
#else
                         (task_size==0? (void*)(new thread_pool<StdTaskQ>(pool_size, task_size))
                                        : (void*)(new thread_pool<FiFoTaskQ>(pool_size, task_size)) )
#endif
                         :(task_size==0? (void*)(new thread_one<StdTaskQ>(task_size))
                                        : (void*)(new thread_one<FiFoTaskQ>(task_size)) )
             )
{
};
ThreadPool::~ThreadPool()
{
    delete ((thread_pool_if*)priv);
}
bool ThreadPool::post(FUNCTION_T pFun, void *pParam)
{
    return ((thread_pool_if*)priv)->run_task(pFun, pParam);
}
size_t ThreadPool::task_count()
{
    return ((thread_pool_if*)priv)->task_count();
}
size_t ThreadPool::working_count()
{
    return ((thread_pool_if*)priv)->working_count();
}
