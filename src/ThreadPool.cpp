#include "mqa/ThreadPool.h"
#include <queue>
#include <boost/thread.hpp>
//#include <boost/atomic.hpp>

#include "ftl/FifoPool.h"
using namespace std;

//#define USE_TBB

template< typename ARG_T >
struct TASK1_T
{
    typedef ARG_T arg_type;
    TASK1_T(ThreadPool::FUNCTION_T f, const ARG_T& p)
        : pParam(p)
    {}
    TASK1_T()
    {}
    ARG_T pParam;

};

template< typename ARG_T >
struct TASK2_T
{
    typedef ARG_T arg_type;

    TASK2_T(ThreadPool::FUNCTION_T f=ThreadPool::FUNCTION_T(), const ARG_T& p= ARG_T())
        : pFun(f), pParam(p)
    {}
    TASK2_T(const TASK1_T<ARG_T> &task1, ThreadPool::FUNCTION_T fun)
        : pFun(fun), pParam(task1.pParam)
    {}
    TASK2_T(const TASK2_T &task2, ThreadPool::FUNCTION_T fun)
        : pFun(task2.pFun?task2.pFun:fun), pParam(task2.pParam)
    {}

    ThreadPool::FUNCTION_T pFun;
    ARG_T pParam;

    void operator()(){
        pFun(pParam);
    }


};

template< typename ELEM_TYPE >
struct StdQueue
{
    typedef ELEM_TYPE value_type;
    std::queue< ELEM_TYPE >  pool;

    StdQueue(size_t size){}

    bool empty(){return pool.empty();}
    bool push(const ELEM_TYPE& e){ pool.push(e); return true;}
    ELEM_TYPE front(){ return pool.front();}
    void pop(){ pool. pop();}
    size_t size(){ return pool.size();}
};

template< typename ELEM_TYPE >
struct FifoQueue
{
    typedef ELEM_TYPE value_type;
    ftl::FifoPool pool;

    FifoQueue(size_t size)
        : pool(size*(sizeof(ELEM_TYPE)))
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
template<typename ARG_T>
struct thread_pool_if
{
    virtual bool run_task(ThreadPool::FUNCTION_T pFun, const ARG_T& pParam) {return false;}

    virtual size_t task_count() {return 0;}
    virtual size_t working_count() {return 0;}

    virtual ~thread_pool_if(){}
};
#ifdef USE_TBB
#include <tbb/concurrent_queue.h>

template<typename TASK_T>
struct thread_concurrentQ
    : public thread_pool_if<typename TASK_T::arg_type>
{
protected:
    typedef typename TASK_T::arg_type arg_type;
    typedef tbb::concurrent_bounded_queue<TASK_T> BoundedConcurrentTaskQ_T;
    BoundedConcurrentTaskQ_T  tasks_;
    const ThreadPool::FUNCTION_T    taskProc_;
    boost::thread_group       threads_;
    volatile bool             running_;
    const size_t              maxTasks_;
    const size_t              NTHREADS;
    int                       workingThreads_;
public:
    thread_concurrentQ(size_t pool_size,size_t task_size) 
        : maxTasks_(task_size)
        , taskProc_(ThreadPool::FUNCTION_T())
          ,running_( true )
          ,workingThreads_(0)
          ,NTHREADS(pool_size?pool_size:1)

    {
        for ( std::size_t i = 0; i < NTHREADS; ++i )
            threads_.create_thread( boost::bind( &thread_concurrentQ::pool_main, this ) ) ;
        if(maxTasks_>0) tasks_.set_capacity(maxTasks_);
    }
    thread_concurrentQ(ThreadPool::FUNCTION_T fun, size_t pool_size, size_t task_size)
        : maxTasks_(task_size)
        , taskProc_(fun)
          ,running_( true )
          ,workingThreads_(0)
          ,NTHREADS(pool_size?pool_size:1)

    {
        for ( std::size_t i = 0; i < NTHREADS; ++i )
            threads_.create_thread( boost::bind( &thread_concurrentQ::pool_main, this ) ) ;
        if(maxTasks_>0) tasks_.set_capacity(maxTasks_);
    }
    virtual bool run_task(ThreadPool::FUNCTION_T f, const arg_type& param) {
        if( f == ThreadPool::FUNCTION_T() && taskProc_ == ThreadPool::FUNCTION_T() ) return false;
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
                {
                    TASK2_T<arg_type> task2(task, taskProc_);
                    task2();
                }
                workingThreads_--;
            }catch(tbb::user_abort& ){
                break;
            }catch(...) {
                break;
            }
        }
    }
};

#endif // USE_TBB

//typedef FiFoQueue<TASK_T> FiFoTaskQ;

template<typename QUEUE_TYPE>
class thread_pool
    : public thread_pool_if<typename QUEUE_TYPE::value_type::arg_type>
{
protected:
    typedef typename QUEUE_TYPE::value_type TASK_T;
    typedef typename TASK_T::arg_type arg_type;
    QUEUE_TYPE                tasks_;
    const ThreadPool::FUNCTION_T    taskProc_;

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
        ,taskProc_(ThreadPool::FUNCTION_T())
        ,running_( true )
        ,tasks_(task_size)
        ,workingThreads_(0)
        ,NTHREADS(pool_size?pool_size:1)
    {
        for ( std::size_t i = 0; i < NTHREADS; ++i )
            threads_.create_thread( boost::bind( &thread_pool::pool_main, this ) ) ;
    }
    /// @brief Constructor.
    thread_pool( ThreadPool::FUNCTION_T func, std::size_t pool_size, size_t task_size )
        :
        maxTasks_(task_size)
       ,taskProc_(func)
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

    virtual bool run_task(ThreadPool::FUNCTION_T f, const arg_type& param)
    {
        if( f == ThreadPool::FUNCTION_T() && taskProc_ == ThreadPool::FUNCTION_T() ) return false;
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
                    TASK2_T<arg_type> task(task, taskProc_);
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
    typedef typename QUEUE_TYPE::value_type TASK_T;
    typedef typename TASK_T::arg_type arg_type;

    thread_one(size_t task_size)
        : thread_pool<QUEUE_TYPE>(1, task_size)
    {}
    thread_one(ThreadPool::FUNCTION_T fun, size_t task_size)
        : thread_pool<QUEUE_TYPE>(fun, 1, task_size)
    {}
    virtual bool run_task(ThreadPool::FUNCTION_T f, const arg_type& param) {
        if( f == ThreadPool::FUNCTION_T() && thread_pool<QUEUE_TYPE>::taskProc_ == ThreadPool::FUNCTION_T() ) return false;
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
                    {
                        TASK2_T<arg_type> task2(task, thread_pool<QUEUE_TYPE>::taskProc_);
                        task2();
                    }
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
#ifdef USE_TBB
                         ((void*)(new thread_concurrentQ(pool_size, task_size)) )
#else
                         (task_size==0? (void*)(new thread_pool<StdQueue<TASK1_T<FUNCTION_ARG_T> > >(pool_size, task_size))
                                        : (void*)(new thread_pool<FifoQueue<TASK1_T<FUNCTION_ARG_T> > >(pool_size, task_size)) )
#endif
                         :(task_size==0? (void*)(new thread_one<StdQueue<TASK1_T<FUNCTION_ARG_T> > >(task_size))
                                        : (void*)(new thread_one<FifoQueue<TASK1_T<FUNCTION_ARG_T> > >(task_size)) )
             ){};

ThreadPool::ThreadPool( FUNCTION_T fun, std::size_t pool_size, size_t task_size )
 //: priv((void*)(new thread_one<StdTaskQ>(task_size)))
    //: priv((void*)(new thread_concurrentQ(pool_size, task_size)))
    : priv( pool_size>1?
#ifdef USE_TBB
                         ((void*)(new thread_concurrentQ(fun, pool_size, task_size)) )
#else
                         (task_size==0? (void*)(new thread_pool<StdQueue<TASK2_T<FUNCTION_ARG_T> > >(fun, pool_size, task_size))
                                        : (void*)(new thread_pool<FifoQueue<TASK2_T<FUNCTION_ARG_T> > >(fun, pool_size, task_size)) )
#endif
                         :(task_size==0? (void*)(new thread_one<StdQueue<TASK2_T<FUNCTION_ARG_T> > >(fun, task_size))
                                        : (void*)(new thread_one<FifoQueue<TASK2_T<FUNCTION_ARG_T> > >(fun, task_size)) )
             ){};

ThreadPool::~ThreadPool()
{
    delete ((thread_pool_if<FUNCTION_ARG_T>*)priv);
}
bool ThreadPool::post(FUNCTION_T pFun, const FUNCTION_ARG_T& pParam)
{
    return ((thread_pool_if<FUNCTION_ARG_T>*)priv)->run_task(pFun, pParam);
}
bool ThreadPool::post(const FUNCTION_ARG_T& pParam)
{
    return ((thread_pool_if<FUNCTION_ARG_T>*)priv)->run_task(FUNCTION_T(), pParam);
}
size_t ThreadPool::task_count()
{
    return ((thread_pool_if<FUNCTION_ARG_T>*)priv)->task_count();
}
size_t ThreadPool::working_count()
{
    return ((thread_pool_if<FUNCTION_ARG_T>*)priv)->working_count();
}
