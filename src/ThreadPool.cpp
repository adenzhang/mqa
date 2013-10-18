#include "ThreadPool.h"
#include <queue>
#include <boost/thread.hpp>

#include "ftl/FifoPool.h"

struct TASK_T
{
    TASK_T(ThreadPool::FUNCTION_T f, void *p)
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

};
typedef FiFoQueue<TASK_T> FiFoTaskQ;
template<typename QUEUE_TYPE>
class thread_pool
    : public thread_pool_if
{
private:
    QUEUE_TYPE         tasks_;

    boost::thread_group threads_;
    boost::mutex mutex_;
    boost::condition_variable condition_;
    bool running_;
    size_t       maxTasks_;
public:

    /// @brief Constructor.
    thread_pool( std::size_t pool_size, size_t task_size )
        : 
        maxTasks_(task_size)
        ,running_( true )
        ,tasks_(task_size)
    {
        for ( std::size_t i = 0; i < pool_size; ++i )
        {
            threads_.create_thread( boost::bind( &thread_pool::pool_main, this ) ) ;
        }
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

    /// @brief Add task to the thread pool if a thread is currently available.
    template < typename Task >
    void run_task( Task task )
    {
        //boost::unique_lock< boost::mutex > lock( mutex_ );

        //// If no threads are available, then return.
        //if ( 0 == available_ ) return;

        //// Decrement count, indicating thread is no longer available.
        //--available_;

        //// Set task and signal condition variable so that a worker thread will
        //// wake up andl use the task.
        //tasks_.push( boost::function< void() >( task ) );
        //condition_.notify_one();
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
    bool run_task(TASK_T& task )
    {
        boost::unique_lock< boost::mutex > lock( mutex_ );

        // If no threads are available, then return.
        //if ( 0 == available_ ) return false;

        // Decrement count, indicating thread is no longer available.
        //--available_;
        if( !tasks_.push( task ) )
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
private:
    /// @brief Entry point for pool threads.
    void pool_main()
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
                TASK_T task = tasks_.front();
                tasks_.pop();

                lock.unlock();

                // Run the task.
                try
                {
                    task();
                }
                // Suppress all exceptions.
                catch ( ... ) {}
            }

            // Task has finished, so increment count of available threads.
            //lock.lock();
            //++available_;
        } // while running_
    }
};
ThreadPool::ThreadPool( std::size_t pool_size, size_t task_size )
    : priv(task_size==0? (void*)(new thread_pool<StdTaskQ>(pool_size, task_size)) : (void*)(new thread_pool<FiFoTaskQ>(pool_size, task_size)) )
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
